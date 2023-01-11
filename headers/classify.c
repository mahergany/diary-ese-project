#include <stdio.h>
#include <string.h>
#include <ctype.h>

int detectTone(char *text);
void filterString(char *str);
void toLowerCase(char *text);
int inFile(char *wordToSearchFor, char *filename);
void removeChar(char *str, char charToRemove);

int main()
{
    char text[1000] = {0};

    printf("Enter your sentence: ");
    gets(text);

    int score = detectTone(text);
    if (score == 0)
        puts("You're feeling neutral.");
    else if (score > 0)
        puts("You're happy.");
    else
        puts("You're not happy.");

    return 0;
}
int detectTone(char *text)
{
    filterString(text);
    toLowerCase(text);
    int positiveWords = 0, negativeWords = 0;
    char *token = strtok(text, " ");

    while (token != NULL)
    {
        if (inFile(token, "negative-words.txt"))
            negativeWords += 1;

        if (inFile(token, "positive-words.txt"))
            positiveWords += 1;

        token = strtok(NULL, " ");
    }

    return positiveWords - negativeWords;
}
void filterString(char *str)
{
    int i, j;
    int len = strlen(str);
    for (i = 0; i < len; i++)
    {
        if (isspace(str[i]) || isalpha(str[i]))
            continue;

        for (j = i; j < len; j++)
        {
            str[j] = str[j + 1];
        }
        len--;
        i--;
    }
}
void toLowerCase(char *text)
{
    for (int i = 0; i < strlen(text); i++)
        text[i] = tolower(text[i]);
}
int inFile(char *wordToSearchFor, char *filename)
{
    FILE *file = fopen(filename, "r");

    char wordInFile[100];

    while (!feof(file))
    {
        fgets(wordInFile, 99, file);
        removeChar(wordInFile, '\n');
        if (strcmp(wordInFile, wordToSearchFor) == 0)
            return 1;
    }

    fclose(file);
    return 0;
}
void removeChar(char *str, char charToRemove)
{
    int i, j;
    int len = strlen(str);
    for (i = 0; i < len; i++)
    {
        if (str[i] == charToRemove)
        {
            for (j = i; j < len; j++)
            {
                str[j] = str[j + 1];
            }
            len--;
            i--;
        }
    }
}