#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "trebuchet.h"
#define ONLY_DIGIT 0
#define DIGIT_AND_SPELL 1

void read_puzzle(char *filepath) {
    Max_Lines = 0;
    FILE *file = fopen(filepath, "r");
    if(file == NULL) {
        perror("unable to open file");
        exit(1);
    }
    char line[LINEMAX];
    while(fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line,"\n")] = 0;
        Lines[Max_Lines] = (char *)malloc(strlen(line)+1);
        if(Lines[Max_Lines] == NULL) {
            perror("unable to allot memory");
            exit(1);
        }
        strcpy(Lines[Max_Lines], line);
        Max_Lines++;
    }
}

void free_puzzle() {
    for(int i=0; i<Max_Lines; i++)
        free(Lines[i]);
}

int spelled_digit(int *digit, char *srce) {
    char spelled_digits[10][6] = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
    for(int i=0; i < 10; i++) {
        char *spelled = spelled_digits[i];
        int length = strlen(spelled);
        if(!strncmp(spelled, srce, length)) {
            *digit = i;
            return 1;
        }
    }
    return 0;
}

int spelled_or_digit(int *digit, char *srce, int spell) {
    if(isdigit(*srce)) {
        *digit = *srce - '0';
        return 1;
    }
    if(spell && spelled_digit(digit, srce)) {
        return 1;
    }
    return 0;
}

int digit_calibration_value(char *line, int spell) {
    int digit1;
    int digit2;
    for(int i=0; i < strlen(line); i++) {
        if(spelled_or_digit(&digit1, &line[i], spell)) {
            break;
        }
    }
    for(int i=strlen(line)-1; i>=0; i--) {
        if(spelled_or_digit(&digit2, &line[i], spell)) {
            break;
        }
    }
    return digit1 * 10 + digit2;
}

int sum_calibration_values(int spell) {
    int result = 0;
    for(int i=0; i < Max_Lines; i++) {
        int value = digit_calibration_value(Lines[i], spell);
        result += value;
    }
    return result;
}
