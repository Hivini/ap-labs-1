
include ../../common.mk

build:
	gcc -c testLogger.c logger.c
	gcc -o testLogger testLogger.c logger.c

test:
	make build
	./testLogger