/*
 * libcam.h
 *
 *  Created on: Dec 24, 2013
 *      Author: lynx
 */

#ifndef LIBCAM_H_
#define LIBCAM_H_


struct buffer {
	void *start;
	size_t length;
};

typedef enum {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USEPTR
} io_methods;

class CAMERA {
private:
	void Open();
	void Close();

	void Init();
	void UnInit();

	void Start();
	void Stop();

	void InitUserp(unsigned int buffSize);
	void InitMMAP();
	void InitRead(unsigned int buffSize);

	bool initialised;

	// USE_LOOKUP
	void genYUVtoRGBLookup();
	unsigned char yv[256][256];
	unsigned char yu[256][256];
			 int y2v[256][256];
			 int y2u[256][256];

public:
	const char* name; // dev name
	int width;
	int height;
	int fps;
	int w2;

	unsigned char *data;

	io_methods io;
	int fd;
	buffer *buffers;
	int n_buffers;

	int mb, Mb, db, mc, Mc, dc, ms, Ms, ds, mh, Mh, dh, msh, Msh, dsh;
	bool ha;

	// camera
	CAMERA(const char* name, int w, int h, int fps=30);
	void StartCamera(const char* name, int w, int h, int fps = 30);
	virtual ~CAMERA();

	unsigned char *Get();
	bool Update(unsigned int t=100, int timeout_ms=500);
	bool Update(CAMERA *c2, unsigned int t= 100, int timeout_ms = 500);

	StopCam();
	int minBrightness();
	int defaultBrightness();
	int minContrast();
	int maxContrast();
	int defaultContrast();
};

#endif /* LIBCAM_H_ */











