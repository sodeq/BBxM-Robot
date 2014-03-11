/*
 * imaging.h
 *
 *  Created on: Jan 18, 2014
 *      Author: lynx
 */

#ifndef IMAGING_H_
#define IMAGING_H_

extern "C" {
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <linux/videodev.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
}


// global variables and functions
struct buffer{
	void* start;
	size_t length;
};
enum io_method {IO_METHOD_READ, IO_METHOD_MMAP, IO_METHOD_USERPTR};

static void errno_exit(const char* s) {
	fprintf(stderr,"%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

static int xioctl(int fd, int req, void* arg) {
	int r, itt=0;
	do{
		r = ioctl(fd, req, arg);
		itt++;
	} while( (-1 == r) && (EINTR == errno) &&(itt < 100) );
	return r;
}

typedef unsigned char uchar_;

#define CLEAR(x) memset(&(x), 0, sizeof(x))

class CAMERA{
private:
	bool initialized;
	buffer *buffers;
	const char* deviceName;
	// frame dimension
	int width, height;
	float framespeed;
	int w2;

	// frame data
	unsigned char* data;	// single channel
	unsigned char *R, *G, *B;
	int fd;	// defice file definition
	int n_buffers;
	int mb, Mb, db, mc, Mc, dc, ms, Ms,  mh, Mh, dh, msh, Msh, dsh;
	bool ha;
	io_method io;


	// Close camera
	void Close() {
		if(-1 == close(fd)) {
			printf("Camera Close!!!\n");
		}
		fd = -1;
	}

	void UnInit(){
		unsigned int i;

		switch(io) {
		case IO_METHOD_READ:
			free (buffers[0].start);
			break;

		case IO_METHOD_MMAP:
			for(i = 0; i < (unsigned int)n_buffers; ++i)
				if(-1 == munmap (buffers[i].start, buffers[i].length))
					errno_exit ("munmap");
			break;

		case IO_METHOD_USERPTR:
			for (i = 0; i < (unsigned int)n_buffers; ++i)
				free (buffers[i].start);
			break;
		}

		free (buffers);
	}


	void Start() {
		uint i;
		enum v4l2_buf_type type;

		switch (io) {
			case IO_METHOD_READ:
				break;
			case IO_METHOD_MMAP:
				for (i = 0; i < (uint)n_buffers; ++i) {
					struct v4l2_buffer buf;
					CLEAR(buf);
					buf.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
					buf.memory 	= V4L2_MEMORY_MMAP;
					buf.index	= 1;
					if(-1 == xioctl(fd, VIDIOC_QBUF, &buf)) errno_exit("VIDIOC_QBUF");
				}
				type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				if(-1==xioctl(fd, VIDIOC_STREAMON, &type)) errno_exit("VIDIOC_STREAM_ON");
				break;

			case IO_METHOD_USERPTR:
				for(i = 0; i < (unsigned int)n_buffers; ++i) {
					struct v4l2_buffer buf;

					CLEAR (buf);

					buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
					buf.memory      = V4L2_MEMORY_USERPTR;
					buf.index       = i;
					buf.m.userptr	= (unsigned long) buffers[i].start;
					buf.length      = buffers[i].length;

					if(-1 == xioctl (fd, VIDIOC_QBUF, &buf))
					  errno_exit ("VIDIOC_QBUF");
				  }

				  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

				  if(-1 == xioctl (fd, VIDIOC_STREAMON, &type))
					errno_exit ("VIDIOC_STREAMON");

				  break;
			default:
				break;
		}
	}

	void Stop() {
		enum v4l2_buf_type type;
		switch (io) {
			case IO_METHOD_READ:
				break;
			case IO_METHOD_MMAP:
			case IO_METHOD_USERPTR:
				type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				if(-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
					errno_exit("VIDIOC_STREAMOFF");
				break;
			default:
				break;
		}
	}

	void init_userp(unsigned int buffSize) {
		/*struct v4l2_requestbuffers req;
		uint page_size;
		page_size = getpagesize();
		buffSize = (buffSize+ page_size-1) & ~(page_size-1);

		CLEAR(req);

		req.count		= 4;
		req.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory		= V4L2_MEMORY_USERPTR;

		if(-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
			if(EINVAL == errno) {
				fprintf(stderr, "%s doesn't support user pointer i/o\n", deviceName);
				exit(EXIT_FAILURE);
			} else {
				errno_exit("VIDIOC_REQBUFS");
			}
		}
		buffers = calloc(4, sizeof(*buffers));

		if(!buffers) {
			fprintf(stderr, "out of memory\n");
			exit(EXIT_FAILURE);
		}

		for(n_buffers = 0; n_buffers < 4; ++n_buffers) {
			buffers[n_buffers].length = buffSize;
			buffers[n_buffers].start = memalign(page_size, buffSize);

			if(!buffers[n_buffers].start) {
				fprintf(stderr, "Out of memory\n");
				exit(EXIT_FAILURE);
			}
		}*/
	}

	void init_mmap() {
		struct v4l2_requestbuffers req;
		CLEAR(req);
		req.count		= 4;
		req.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory		= V4L2_MEMORY_MMAP;

		if(-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
			if(EINVAL == errno) {
				fprintf(stderr, "%s doesn't support memory mapping\n", deviceName);
				exit(1);
			} else {
				errno_exit("VIDIOC_REQBUFS");
			}
		}

		if(req.count < 2) {
			fprintf(stderr, "Insufficient buffer memory %s\n", deviceName);
			exit(1);
		}

		buffers = (buffer*)calloc(req.count, sizeof(*buffers));

		if(!buffers) {
			fprintf(stderr, "OUT of memory");
			exit(1);
		}

		for(n_buffers = 0; n_buffers < (int)req.count; ++n_buffers) {
			struct v4l2_buffer buf;
			CLEAR(buf);

			buf.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory		= V4L2_MEMORY_MMAP;
			buf.index		= n_buffers;

			if(-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
				errno_exit("VIDIOC_QUERYBUF");

			buffers[n_buffers].length 	= buf.length;
			buffers[n_buffers].start	= mmap(NULL,	// start anywhere
											   buf.length,
											   PROT_READ | PROT_WRITE, // required
											   MAP_SHARED,	// recommended
											   fd,
											   buf.m.offset);
			if(MAP_FAILED == buffers[n_buffers].start)
				errno_exit("mmap");
		}
	}

	void init_read(unsigned int buffSize) {
		/*buffers = calloc (1, sizeof (*buffers));

		if (!buffers) {
				fprintf (stderr, "Out of memory\n");
				exit (EXIT_FAILURE);
		}

		buffers[0].length = buffSize;
		buffers[0].start = malloc(buffSize);

		if (!buffers[0].start) {
				fprintf (stderr, "Out of memory\n");
					exit (EXIT_FAILURE);
		}*/
	}



public:
	// empty constructor
	CAMERA() {
		this->initialized = false;
	}

	CAMERA(const char* name, int w, int h, float fps=25.f){
		initialized = false;
		this->InitParam(name, w, h, fps);
	}


	/***********************************************************\
	 * 					CAMERA INITIALIZATION					*
	\***********************************************************/
	void Init(io_method iom=IO_METHOD_READ) {
		struct v4l2_capability cap;
		struct v4l2_cropcap cropcap;
		struct v4l2_crop crop;
		struct v4l2_format fmt;
		uint min;

		if(-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
			if(EINVAL == errno) {
				fprintf(stderr, "%s is no V4L2 device \n", deviceName);
				exit(1);
			} else {
				printf("VIDEO_QUERYCAP\n");
			}
		}
		if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
			fprintf(stderr, "%s is no video capture device\n", deviceName);
			exit(1);
		}

		// TODO: need to add switch conditions
		switch (iom) {
			case IO_METHOD_READ:
				if(!(cap.capabilities && V4L2_CAP_READWRITE)) {
					fprintf(stderr, "[IO_METHOD_READ]==> %s doesn't support read i/o\n", deviceName);
					exit(1);
				}
				break;
			case IO_METHOD_MMAP:
			case IO_METHOD_USERPTR:
				if(!(cap.capabilities && V4L2_CAP_STREAMING)){
					fprintf(stderr, "[IO_METHOD_USERPTR]==> %s doesn't support streaming i/o\n", deviceName);
				}
				break;
			default:
				break;
		}
		// IO_METHOD_READ
		/*if(!(cap.capabilities && V4L2_CAP_READWRITE)) {
			fprintf(stderr, "[IO_METHOD_READ]==> %s doesn't support I/O\n", deviceName);
			exit(1);
		}*/

		CLEAR(cropcap);
		cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if(0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
			crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			crop.c = cropcap.defrect;	// reset to default
			if(-1 ==xioctl(fd, VIDIOC_CROPCAP, &crop)) {
				switch(errno) {
				case EINVAL:
					break; // cropping not supported
				default:
					break;
				}
			}
		} else {
			// error ignored
		}

		CLEAR(fmt);
		fmt.type				=	V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width		= 	width;
		fmt.fmt.pix.height		=	height;
		fmt.fmt.pix.pixelformat	=	V4L2_PIX_FMT_YUYV;
		fmt.fmt.pix.field		=	V4L2_FIELD_INTERLACED;

		if(-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
			errno_exit("[VIDIO_S_FMT]");

		// streaming list
		struct v4l2_streamparm p;
		p.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		p.parm.capture.timeperframe.numerator=1;
		p.parm.capture.timeperframe.denominator=framespeed;
		p.parm.output.timeperframe.numerator=1;
		p.parm.output.timeperframe.denominator=framespeed;

		if(-1 == xioctl(fd, VIDIOC_S_PARM, &p))
			errno_exit("VIDIO_S_PARM");

		// default values (mins AND max)
		struct v4l2_queryctrl queryctl;
		memset(&queryctl, 0, sizeof(queryctl));
		queryctl.id = V4L2_CID_BRIGHTNESS;
		if(-1==xioctl(fd, VIDIOC_QUERYCTRL, &queryctl)) {
			if(errno != EINVAL) {
				printf("brightness error\n");
			} else {
				printf("brightness adjustment not supported\n");
			}
		} else if(queryctl.flags & V4L2_CTRL_FLAG_DISABLED) {
			printf("brightness adjustment is not supported\n");
		}
		mb = queryctl.minimum;
		Mb = queryctl.maximum;
		db = queryctl.default_value;

		memset(&queryctl, 0, sizeof(queryctl));
		queryctl.id = V4L2_CID_CONTRAST;
		if(-1 == xioctl(fd, VIDIOC_QUERYCTRL, &queryctl)) {
			if(errno != EINVAL) {
				printf("contrast error");
			} else {
				printf("contrast adj error\n");
			}
		} else if(queryctl.flags & V4L2_CTRL_FLAG_DISABLED) {
			printf("contrast adj error\n");
		}

		mc = queryctl.minimum;
		Mc = queryctl.maximum;
		dc = queryctl.default_value;
		memset(&queryctl, 0, sizeof(queryctl));
		queryctl.id = V4L2_CID_SATURATION;
		if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctl)) {
			if(errno != EINVAL) {
			  //perror ("VIDIOC_QUERYCTRL");
			  //exit(EXIT_FAILURE);
			  printf("saturation error\n");
			} else {
			  printf("saturation is not supported\n");
			}
		  } else if(queryctl.flags & V4L2_CTRL_FLAG_DISABLED) {
			printf ("saturation is not supported\n");
		  }
		  ms=queryctl.minimum;
		  Ms=queryctl.maximum;
		  dsh =queryctl.default_value;


		  memset(&queryctl, 0, sizeof(queryctl));
		  queryctl.id = V4L2_CID_HUE;
		  if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctl)) {
			if(errno != EINVAL) {
			  //perror ("VIDIOC_QUERYCTRL");
			  //exit(EXIT_FAILURE);
			  printf("hue error\n");
			} else {
			  printf("hue is not supported\n");
			}
		  } else if(queryctl.flags & V4L2_CTRL_FLAG_DISABLED) {
			printf ("hue is not supported\n");
		  }
		  mh=queryctl.minimum;
		  Mh=queryctl.maximum;
		  dh=queryctl.default_value;


		  memset(&queryctl, 0, sizeof(queryctl));
		  queryctl.id = V4L2_CID_HUE_AUTO;
		  if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctl)) {
			if(errno != EINVAL) {
			  //perror ("VIDIOC_QUERYCTRL");
			  //exit(EXIT_FAILURE);
			  printf("hueauto error\n");
			} else {
			  printf("hueauto is not supported\n");
			}
		  } else if(queryctl.flags & V4L2_CTRL_FLAG_DISABLED) {
			printf ("hueauto is not supported\n");
		  }
		  ha=queryctl.default_value;


		  memset(&queryctl, 0, sizeof(queryctl));
		  queryctl.id = V4L2_CID_SHARPNESS;
		  if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctl)) {
			if(errno != EINVAL) {
			  //perror ("VIDIOC_QUERYCTRL");
			  //exit(EXIT_FAILURE);
			  printf("sharpness error\n");
			} else {
			  printf("sharpness is not supported\n");
			}
		  } else if(queryctl.flags & V4L2_CTRL_FLAG_DISABLED) {
			printf ("sharpness is not supported\n");
		  }
		  msh=queryctl.minimum;
		  Msh=queryctl.maximum;
		  dsh=queryctl.default_value;

		//TODO: TO ADD SETTINGS
		//here should go custom calls to xioctl

		//END TO ADD SETTINGS

		  /* Note VIDIOC_S_FMT may change width and height. */

		  /* Buggy driver paranoia. */
		  min = fmt.fmt.pix.width * 2;
		  if(fmt.fmt.pix.bytesperline < min)
			fmt.fmt.pix.bytesperline = min;
		  min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
		  if(fmt.fmt.pix.sizeimage < min)
			fmt.fmt.pix.sizeimage = min;

		  switch(io) {
			case IO_METHOD_READ:
			  init_read(fmt.fmt.pix.sizeimage);
			  break;

			case IO_METHOD_MMAP:
			  init_mmap();
			  break;

			case IO_METHOD_USERPTR:
			  init_userp(fmt.fmt.pix.sizeimage);
			  break;
			}

		}

	void InitParam(const char* name, int w, int h, float fps=25.f){
		if(this->initialized == false) {
			deviceName = name;
			width = w;
			height = h;
			framespeed = fps;
			w2 = w/2;
			data = (uchar_*)malloc(w*h*sizeof(uchar_));
			R = (uchar_ *)malloc(w*h*sizeof(uchar_));
			G = (uchar_ *)malloc(w*h*sizeof(uchar_));
			B = (uchar_ *)malloc(w*h*sizeof(uchar_));

			/*Open();
			Init();
			Start();*/
			printf("parameter initialization succeded!\n");
		}
	}

	/**
	 * Open device
	 **/
	void Open() {
		struct stat st;
		if(-1 == stat(deviceName, &st)) {
			fprintf(stderr, "Cannot identify '%s' : '%d, %s\n", deviceName, errno, strerror(errno));
			exit(1);

			if(!S_ISCHR(st.st_mode)) {
				fprintf(stderr, "%s is no device\n", deviceName);
				exit(1);
			}

			fd = open(deviceName, O_RDWR | O_NONBLOCK,0);

			if(-1 == fd) {
				fprintf(stderr, "Cannot open '%s' : %d, %s\n", deviceName, errno, strerror(errno));
				exit(1);
			}
		}
		printf("Open() succeded!\n");
	}

	template<typename _Tp>
	_Tp operator[](int i){
		return this->data[i];
	}

	virtual ~CAMERA() {
		this->StopCam();
	}

	// Get frame!
	unsigned char* Get() {
		struct v4l2_buffer buf;
		switch (io) {
			case IO_METHOD_READ:
				break;
			case IO_METHOD_MMAP:
				CLEAR(buf);
				buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				buf.memory = V4L2_MEMORY_MMAP;

				if(-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
					switch(errno) {
					case EAGAIN:
						return 0;
					case EIO:
					default:
						return 0;	// errno_exit ("VIDIOC_DQBUF");
					}
				}
				assert(buf.index < (uint)n_buffers);
				memcpy( data,
						(uchar_*)buffers[buf.index].start,
						buffers[buf.index].length);
				if(-1 == xioctl(fd, VIDIOC_DQBUF, &buf))	return 0;

				return data;
				break;
			case IO_METHOD_USERPTR:
				break;
			default:
				break;
		}
		return 0;
	}

	bool Update(unsigned int t = 100, int timeout_ms=500) {
		bool grabbed = false;
		int grab_time_uS=0;
		while(!grabbed) {
			if((!grabbed) && (this->Get()!= 0))
				grabbed = true;

			if(!grabbed) {
				usleep(t);
				grab_time_uS += (int)t;
				if(grab_time_uS > timeout_ms*1000) {
					break;
				}
			}
		}
		return grabbed;
	}

	bool Update(CAMERA* c2, unsigned int t = 100, int timeout_ms=500) {
		/* STEREO Vision purposes */
		bool left_ 	= false;
		bool right_	= false;
		int grab_time_us = 0;
		while(!(left_ && right_)) {
			if((!left_) && (this->Get()!=0)) left_=  true;
			if((!right_) && (c2->Get() != 0))right_ = true;
			if(!(left_ && right_)) {
				usleep(t);
				grab_time_us += (int)t;
				if(grab_time_us > timeout_ms*1000)
					break;
			}
		}
		return (left_ & right_);
	}

	void StopCam() {
		Stop();
		UnInit();
		Close();
		free(data);
		free(R);
		free(G);
		free(B);
		initialized = false;
	}

	int minBrightness()const {	return mb;	}
	int maxBrightness()const { 	return Mb;	}
	int defaultBrightness() const { return db; }
	int minContrast()const { return mc; }
	int maxContrast()const { return Mc; }
	int defaultContrast()const { return dc;}

	uchar_ operator[](int idx){ return data[idx];}
};


typedef struct {
	int w, h, maxGrey;
	unsigned char* data;
	int flag;
}MyImage;

char* strrev(char* str);

void itochar(int x, char* szBuffer, int radix) {
	int i = 0, n, xx;
	n = x;
	while(n > 0) {
		xx = n%radix;
		szBuffer[i++] = '0' + xx;
	}
	szBuffer[i] = '\0';
	strrev(szBuffer);
}

char* strrev(char* str) {
	char *p1, *p2;
	if(!str || !*str) {
		return str;
	}
	for(p1=str, p2=str+strlen(str)-1; p2 > p1; ++p1, --p2) {
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}
	return str;
}

int WritePGM(char* fileName, MyImage *image) {
	char param[5];
	int i;
	const char* format = "P5";
	if(image->flag == 0) {
		return -1;
	}

	FILE *fp = fopen(fileName, "w");
	if(!fp) {
		printf("unable to create new file");
		return -2;
	}

	fputs(format, fp);
	fputc('\n', fp);
	itochar(image->w, param, 10);
	fputs(param, fp);
	param[0] = 0;
	fputc(' ', fp);
	itochar(image->w, param, 10);
	fputs(param, fp);
	param[0] = 0;
	fputc('\n', fp);
	itochar(image->maxGrey, param, 10);
	fputs(param, fp);
	fputc('\n', fp);

	for(i = 0; i < (image->w*image->h); ++i) {
		fputc(image->data[i], fp);
	}

	fclose(fp);

	return 0;
}

#endif /* IMAGING_H_ */
