/*
 * helper.h
 *
 *  Created on: Jan 19, 2014
 *      Author: lynx
 */

#ifndef HELPER_H_
#define HELPER_H_

int run = 1;

void sigcatch (int sig)
{
	fprintf (stderr, "Exiting...\n");
	run = 0;
}

struct _UVC_PARAM_ {
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
};

// C++ wrapper for uvc capturing!!!
class UVC {
private:
	_UVC_PARAM_ uvc_;

	// NOTE: obvious
	void SetDefaultParameter(struct _UVC_PARAM_ *p) {

	}
public:
	UVC(unsigned char* device) {
		SetDefaultParameter(&uvc_);
	}
};


#endif /* HELPER_H_ */
