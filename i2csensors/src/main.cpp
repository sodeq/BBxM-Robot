/*
 * main.cpp
 *
 *  Created on: Jan 15, 2014
 *      Author: lynx
 */



//#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include "from-u-boot/i2c_interface.h"
#include "from-u-boot/enable_i2c_clocks.h"


using namespace std;

int main(int n, char** arg) {
	size_t i;
	int version;
	int res = enable_i2c_clocls();
	uint8_t curr_addr;
	uint8_t to_send[4] = {0xA0, 0xAA, 0xA5, 0x00};

	if(n != 3) {
		printf("Usage: changei2addr <old_addr_hex> <new_addr_hex>\n");
		return -1;	// error code
	}

	if(res) {
		printf("Error enabling I2C clocks: %i\n", res);
		return res;	// i2c reading failed
	}

	// 100 kHz
	i2c_init(100000, 1);

	usleep(100 * 1000);
	curr_addr = strtol(arg[1], NULL, 16);
	curr_addr >>=1; // translate to 7-bit format
	to_send[3] = strtol(arg[2], NULL, 16);
	printf("changing 0x%X to 0x%X. Press enter to confirm!", curr_addr, to_send[3]);
	getchar();

	usleep(100*1000);
	version = i2c_reg_read(curr_addr, 0);
	printf("firmware version read from old address (%X): %i\n", curr_addr, version);

	usleep(100*1000);
	for(i = 0; i < sizeof(to_send)/sizeof(to_send[0]); ++i)
		i2c_reg_write(curr_addr, 0, to_send[i]);

	usleep(100*1000);
	version = i2c_reg_read(to_send[3] >> 1, 0);
	printf("Firmware version read from new address (%X): %i\n", to_send[3], version);

	i2c_uninit();


	return 0;	// success code
}
