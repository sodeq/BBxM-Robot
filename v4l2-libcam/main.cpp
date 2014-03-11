/*
 * main.cpp
 *
 *  Created on: Dec 24, 2013
 *      Author: lynx
 */


/*****************************************************\
 * open camera
\*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>	// sleep()
#include "libcam.h"

using namespace std;

void Help() {
	printf("usage: <app_name> <dev_path>\n");
}

int main(int n, char** arg) {
	if(n==1) {

		// basic funtionality
		Camera c("/dev/video0", 640, 480);
		c.Update();
		for(int i = 0; i < 640; i++)
		{
			for(int j = 0; j < 480; j++)
				cout << (int)c.data[i*j+j] << "\t";
			cout << endl;
		}

	} else if(n>1) {

		Help();
		// single webcam demo
		int ww = 320;
		int hh = 240;
		int fps = 30;
		const char *device = "/dev/video0";

		// parse command line
		for(int i = 0; i < n; i++) {
			std::string a = arg[i];

			if(a == "-w") {
				ww = atoi(arg[i+1]);
			} else if( a == "-h" ) {
				hh = atoi(arg[i+1]);
			} else if( a == "-d" ) {
				device = arg[i+1];
			} else if( a == "-f" ) {
				fps = atoi(arg[i+1]);
			}
		}

		// instance a camera object
		Camera cam(device, ww , hh, fps);
		// allocate momory for the grabbed frames
		unsigned char *img = new unsigned char[ww*hh*4];

		int itt = 0;
		while( itt < 20) {
			cam.Update();
			printf("frame grabbed %d --> data[0]: %i\n", itt, cam.data[0]);
			itt++;
		}

		delete img;
	}

	return 0;
}























