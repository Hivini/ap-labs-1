
include ../../common.mk

compile:
	gcc -o fahrenheit_celsius fahrenheit_celsius.c

sample-conversion:
	make compile
	./fahrenheit_celsius 10

sample-range-conversion:
	make compile
	./fahrenheit_celsius 0 100 10

conversion:
	make compile
	./fahrenheit_celsius ${temperature}

range-conversion:
	make compile
	./fahrenheit_celsius ${start} ${end} ${increment}

remove:
	rm fahrenheit_celsius
