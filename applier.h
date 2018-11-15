/*
 * Some kind of reverse image joiner ?
 *
 *
 * Image applier header file
 *
 *
 *
 * Copyright Tiago Teixeira, 2018
*/
#ifndef _TRANSPARENCY_APPLIER_H_
#define _TRANSPARENCY_APPLIER_H_ 1

#include <stdio.h>

/*
	The main worker function

	Pass to it the file pointers (r) of the two bitmap files
	starting from the dimensions,
	the transparency color array
	and the output file as write (binary) mode

	And shall return 0 (ok)
	or some error
*/
int apply_transparency(
	FILE*color_image,	/* the color image */
	FILE*transparency_image, /* the odd gray-scaled image to join/apply */
	FILE*output	/* the output file, reopened as "r" from the start */
);

/*
	Errors definitions
*/
#define MALLOC_ERROR	-1
#define READ_ERROR	-2
#define WRITE_ERROR	-3
#define SIZE_ERROR	-4	/* the images dimensions don't match */
#define SEEK_ERROR	-5	/* fseek()==-1 */
#define PARAM_ERROR	-6	/* invalid parameters passed */

#endif
