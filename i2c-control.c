/* 
 * elinux.org/interfacing_with_I2C_Devices
 * Example Code for communicating with AD7991
 * CLONED
 */
 

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*void Sensor_ADC_init(void) {
	int file;
	char filename[40];
	//const gchar *buffer;
	//int addr = 0b00101001;	// I2C address of the ADC
	
	sprintf(filename, "/dev/i2c-2");
	if((file = open(filename,O_RDWR)) < 0) {
		printf("Failed to open the bus.\n");
		exit(1);
	}
}*/

/*
void Sensor_ADC_Read(FILE* file) {
	float data;
	char buf[10] = {0};
	char channel;
	
	for(int i = 0; i < 4; ++i) {
		// Using I2C Read
		if(read(file, buf,2) != 2) {
			// ERROR HANDLING  Transaction failed!!!
			printf("FAILED TO READ FROM I2C BUS.\n");
			//buf = fprintf(stderr, errno);
			//printf(buf);
			printf("\n\n");
		} else {
			data = (float)((buf[0] & 0b000011111) << 8 ) + buf[1];
			data = data/4096*5;
			channel = ( (buf[0] & 0b00110000) >> 4);
			printf("Channel %02d Data: %04f\n", channel, data); 
		}
	}
} */

int main(void)
{
	printf("HALLO!!!\n");
	return 0;
}
