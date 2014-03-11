/*
 * Copyright (C) 2009 Giacomo Spigler
 * 2013 - George Jordanov - improve in performance for HSV conversion and improvements
 *
 * CopyPolicy: Released under the terms of the GNU GPL v3.0.
 */

#ifndef __LIBCAM__H__
#define __LIBCAM__H__

// FIXME: REMOVE if POSSIBLE!!!

#define USE_LOOKUP 1

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
// NOTE: PRIVATE
private:
	unsigned char *data;
	void  StartCamera(const char *name, int w, int h, int fps=30);
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
	unsigned char yv[256][256];	// RED
	unsigned char yu[256][256];	// BLUE
	int y2v[256][256];			// GREEN/2
	int y2u[256][256];			// GREEN/2
	void genYUVtoRGBLookups();

	// FOR direct conversion purposes
	unsigned char *R, *G, *B;

	void toRGB() {
		for(int x = 0; x < this->w2; x++) {
			for(int y = 0; y < this->height; y++) {
				int y0, y1, u, v;

				int i = (y*w2 + x)*4;
				y0 = data[i];
				u = data[i+1];
				y1 = data[i+2];
				v = data[i+4];
			}
		}
	  }
#endif

// NOTE: PUBLIC
public:
  const char *name;  //dev_name
  int width;
  int height;
  int fps;

  int w2;

  //unsigned char *data;

  io_method io;
  int fd;
  buffer *buffers;
  int n_buffers;

  int mb, Mb, db, mc, Mc, dc, ms, Ms, ds, mh, Mh, dh, msh, Msh, dsh;
  bool ha;

  //Camera();
  Camera(const char *name, int w, int h, int fps=30);

  ~Camera();

  unsigned char *Get();    //deprecated
  bool Update(unsigned int t=100, int timeout_ms=500); //better  (t=0.1ms, in usecs)
  bool Update(Camera *c2, unsigned int t=100, int timeout_ms=500);

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

  // data access
  unsigned char at(int x, int y) {
	  return (this->data[this->width*x + y]);
  }
  unsigned char getVal(int l) {
	  return this->data[l];
  }
  unsigned char operator[] (int x) {
	  return data[width*x];
  }
};





#endif
