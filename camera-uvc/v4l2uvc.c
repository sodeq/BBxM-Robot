
/*******************************************************************************
#             uvccapture: USB UVC Video Class Snapshot Software                #
#This package work with the Logitech UVC based webcams with the mjpeg feature  #
#.                                                                             #
# 	Orginally Copyright (C) 2005 2006 Laurent Pinchart &&  Michel Xhaard   #
#       Modifications Copyright (C) 2006  Gabriel A. Devenyi                   #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; either version 2 of the License, or            #
# (at your option) any later version.                                          #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "v4l2uvc.h"

//static int debug = 0;



static int init_v4l2 (struct vdIn *vd);

int close_v4l2 (struct vdIn *vd)
{
  int i;

  if (vd->isstreaming)
    video_disable (vd);

  /* If the memory maps are not released the device will remain opened even
     after a call to close(); */
  for (i = 0; i < NB_BUFFER; i++) {
    munmap (vd->mem[i], vd->buf.length);
  }

  if (vd->tmpbuffer)
    free (vd->tmpbuffer);
  vd->tmpbuffer = NULL;
  free (vd->framebuffer);
  vd->framebuffer = NULL;
  free (vd->videodevice);
  free (vd->status);
  free (vd->pictName);
  vd->videodevice = NULL;
  vd->status = NULL;
  vd->pictName = NULL;
  close (vd->fd);
  return 0;
}

int
v4l2GetControl (struct vdIn *vd, int control)
{
  struct v4l2_queryctrl queryctrl;
  struct v4l2_control control_s;
  int err;

  if (isv4l2Control (vd, control, &queryctrl) < 0)
    return -1;
  control_s.id = control;
  if ((err = ioctl (vd->fd, VIDIOC_G_CTRL, &control_s)) < 0) {
    fprintf (stderr, "ioctl get control error\n");
    return -1;
  }
  return control_s.value;
}

int
v4l2SetControl (struct vdIn *vd, int control, int value)
{
  struct v4l2_control control_s;
  struct v4l2_queryctrl queryctrl;
  int min, max, step, val_def;
  int err;

  if (isv4l2Control (vd, control, &queryctrl) < 0)
    return -1;
  min = queryctrl.minimum;
  max = queryctrl.maximum;
  step = queryctrl.step;
  val_def = queryctrl.default_value;
  if ((value >= min) && (value <= max)) {
    control_s.id = control;
    control_s.value = value;
    if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) {
      fprintf (stderr, "ioctl set control error\n");
      return -1;
    }
  }
  return 0;
}

int
v4l2UpControl (struct vdIn *vd, int control)
{
  struct v4l2_control control_s;
  struct v4l2_queryctrl queryctrl;
  int min, max, current, step, val_def;
  int err;

  if (isv4l2Control (vd, control, &queryctrl) < 0)
    return -1;
  min = queryctrl.minimum;
  max = queryctrl.maximum;
  step = queryctrl.step;
  val_def = queryctrl.default_value;
  current = v4l2GetControl (vd, control);
  current += step;
  if (current <= max) {
    control_s.id = control;
    control_s.value = current;
    if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) {
      fprintf (stderr, "ioctl set control error\n");
      return -1;
    }
  }
  return control_s.value;
}

int
v4l2DownControl (struct vdIn *vd, int control)
{
  struct v4l2_control control_s;
  struct v4l2_queryctrl queryctrl;
  int min, max, current, step, val_def;
  int err;

  if (isv4l2Control (vd, control, &queryctrl) < 0)
    return -1;
  min = queryctrl.minimum;
  max = queryctrl.maximum;
  step = queryctrl.step;
  val_def = queryctrl.default_value;
  current = v4l2GetControl (vd, control);
  current -= step;
  if (current >= min) {
    control_s.id = control;
    control_s.value = current;
    if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) {
      fprintf (stderr, "ioctl set control error\n");
      return -1;
    }
  }
  return control_s.value;
}

int
v4l2ToggleControl (struct vdIn *vd, int control)
{
  struct v4l2_control control_s;
  struct v4l2_queryctrl queryctrl;
  int current;
  int err;

  if (isv4l2Control (vd, control, &queryctrl) != 1)
    return -1;
  current = v4l2GetControl (vd, control);
  control_s.id = control;
  control_s.value = !current;
  if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) {
    fprintf (stderr, "ioctl toggle control error\n");
    return -1;
  }
  return control_s.value;
}



int
v4l2ResetPanTilt (struct vdIn *vd, int pantilt)
{
  int control = V4L2_CID_PANTILT_RESET;
  struct v4l2_control control_s;
  struct v4l2_queryctrl queryctrl;
  unsigned char val;
  int err;

  if (isv4l2Control (vd, control, &queryctrl) < 0)
    return -1;
  val = (unsigned char) pantilt;
  control_s.id = control;
  control_s.value = val;
  if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) {
    fprintf (stderr, "ioctl reset Pan control error\n");
    return -1;
  }

  return 0;
}
union pantilt {
  struct {
    short pan;
    short tilt;
  } s16;
  int value;
} pantilt;

int
v4L2UpDownPan (struct vdIn *vd, short inc)
{
  int control = V4L2_CID_PANTILT_RELATIVE;
  struct v4l2_control control_s;
  struct v4l2_queryctrl queryctrl;
  int err;

  union pantilt pan;

  control_s.id = control;
  if (isv4l2Control (vd, control, &queryctrl) < 0)
    return -1;

  pan.s16.pan = inc;
  pan.s16.tilt = 0;

  control_s.value = pan.value;
  if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) {
    fprintf (stderr, "ioctl pan updown control error\n");
    return -1;
  }
  return 0;
}

int
v4L2UpDownTilt (struct vdIn *vd, short inc)
{
  int control = V4L2_CID_PANTILT_RELATIVE;
  struct v4l2_control control_s;
  struct v4l2_queryctrl queryctrl;
  int err;
  union pantilt pan;

  control_s.id = control;
  if (isv4l2Control (vd, control, &queryctrl) < 0)
    return -1;

  pan.s16.pan = 0;
  pan.s16.tilt = inc;

  control_s.value = pan.value;
  if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) {
    fprintf (stderr, "ioctl tiltupdown control error\n");
    return -1;
  }
  return 0;
}
