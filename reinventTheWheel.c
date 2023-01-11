#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "raygui.h"

#define ROWS 6
#define COLS 7
#define ENTRYSIZE 500
#define INPUTSIZE 5000

typedef struct
{
    int i;    // horizontal index
    int j;    // vertical index
    int date; // date corresponding to index
} Cell;

typedef struct
{
    int hasEntry;
    int date;
    int month;
    int year;
} dates;
typedef struct
{
    Rectangle box;
    bool isVisible;
    char date[10];
    bool isEntryPresent;
    int mood;
} dayBox;

enum
{
    MAIN_LAYER,
    INPUT_LAYER
};

Cell grid[ROWS][COLS];
char *days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
char *months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
int dateButtons[31] = {0};

// !DELETE
int mainScreen = 1;
int dateScreen = 0;

int currentLayer = MAIN_LAYER;

int currentMonth = 1;
int currentYear = 2023;

int screenWidth = 1536;
int screenHeight = 864;

int cellWidth = 175;
int cellHeight = 100;

int gridX = 140;
int gridY = 200;

int get;
char entry[ENTRYSIZE] = "";
char dateString[50] = "";

// dates datePage[4][12][31];
bool datePage[4][13][32] = {0}; // datePage[yearIndex][monthIndex][dateIndex]
/*
yearIndex: 0 = 2021, 1 = 2022, 2 = 2023, 3 = 2024
month number and day number corresponds to their respective indices (for aasani ;-;)
*/
char path[50];
char fileName[16] = "";

dayBox dayBoxes[42];

// function declarations
int getCurrentDay(int date, int month, int year);
int getNumberofDays(int month, int year);
void generateCalendar(int month, int year);
int checkLeapYear(int year);
void gridDraw(Cell cell, Color color);
void enterText(Cell cell);
void monthOption(int month);
void yearOption(int currentYear);
void saveFile(char currentEntry[], int date, int month, int year);
void getPreviousEntry(int date, int month, int year);

void drawMonthPicker();

int main()
{
    char clickedDate[20] = {0};
    char userInput[INPUTSIZE] = {0};

    InitWindow(screenWidth, screenHeight, "Window");
    SetWindowTitle("Sentiment Tracking Diary");

    Texture2D neutral = LoadTexture("resources/neutral-face.png");
    Texture2D happy = LoadTexture("resources/grinning-face.png");
    Texture2D sad = LoadTexture("resources/frowning-face.png");

    SetTargetFPS(60);

    for (int j = 0; j < ROWS; j++)
    {
        for (int i = 0; i < COLS; i++)
        {
            grid[i][j] = (Cell){i, j};
        }
    }

    while (!WindowShouldClose())
    {
        int numberOfDays = getNumberofDays(currentMonth, currentYear);
        int colIndex = getCurrentDay(1, currentMonth, currentYear);
        int index = 0;
        bool startMakingVisibile = false;

        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                // Box for this day
                index = i * 7 + j + 1;
                dayBoxes[index].box.x = gridX + grid[j][i].i * cellWidth;
                dayBoxes[index].box.y = gridY + grid[j][i].j * cellHeight;
                dayBoxes[index].box.width = cellWidth - 2;
                dayBoxes[index].box.height = cellHeight - 2;

                // isItVisible?
                if (index == colIndex + 1)
                {
                    startMakingVisibile = true;
                }
                if (startMakingVisibile && colIndex + numberOfDays >= index)
                {
                    dayBoxes[index].isVisible = true;
                }
                else
                {
                    dayBoxes[index].isVisible = false;
                }

                // Setting up the text
                if (dayBoxes[index].isVisible)
                {
                    char str_date[5];
                    itoa(index - colIndex, str_date, 10);
                    strcpy(dayBoxes[index].date, str_date);
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentLayer == MAIN_LAYER)
        {
            drawMonthPicker();

            // Draw day names
            for (int i = 0; i <= 6; i++)
            {
                DrawText(days[i], gridX + grid[i][0].i * cellWidth + ((cellWidth - MeasureText(days[i], 28)) / 2), gridY + grid[i][0].j * cellHeight - 35, 28, BLACK);
            }

            // Drawing the boxes
            for (int i = 0; i < 42; i++)
            {
                if (!dayBoxes[i].isVisible)
                    continue;
                DrawRectangleRounded(dayBoxes[i].box, 0.1, 1000, CheckCollisionPointRec(GetMousePosition(), dayBoxes[i].box) ? (Color){178, 190, 195, 255} : (Color){223, 230, 233, 255});
                DrawText(dayBoxes[i].date, dayBoxes[i].box.x + 8, dayBoxes[i].box.y + 8, 20, BLACK);

                if (CheckCollisionPointRec(GetMousePosition(), dayBoxes[i].box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    strcpy(clickedDate, dayBoxes[i].date);
                    currentLayer = INPUT_LAYER;
                }
            }
        }
        if (currentLayer == INPUT_LAYER)
        {
            char month[40] = {0}, year[8] = {0};
            char fullDate[50] = {0};

            itoa(currentMonth, month, 10);
            itoa(currentYear, year, 10);

            strcpy(fullDate, clickedDate);
            strcat(fullDate, "/");
            strcat(fullDate, month);
            strcat(fullDate, "/");
            strcat(fullDate, year);

            // sprintf(fullDate, "%d-%d-%d", clickedDate, currentMonth, currentYear);
            DrawText(fullDate, gridX, gridY - 110, 64, DARKGRAY);
        }

        // draw the calendar
        // if (mainScreen)
        // {
        //     generateCalendar(currentMonth, currentYear);

        // for (int i = 1; i <= getNumberofDays(currentMonth, currentYear); i++)
        // {
        //     // getPreviousEntry(i, currentMonth, currentYear);
        //     //  entry[ENTRYSIZE] = *LoadFileText(path);
        //     if (dateButtons[i])
        //     {
        //         // entry[ENTRYSIZE] = '\0';
        //         // getPreviousEntry(i, currentMonth, currentYear);
        //         mainScreen = 0;
        //         dateScreen = 1;
        //         char date[3], month[15], year[5];
        //         itoa(i, date, 10);
        //         itoa(currentMonth, month, 10);
        //         itoa(currentYear, year, 10);
        //         char *pageTitle = strcat(strcat(date, "/"), strcat(strcat(month, "/"), year));
        //         // if (FileExists(path))
        //         // {
        //         //     // getPreviousEntry(i, currentMonth, currentYear);
        //         //     sprintf(fileName, "%d-%d-%d.txt", i, currentMonth, currentYear);
        //         //     sprintf(path, "C:\\Users\\PC\\Desktop\\diary entries\\%s", fileName);
        //         //     entry[ENTRYSIZE] = *LoadFileText(path);
        //         // }
        //         // else
        //         //     entry[0] = '\0';
        //         // if (get)
        //         // {
        //         //     sprintf(fileName, "%d-%d-%d.txt", i, currentMonth, currentYear);
        //         //     sprintf(path, "C:\\Users\\PC\\Desktop\\diary entries\\%s", fileName);
        //         //     if (FileExists(path))
        //         //     {
        //         //         FILE *ptr;
        //         //         ptr = fopen(path, "r");
        //         //         fgets(entry, 500, ptr);
        //         //         fclose(ptr);
        //         //     }
        //         //     else
        //         //         entry[0] = '\0';
        //         //     get = 0;
        //         // }
        //         // ClearBackground(RAYWHITE);
        //         if (dateScreen)
        //         {
        //             if (get)
        //             {
        //                 sprintf(fileName, "%d-%d-%d.txt", i, currentMonth, currentYear);
        //                 sprintf(path, "C:\\Users\\PC\\Desktop\\diary entries\\%s", fileName);
        //                 if (FileExists(path))
        //                 {
        //                     FILE *ptr;
        //                     ptr = fopen(path, "r");
        //                     fgets(entry, 500, ptr);
        //                     fclose(ptr);
        //                 }
        //                 else
        //                     entry[0] = '\0';
        //                 get = 0;
        //             }
        //             DrawText(pageTitle, 50, 50, 100, BLACK);
        //             if (GuiButton((Rectangle){1400, 750, 125, 75}, "Back"))
        //             {
        //                 mainScreen = 1;
        //                 dateScreen = 0;
        //             }
        //             if (GuiButton((Rectangle){1200, 750, 125, 75}, "Save"))
        //             {
        //                 if (strcmp(entry, "") != 0)
        //                 {
        //                     saveFile(entry, i, currentMonth, currentYear);
        //                 }
        //             }
        //             bool textBoxEditMode = true;
        //             if (GuiTextBoxMulti((Rectangle){50, 175, 750, 600}, entry, ENTRYSIZE, textBoxEditMode))
        //                 textBoxEditMode = !textBoxEditMode;
        //         }
        //     }
        // }
        EndDrawing();
    }
}

int checkLeapYear(int year)
{
    if ((year % 4 == 0) && ((year % 400 == 0) || (year % 100 != 0)))
        return 1;
    return 0;
}
int getNumberofDays(int month, int year)
{
    switch (month)
    {
    case 1:
        return 31;
    case 2:
        return (checkLeapYear(year) ? 29 : 28);
    case 3:
        return 31;
    case 4:
        return 30;
    case 5:
        return 31;
    case 6:
        return 30;
    case 7:
        return 31;
    case 8:
        return 31;
    case 9:
        return 30;
    case 10:
        return 31;
    case 11:
        return 30;
    case 12:
        return 31;
    }
    return -1;
}
int getCurrentDay(int date, int month, int year) // algorithm that returns jth index for day
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 3;
    int dayIndex = (year + year / 4 - year / 100 + year / 400 + t[month - 1] + date) % 7;
    if (dayIndex == 0)
        dayIndex = 6;
    else
        dayIndex -= 1;
    return dayIndex;
}
void gridDraw(Cell cell, Color color)
{
    // DrawRectangleLines(gridX + cell.i * cellWidth, gridY + cell.j * cellHeight, cellWidth, cellHeight, BLACK);
    // DrawRectangleLines(gridX + cell.i * cellWidth, gridY + cell.j * cellHeight, cellWidth, cellHeight, BLACK);
}
void generateCalendar(int month, int year)
{
    int n = getNumberofDays(month, year);
    int rowCount = 0;
    int colIndex;
    for (int date = 1; date <= n; date++)
    {
        colIndex = getCurrentDay(date, month, year);
        char str_date[5];
        itoa(date, str_date, 10);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 22);
        dateButtons[date] = GuiButton((Rectangle){gridX + grid[colIndex][rowCount].i * cellWidth + 140, gridY + grid[colIndex][rowCount].j * cellHeight, 35, 35}, str_date);
        if (colIndex == 6 && date != n)
        {
            rowCount++;
        }
    }
    if (rowCount == 4 && colIndex == 6)
        for (int i = 0; i < COLS; i++)
            DrawRectangle(gridX + grid[i][5].i * cellWidth, gridY + grid[i][5].j * cellHeight, cellWidth, cellHeight, DARKGRAY);
    else if (rowCount == 4)
    {
        for (int i = colIndex + 1; i < COLS; i++)
            DrawRectangle(gridX + grid[i][4].i * cellWidth, gridY + grid[i][4].j * cellHeight, cellWidth, cellHeight, DARKGRAY);
        for (int i = 0; i < COLS; i++)
            DrawRectangle(gridX + grid[i][5].i * cellWidth, gridY + grid[i][5].j * cellHeight, cellWidth, cellHeight, DARKGRAY);
    }
    else if ((rowCount == 5) || (rowCount == 4 && colIndex == 6))
        for (int i = colIndex + 1; i < COLS; i++)
            DrawRectangle(gridX + grid[i][5].i * cellWidth, gridY + grid[i][5].j * cellHeight, cellWidth, cellHeight, DARKGRAY);
}
void saveFile(char currentEntry[], int date, int month, int year)
{
    // char path[50];
    // char fileName[16] = "";
    sprintf(fileName, "%d-%d-%d.txt", date, month, year);
    sprintf(path, "C:\\Users\\PC\\Desktop\\diary entries\\%s", fileName);
    // DrawText(path, 500, 500, 50, BLACK);
    FILE *filePtr = fopen(path, "w");
    fprintf(filePtr, "%s", currentEntry);
    fclose(filePtr);
}
void getPreviousEntry(int date, int month, int year)
{
    sprintf(fileName, "%d-%d-%d.txt", date, month, year);
    sprintf(path, "C:\\Users\\PC\\Desktop\\diary entries\\%s", fileName);
    if (FileExists(path))
    {
        FILE *ptr;
        ptr = fopen(path, "r");
        fgets(entry, 500, ptr);
        fclose(ptr);
    }
    else
        entry[0] = '\0';
}
void drawMonthPicker()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 22);
    strcpy(dateString, months[currentMonth - 1]);
    strcat(dateString, " ");
    char year[5];
    itoa(currentYear, year, 10);
    strcat(dateString, year);

    // TODO: center this thingy
    DrawText(dateString, gridX + 25 + 550 / 2 - MeasureText(dateString, 50) / 2, gridY - 110, 50, BLACK);
    Rectangle button1 = {gridX, gridY - 110, 40, 40};
    Rectangle button2 = {gridX + 550, gridY - 110, 40, 40};

    // rgb(84, 160, 255)
    // rgb(46, 134, 222)
    DrawRectangleRounded(button1, 0.1, 1000, CheckCollisionPointRec(GetMousePosition(), button1) ? (Color){46, 134, 222, 255} : (Color){84, 160, 255, 255});
    DrawRectangleRounded(button2, 0.1, 1000, CheckCollisionPointRec(GetMousePosition(), button2) ? (Color){46, 134, 222, 255} : (Color){84, 160, 255, 255});

    if (CheckCollisionPointRec(GetMousePosition(), button1) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        currentMonth -= 1;

        if (currentMonth == 0)
        {
            currentMonth = 12;
            currentYear -= 1;
        }
    }
    if (CheckCollisionPointRec(GetMousePosition(), button2) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        currentMonth += 1;
        if (currentMonth == 13)
        {
            currentMonth = 1;
            currentYear += 1;
        }
    }
}
