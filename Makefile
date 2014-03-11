
GCC ?= gcc
all:
	$(GCC) i2c-control.c -o bin/i2c-control
	
clean:
	rm bin/i2c-control
