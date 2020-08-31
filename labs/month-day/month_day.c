#include <stdio.h>
#include <stdlib.h>

#define ERROR_YEARDAY 1

static char daytab[2][12] = { {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} };

static char *months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

/* month_day function's prototype*/
void month_day(long int year, long int yearday, int *pmonth, int *pday, int *error) {
    int y, month, day, maxDays;
    long int i, leap;
    leap = year % 4 == 0 && year % 100 != 0 || year % 400 == 0;
    maxDays = leap ? 366 : 365;
    if (yearday > maxDays || yearday < 1) {
        *error = ERROR_YEARDAY;
        return;
    }

    month = 0;
    i = yearday;
    while (1) {
        day = 0;
        y = daytab[leap][month];
        for (y; y > 0; y--) {
            day++;
            i--;
            if (i == 0) {
                *pmonth = month;
                *pday = day;
                return;
            }
        }
        month++;
    }
}

int main(int argc, char **argv) {
    long int year, yearday;
    int pmonth, pday, error;

    if (argc != 3) {
        printf("Not valid arguments, correct usage: ./month_day <year> <yearday>\n");
        return 0;
    } else {
        year = strtol(argv[1], NULL, 10);
        yearday = strtol(argv[2], NULL, 10);
        month_day(year, yearday, &pmonth, &pday, &error);
        if (error == ERROR_YEARDAY) {
            printf("%ld is not a valid yearday, has to be between 1 and 365, or 366 if is a leap year\n", yearday);
            return 0;
        }
        if (year < 1) {
            printf("INFO: Negative numbers are expected to be B.C.");
        }
        printf("%s %02d, %ld\n", months[pmonth], pday, year);
    }
    return 0;
}
