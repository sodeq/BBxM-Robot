/*
 * Copyright (C) 2009 Giacomo Spigler
 * 2013 - George Jordanov - improve in performance for HSV conversion and improvements
 *
 * CopyPolicy: Released under the terms of the GNU GPL v3.0.
 */

#ifndef __LIBCAM__H__
#define __LIBCAM__H__

// FIXME: REMOVE if POSSIBLE!!!
/*
#define USE_OPENCV 1
#define USE_LOOKUP 1

#ifdef USE_OPENCV
#include <cv.h>
#include "opencv2/core/core.hpp"
#endif
*/



struct buffer {
        void *	start;
        size_t  length;
};

typedef enum {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR
} io_method;

class Camera {
private:
  void Open();
  void Close();

  void Init();
//  void Init(int iomethod) ;
  void UnInit();

  void Start();
  void Stop();

  void init_userp(unsigned int buffer_size);
  void init_mmap();
  void init_read(unsigned int buffer_size);

  bool initialised;
#ifdef USE_LOOKUP
    void genYUVtoRGBLookups();
    unsigned char yv[256][256];
    unsigned char yu[256][256];
             int y2v[256][256];
             int y2u[256][256];
#endif

public:
  const char *name;  //dev_name
  int width;
  int height;
  int fps;

  int w2;

  unsigned char *data;

  io_method io;
  int fd;
  buffer *buffers;
  int n_buffers;

  int mb, Mb, db, mc, Mc, dc, ms, Ms, ds, mh, Mh, dh, msh, Msh, dsh;
  bool ha;

  //Camera();
  Camera(const char *name, int w, int h, int fps=30);
  void  StartCamera(const char *name, int w, int h, int fps=30);
  ~Camera();

  unsigned char *Get();    //deprecated
  bool Update(unsigned int t=100, int timeout_ms=500); //better  (t=0.1ms, in usecs)
  bool Update(Camera *c2, unsigned int t=100, int timeout_ms=500);

// FIXME: REMOVE if POSSIBLE
/*
#ifdef USE_OPENCV
  void toIplImage(IplImage *im);
  void toGrayScaleIplImage(IplImage *im);
  void toGrayScaleMat(cv::Mat& im);
  void toMat (cv::Mat& im);
#endif
*/


  void StopCam();

  int minBrightness();
  int maxBrightness();
  int defaultBrightness();
  int minContrast();
  int maxContrast();
  int defaultContrast();
  int minSaturation();
  int maxSaturation();
  int defaultSaturation();
  int minHue();
  int maxHue();
  int defaultHue();
  bool isHueAuto();
  int minSharpness();
  int maxSharpness();
  int defaultSharpness();

  int setBrightness(int v);
  int setContrast(int v);
  int setSaturation(int v);
  int setHue(int v);
  int setHueAuto(bool v);
  int setSharpness(int v);
};





#endif
