#ifndef SG_FRAMEBUF_H_
#define SG_FRAMEBUF_H_

#include "typedef.h"

#define FB_WINDOWS									0x00000001		/* automatically converts rgba to bgra */

struct sg_framebuf {
	void* buf;
	dword width;
	dword height;
	dword flags;
};

#define SG_FB_INDEX(fb,x,y)							((x) % (fb)->width) + ((y) * (fb)->width)

#endif