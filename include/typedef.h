#ifndef TYPEDEF_H_
#define TYPEDEF_H_

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

typedef byte bool;

#ifndef TRUE
#define TRUE										1
#endif

#ifndef FALSE
#define FALSE										0
#endif

#ifndef NULL
#define NULL										((void*)0)
#endif

#endif