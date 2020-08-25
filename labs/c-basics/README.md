Lab - C Basics
==============
Using basic concepts of C Programming from [C Basics lecture](http://talks.obedmr.com/hello-c-world/00-basics.slide#1),
create a program that calculates the conversion from Farenheit  to Celsius degrees.

Modify the provided `fahrenheit_celsius.c` file to receive the parameters in the following formats:

- Simple conversion
```
# ./fahrenheit_celsius <num_farenheit_degrees>
# Example:
./fahrenheit_celsius 0
Fahrenheit:   0, Celsius:  -17.8
```

- Range conversion
```
# ./fahrenheit_celsius <start> <end> <increment>
# Example:
./fahrenheit_celsius 0 100 10
Fahrenheit:   0, Celsius:  -17.8
Fahrenheit:  20, Celsius:   -6.7
Fahrenheit:  40, Celsius:    4.4
Fahrenheit:  60, Celsius:   15.6
Fahrenheit:  80, Celsius:   26.7
Fahrenheit: 100, Celsius:   37.8
```

# Extra
- Makefile
```
# Example conversion using make command.
make conversion temperature=32

# Example range conversion using make command.
make range-conversion start=0 end=99 increment=10
```
