/*
 * main.cpp
 *
 *  Created on: Dec 24, 2013
 *      Author: lynx
 */

/* TODO:
 * Let's build a camera based line follower robot!!!
 * 1. Open up the camera
 * 2. Detect lines!
 * 3. Drive motor speed based on the gradient of line
 */

#include <iostream>
#include <thread>
#include <omp.h>

#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"	// canny

#define __WINDOW_NAME__ "VIDEO"
#define __WINDOW_CANNY__ "CANNY"

using namespace std;

cv::VideoCapture *cap = new cv::VideoCapture;
cv::Mat			 *frm = new cv::Mat;
std::string filename, opt;
cv::Mat			 binary;

int main(int n, char** arg) {
	if(n == 1) {
		cap->open(1);
	} else if(n > 1) {
		filename = arg[1];
		cap->open(filename);
	}

	if(cap->isOpened()) {
		cv::namedWindow(__WINDOW_NAME__, CV_WINDOW_AUTOSIZE);
		cv::namedWindow(__WINDOW_CANNY__, CV_WINDOW_AUTOSIZE);
		while(1) {
			cap->operator >>(*frm);
			cv::Canny(*frm, binary, 128, 200, 3);


			cv::imshow(__WINDOW_CANNY__, binary);
			cv::imshow(__WINDOW_NAME__, *frm);
			if(cv::waitKey(30) > 0) {
				cap->release();
				cv::destroyWindow(__WINDOW_CANNY__);
				cv::destroyWindow(__WINDOW_NAME__);
				break;
			}
		}
	}

	delete cap;
	delete frm;

	return 0;
}


