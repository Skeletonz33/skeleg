#ifndef SG_COLOR_H_
#define SG_COLOR_H_

#include "typedef.h"

struct sg_rgba {
	byte r;
	byte g;
	byte b;
	byte a;
};

struct sg_frgba {
	float r;
	float g;
	float b;
	float a;
};

inline bool sg_rgba2frgba(struct sg_frgba* dst, const struct sg_rgba* src) {
	if(dst == NULL || src == NULL) return FALSE;

	dst->r = (float)src->r / 255.0f;
	dst->g = (float)src->g / 255.0f;
	dst->b = (float)src->b / 255.0f;
	dst->a = (float)src->a / 255.0f;

	return TRUE;
}

inline bool sg_frgba2rgba(struct sg_rgba* dst, const struct sg_frgba* src) {
	if(dst == NULL || src == NULL) return FALSE;

	dst->r = (byte)((float)src->r * 255.0f + 0.5f);
	dst->g = (byte)((float)src->g * 255.0f + 0.5f);
	dst->b = (byte)((float)src->b * 255.0f + 0.5f);
	dst->a = (byte)((float)src->a * 255.0f + 0.5f);

	return TRUE;
}

inline bool sg_rgba2bgra(struct sg_rgba* c) {
	if(c == NULL) return FALSE;

	c->r ^= c->b;
	c->b ^= c->r;
	c->r ^= c->b;

	return TRUE;
}

#endif