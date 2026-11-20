//#define __DEBUG
#include "debug.h"
#include "triangle.h"

static inline float dotf(float x0, float y0, float x1, float y1) {
	return x0 * x1 + y0 * y1;
}

static inline float minf(float a, float b) {
	return a > b ? b : a;
}

static inline float minf3(float a, float b, float c) {
	return minf(a, minf(b, c));
}

static inline float maxf(float a, float b) {
	return a < b ? b : a;
}

static inline float maxf3(float a, float b, float c) {
	return maxf(a, maxf(b, c));
}

static inline void get_minmaxf(const struct sg_triangle* tri, float* fxmin, float* fymin, float* fxmax, float* fymax) {
	const struct sg_vert* verta = tri->verts;
	const struct sg_vert* vertb = tri->verts + 1;
	const struct sg_vert* vertc = tri->verts + 2;

	*fxmin = minf3(verta->x, vertb->x, vertc->x);
	*fymin = minf3(verta->y, vertb->y, vertc->y);
	*fxmax = maxf3(verta->x, vertb->x, vertc->x);
	*fymax = maxf3(verta->y, vertb->y, vertc->y);
}

static inline void to_realcoord(const struct sg_framebuf* fb, float fx, float fy, dword* x, dword* y) {
	float fw = (float)fb->width;
	float fh = (float)fb->height;

	fx = maxf(0.0, minf(fx, 1.0f));
	fy = maxf(0.0, minf(fy, 1.0f));
	
	*x = (dword)(fx * fw);
	*y = (dword)(fy * fh);
}

static inline void get_step(const struct sg_framebuf* fb, float* fdx, float* fdy) {
	float fw = (float)fb->width;
	float fh = (float)fb->height;

	*fdx = 1.0f / fw;
	*fdy = 1.0f / fh;
}

static inline float signf(float x0, float y0, float x1, float y1, float x2, float y2) {
	return (x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2);
}

static inline bool in_tri(const struct sg_triangle* tri, float fx, float fy) {
	const struct sg_vert* verta = tri->verts;
	const struct sg_vert* vertb = tri->verts + 1;
	const struct sg_vert* vertc = tri->verts + 2;

	float ax = verta->x;
	float ay = verta->y;
	float bx = vertb->x;
	float by = vertb->y;
	float cx = vertc->x;
	float cy = vertc->y;

	logf("skeleg: triangle.c: (static) in_tri: ax=%0.5f, ay=%0.5f, bx=%0.5f, by=%0.5f, cx=%0.5f, cy=%0.5f\n", ax, ay, bx, by, cx, cy);

	float sa = signf(fx, fy, ax, ay, bx, by);
	float sb = signf(fx, fy, bx, by, cx, cy);
	float sc = signf(fx, fy, cx, cy, ax, ay);

	bool neg = (sa < 0.0f) || (sb < 0.0f) || (sc < 0.0f);
	bool pos = (sa > 0.0f) || (sb > 0.0f) || (sc > 0.0f);

	return !(neg && pos);
}

static inline float absf(float x) {
	return x < 0 ? -x : x;
}

static inline float tri_area(float ax, float ay, float bx, float by, float cx, float cy) {
	return absf(ax * (by - cy) + bx * (cy - ay) + cx * (ay - by)) / 2.0f;
}

static inline float tri_tarea(const struct sg_triangle* tri) {
	const struct sg_vert* verta = tri->verts;
	const struct sg_vert* vertb = tri->verts + 1;
	const struct sg_vert* vertc = tri->verts + 2;

	float ax = verta->x;
	float ay = verta->y;
	float bx = vertb->x;
	float by = vertb->y;
	float cx = vertc->x;
	float cy = vertc->y;

	return tri_area(ax, ay, bx, by, cx, cy);
}

static inline void interp(struct sg_frgba* dst, const struct sg_triangle* tri, float area, float fx, float fy) {
	const struct sg_vert* verta = tri->verts;
	const struct sg_vert* vertb = tri->verts + 1;
	const struct sg_vert* vertc = tri->verts + 2;

	float ax = verta->x;
	float ay = verta->y;
	float bx = vertb->x;
	float by = vertb->y;
	float cx = vertc->x;
	float cy = vertc->y;

	float bcp = tri_area(bx, by, cx, cy, fx, fy);
	float acp = tri_area(ax, ay, cx, cy, fx, fy);
	float abp = tri_area(ax, ay, bx, by, fx, fy);

	float wa = bcp / area;
	float wb = acp / area;
	float wc = abp / area;

	struct sg_frgba ca = verta->color;
	struct sg_frgba cb = vertb->color;
	struct sg_frgba cc = vertc->color;

	dst->r = ca.r * wa + cb.r * wb + cc.r * wc;
	dst->g = ca.g * wa + cb.g * wb + cc.g * wc;
	dst->b = ca.b * wa + cb.b * wb + cc.b * wc;
	dst->a = 1.0f;
}

bool sg_render(const struct sg_triangle* tri, struct sg_framebuf* fb) {
	if(tri == NULL || fb == NULL) return FALSE;

	float fxmin;
	float fymin;
	float fxmax;
	float fymax;
	get_minmaxf(tri, &fxmin, &fymin, &fxmax, &fymax);

	dword xmin;
	dword ymin;
	dword xmax;
	dword ymax;
	to_realcoord(fb, fxmin, fymin, &xmin, &ymin);
	to_realcoord(fb, fxmax, fymax, &xmax, &ymax);

	float fj = fymin;
	float fi = fxmin;

	float fdx;
	float fdy;
	get_step(fb, &fdx, &fdy);

	float area = tri_tarea(tri);
	for(dword j = ymin;j < ymax;j++) {
		for(dword i = xmin;i < xmax;i++) {
			if(!in_tri(tri, fi, fj)) {
				goto CONTINUE;
			}

			struct sg_frgba color;
			interp(&color, tri, area, fi, fj);

			struct sg_rgba rgba;
			sg_frgba2rgba(&rgba, &color);

			if(fb->flags & FB_WINDOWS) {
				sg_rgba2bgra(&rgba);
			}

			dword idx = SG_FB_INDEX(fb, i, j);
			logf("skeleg: triangle.c: sg_render: idx=%u\n", idx);

			((struct sg_rgba*)fb->buf)[idx] = rgba;

			CONTINUE:
				fi += fdx;
		}

		fj += fdy;
		fi = fxmin;
	}

	return TRUE;
}