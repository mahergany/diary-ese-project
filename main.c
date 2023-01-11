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

Cell grid[ROWS][COLS];

// function declarations
int getCurrentDay(int date, int month, int year);
int getNumberofDays(int month, int year);
void generateCalendar(int month, int year);
int checkLeapYear(int year);
void gridDraw(Cell cell);
void enterText(Cell cell);
void monthOption(int month);
void yearOption(int currentYear);
void saveFile(char currentEntry[], int date, int month, int year);
void getPreviousEntry(int date, int month, int year);

char *days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
char *months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
int dateButtons[31] = {0};
int mainScreen = 1;
int dateScreen = 0;

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
// dates datePage[4][12][31];
int datePage[4][13][32] = {0}; // datePage[yearIndex][monthIndex][dateIndex]
/*
yearIndex: 0 = 2021, 1 = 2022, 2 = 2023, 3 = 2024
month number and day number corresponds to their respective indices (for aasani ;-;)
*/
char path[50];
char fileName[16] = "";

int main()
{

    InitWindow(screenWidth, screenHeight, "Window");

    Texture2D neutral = LoadTexture("resources/neutral-face.png");
    Texture2D happy = LoadTexture("resources/grinning-face.png");
    Texture2D sad = LoadTexture("resources/frowning-face.png");

    SetTargetFPS(60);
    for (int j = 0; j < ROWS; j++)
    {
        for (int i = 0; i < COLS; i++)
        {
            grid[i][j] = (Cell){
                .i = i,
                .j = j,
            };
        }
    }

    while (!WindowShouldClose())
    {
        // if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        // {
        //     Vector2 mousePos = GetMousePosition();
        //     int mousePosX = mousePos.x;
        //     int mousePosY = mousePos.y;
        // }
        char *title = "Sentiment Tracking Diary";
        SetWindowTitle(title);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (mainScreen)
        {
            entry[0] = '\0';
            for (int i = 0; i < COLS; i++)
                for (int j = 0; j < ROWS; j++)
                    gridDraw(grid[i][j]);

            // DrawTexturePro(neutral, (Rectangle){cellWidth, cellHeight, 100, 200}, (Rectangle){cellWidth, cellHeight, 100, 200}, (Vector2){100, 100}, 0, WHITE);

            // print day names
            // TODO: reformat day names column
            for (int i = 0; i <= 6; i++)
            {
                DrawText(days[i], gridX + grid[i][0].i * cellWidth + 25, gridY + grid[i][0].j * cellHeight - 35, 28, BLACK);
            }

            monthOption(currentMonth);
            yearOption(currentYear);
            generateCalendar(currentMonth, currentYear);

            // if month buttons are clicked
            if (GuiButton((Rectangle){gridX, gridY - 110, 40, 40}, "#114#"))
            {
                currentMonth--;
                if (currentMonth == 0)
                    currentMonth = 12;
                entry[0] = '\0';
                monthOption(currentMonth);
                generateCalendar(currentMonth, currentYear);
            }
            if (GuiButton((Rectangle){gridX + 300, gridY - 110, 40, 40}, "#115#"))
            {
                currentMonth++;
                if (currentMonth == 13)
                    currentMonth = 1;
                monthOption(currentMonth);
                generateCalendar(currentMonth, currentYear);
            }
            // if year buttons are clicked
            if (GuiButton((Rectangle){gridX + 350, gridY - 110, 40, 40}, "#114#") && currentYear > 2021)
            {
                currentYear--;
                yearOption(currentYear);
                generateCalendar(currentMonth, currentYear);
            }
            if (GuiButton((Rectangle){gridX + 550, gridY - 110, 40, 40}, "#115#") && currentYear < 2024)
            {
                currentYear++;
                yearOption(currentYear);
                generateCalendar(currentMonth, currentYear);
            }
            fileName[0] = '\0';
            path[0] = '\0';
            datePage[1][12][30] = 1;
            get = 1;
        }
        for (int i = 1; i <= getNumberofDays(currentMonth, currentYear); i++)
        {
            // getPreviousEntry(i, currentMonth, currentYear);
            //  entry[ENTRYSIZE] = *LoadFileText(path);
            if (dateButtons[i])
            {
                // entry[ENTRYSIZE] = '\0';
                // getPreviousEntry(i, currentMonth, currentYear);
                mainScreen = 0;
                dateScreen = 1;
                char date[3], month[15], year[5];
                itoa(i, date, 10);
                itoa(currentMonth, month, 10);
                itoa(currentYear, year, 10);
                char *pageTitle = strcat(strcat(date, "/"), strcat(strcat(month, "/"), year));
                // if (FileExists(path))
                // {
                //     // getPreviousEntry(i, currentMonth, currentYear);
                //     sprintf(fileName, "%d-%d-%d.txt", i, currentMonth, currentYear);
                //     sprintf(path, "C:\\Users\\PC\\Desktop\\diary entries\\%s", fileName);
                //     entry[ENTRYSIZE] = *LoadFileText(path);
                // }
                // else
                //     entry[0] = '\0';
                // if (get)
                // {
                //     sprintf(fileName, "%d-%d-%d.txt", i, currentMonth, currentYear);
                //     sprintf(path, "C:\\Users\\PC\\Desktop\\diary entries\\%s", fileName);
                //     if (FileExists(path))
                //     {
                //         FILE *ptr;
                //         ptr = fopen(path, "r");
                //         fgets(entry, 500, ptr);
                //         fclose(ptr);
                //     }
                //     else
                //         entry[0] = '\0';
                //     get = 0;
                // }
                // ClearBackground(RAYWHITE);
                if (dateScreen)
                {
                    if (get)
                    {
                        sprintf(fileName, "%d-%d-%d.txt", i, currentMonth, currentYear);
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
                        get = 0;
                    }
                    DrawText(pageTitle, 50, 50, 100, BLACK);
                    if (GuiButton((Rectangle){1400, 750, 125, 75}, "Back"))
                    {
                        mainScreen = 1;
                        dateScreen = 0;
                    }
                    if (GuiButton((Rectangle){1200, 750, 125, 75}, "Save"))
                    {
                        if (strcmp(entry, "") != 0)
                        {
                            saveFile(entry, i, currentMonth, currentYear);
                        }
                    }
                    bool textBoxEditMode = true;
                    if (GuiTextBoxMulti((Rectangle){50, 175, 750, 600}, entry, ENTRYSIZE, textBoxEditMode))
                        textBoxEditMode = !textBoxEditMode;
                }
            }
        }
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
void gridDraw(Cell cell)
{
    DrawRectangleLines(gridX + cell.i * cellWidth, gridY + cell.j * cellHeight, cellWidth, cellHeight, BLACK);
}
void generateCalendar(int month, int year)
{
    int n = getNumberofDays(month, year);
    // bool dateButtons[31] = {false};
    // memset(dateButtons, 0, sizeof(dateButtons));
    int rowCount = 0;
    int colIndex;
    for (int col = 0; col < getCurrentDay(1, month, year); col++)
        DrawRectangle(gridX + grid[col][0].i * cellWidth, gridY + grid[col][0].j * cellHeight, cellWidth, cellHeight, DARKGRAY);
    for (int date = 1; date <= n; date++)
    {
        colIndex = getCurrentDay(date, month, year);
        char str_date[5];
        itoa(date, str_date, 10);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 22);
        dateButtons[date] = GuiButton((Rectangle){gridX + grid[colIndex][rowCount].i * cellWidth + 140, gridY + grid[colIndex][rowCount].j * cellHeight, 35, 35}, str_date);
        // GuiButton((Rectangle){gridX + grid[colIndex][rowCount].i * cellWidth + 140, gridY + grid[colIndex][rowCount].j * cellHeight, 35, 35}, str_date);

        // to check if file is saving correctly:
        // sprintf(fileName, "%d-%d-%d.txt", date, currentMonth, currentYear);
        // sprintf(path, "C:\\Users\\PC\\Desktop\\diary entries\\%s", fileName);
        // char a[3];
        // itoa(FileExists(path), a, 10);
        // DrawText(a, gridX + grid[colIndex][rowCount].i * cellWidth + 5, gridY + grid[colIndex][rowCount].j * cellHeight, 30, BLACK);
        if (colIndex == 6 && date != n)
        {
            rowCount++;
        }
        // char *pageTitle = strcat(strcat(date, "/"), strcat(strcat(month, "/"), year));
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
void monthOption(int currentMonth)
{
    // GuiDrawRectangle((Rectangle)(gridX + 50, gridY - 110, 300, 40), 1, RAYWHITE, RAYWHITE);
    DrawText(months[currentMonth - 1], gridX + 50, gridY - 110, 50, BLACK);
}
void yearOption(int currentYear)
{
    char year[5];
    itoa(currentYear, year, 10);
    DrawText(year, gridX + 400, gridY - 110, 50, BLACK);
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