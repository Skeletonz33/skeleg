#ifndef SG_TRIANGLE_H_
#define SG_TRIANGLE_H_

#include "typedef.h"
#include "color.h"
#include "framebuf.h"

struct sg_vert {
	float x;
	float y;
	struct sg_frgba color;
};

struct sg_triangle {
	struct sg_vert verts[3];
};

bool sg_render(const struct sg_triangle* tri, struct sg_framebuf* fb);

#endif