#include <stdio.h>
#include <stdlib.h>

#define   LOWER  0       /* lower limit of table */
#define   UPPER  300     /* upper limit */
#define   STEP   20      /* step size */

double convertToCelsius(double fahrenheit) {
	return (fahrenheit - 32) * 5 / 9;
}

void printComparison(double fahrenheit, double celsius) {
	printf("Fahrenheit: \t%1.2f, Celsius: \t%1.2f\n", fahrenheit, celsius);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("Arguments are needed to run this program.\n");
		return 1;
	} else {
		// Return the conversion to celsius of the given argument.
		if (argc == 2) {
			double parsedNumber = strtod(argv[1], NULL);
			double celsius = convertToCelsius(parsedNumber);
			printComparison(parsedNumber, celsius);
		}
		// Return a table of conversions with start, end and increment represented in the arguments. 
		else if (argc == 4) {
			double start = strtod(argv[1], NULL);
			double end = strtod(argv[2], NULL);
			double increment = strtod(argv[3], NULL);
			if (start > end) {
				printf("Start [%05.2f] is greater than end [%05.2f].\n", start, end);
				return 2;
			}
			for (int i = start; i <= end; i += increment) {
				printComparison(i, convertToCelsius(i));
			}
		} else {
			printf("No valid number of arguments are provided.\n");
			return 3;
		}
	}
    return 0;
}