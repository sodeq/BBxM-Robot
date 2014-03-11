/*
 * main.cpp
 *
 *  Created on: Jan 17, 2014
 *      Author: lynx
 */



#include <signal.h>
#include <time.h>
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

using namespace std;

#include "libcam.h"

class coba {
public:
	coba(int i, int j) : randomized(false){
		this->cols = i;
		this->rows = j;
		data = (unsigned char*)malloc(i*j);
		for(int idx = 0; idx < i*j ; ++idx)
		{
			this->data[idx] = idx;
			printf("[%i]: %i\n", idx, (int)this->data[idx]);
		}

		randomized = true;
	}

	unsigned char operator[] (int i){
		if(!randomized)
			return 0;
		return data[i];
	}

private:
	int cols, rows;
	bool randomized;
	unsigned char *data;
	unsigned char random_0_1() {
		unsigned char d;
		srand(time(0));
		d = (255-0)*((unsigned char)rand()/(unsigned char)RAND_MAX);
		return d;
	}
};

// FIXME!!!
int main(int n, char** arg) {



	// CITATION: imaging.h
	/*CAMERA* cam = new CAMERA;
	cam->InitParam("/dev/video0", 640, 480);
	cam->Open();
	cam->Init();	// TODO: ERROR STILL in IO_METHOD_READ
	cam->operator [](0);*/

	// NOTE <21-01-2014>: trace back to original libcam.h
	if(n == 1) {
		Camera c("/dev/video0", 640, 480);
		c.Update();
		printf("data[0]: %i\n", (int)c.at(0,0));
		printf("data[0]: %i\n", (int)c.getVal(0));
		c.StopCam();
	}

	return 0;
}



















