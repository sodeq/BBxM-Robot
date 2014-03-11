/*
 * camera-uvc.cpp
 *
 *  Created on: Jan 19, 2014
 *      Author: lynx
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/videodev2.h>
#include <linux/videodev.h>
#include <unistd.h>
#include <time.h>

#include "v4l2uvc.h"
#include "helper.h"

//#define NB_BUFFER	16
//#define DHT_SIZE	420


/*struct inIn{// NOTE: camera params
	int fd;
	char *videodevice;
	char *status;
	char *pictName;
	struct v4l2_capability cap;
	struct v4l2_format fmt;
	struct v4l2_buffer buf;
	struct v4l2_requestbuffers rb;
	void *mem[NB_BUFFER];
	unsigned char *tmpbuffer;
	unsigned char *framebuffer;
	int isstreaming;
	int grabmethod;
	int width;
	int height;
	int formatIn;
	int formatOut;
	int framesizeIn;
	int signalquit;
	int toggleAvi;
	int getPict;
};*/




int main(int n, char** arg) {
	char *videodevice = "/dev/video0";
	char *outputfile = "snap.jpg";
	char *post_capture_command[3];
	int format = V4L2_PIX_FMT_MJPEG;
	int grabmethod = 1;
	int width = 320;
	int height = 240;
	int brightness = 0, contrast = 0, saturation = 0, gain = 0;
	int verbose = 0;
	int delay = 0;
	int quality = 95;
	int post_capture_command_wait = 0;
	time_t ref_time;
	struct vdIn *videoIn;
	FILE *file;

	(void) signal (SIGINT, sigcatch);
	(void) signal (SIGQUIT, sigcatch);
	(void) signal (SIGKILL, sigcatch);
	(void) signal (SIGTERM, sigcatch);
	(void) signal (SIGABRT, sigcatch);
	(void) signal (SIGTRAP, sigcatch);

	// set post_capture_command to default values
	post_capture_command[0] = NULL;
	post_capture_command[1] = NULL;
	post_capture_command[2] = NULL;

	if(post_capture_command[0])
		post_capture_command[1] = outputfile;

	videoIn = (struct vdIn *) calloc(1, sizeof(struct vdIn));
	if( init_videoIn(videoIn, videodevice, width, height, format, grabmethod) < 0) {
		fprintf(stderr, "init_videoIn error!!!\n");
		exit(1);
	} else {
		printf("init_videoIn succeeded!!!\n");

		printf("width: %i\theight: %i\n", videoIn->width, videoIn->height);
		printf("grabmethod: %i\tformat: %i\n", videoIn->grabmethod, videoIn->formatOut);
	}

	// NOTE: camera controls
	if(verbose >= 1) fprintf(stderr,"Resetting camera setting\n!");
	v4l2ResetControl(videoIn, V4L2_CID_BRIGHTNESS);
	v4l2ResetControl(videoIn, V4L2_CID_CONTRAST);
	v4l2ResetControl(videoIn, V4L2_CID_SATURATION);
	v4l2ResetControl(videoIn, V4L2_CID_GAIN);



	/*
	 * TODO FIXME URGENT!!!
	 * NOTE: CAPTURING!!!
	 */

	// single capture mode
	if(uvcGrab(videoIn) < 0) {
		fprintf(stderr, "Error capturing!!!\n");
		exit(1);
	} else {
		printf("SUCCEEDED GRAB!!!\n");
	}
	// continuous capturing
	/*while(run) {
		if(verbose >= 2)
			fprintf(stderr, "Grabbing frame\n");
		if(uvcGrab(videoIn) < 0) {
			fprintf(stderr, "Error capturing!!!\n");
			close_v4l2(videoIn);
			free(videoIn);
			exit(1);
		}
	}*/

	return 0;
}

