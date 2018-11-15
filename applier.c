/*
 * Some kind of reverse image joiner ?
 *
 *
 * Image applier source file
 *
 *
 *
 * Copyright Tiago Teixeira, 2018
*/
#include "applier.h"

// stdio is included in applier.h
#include <stdlib.h>
#include <string.h>

struct pixel
{
	unsigned char r,g,b;
};

/*
	Functions to read from a stream, either binary or ascii

	stream shall be the input stream/file
	shift shall be the left bit shift count, for when 'max' is not 255
	amount shall be in number of pixels
	buffer shall be to where the bytes will go

	return shall be the number of pixels read

	These functions are not thread safe!
*/
static int read_binary(FILE*stream, int shift, int amount, struct pixel*buffer)
{
	if(amount<0)
		return PARAM_ERROR;
	// ez
	for(int i=0;i<amount;i++)
	{
		if(fread(buffer+i,sizeof(struct pixel),1,stream)!=1)
		{
			if(feof(stream))
				return i;
			return READ_ERROR;
		}
		if(shift)
		{
			(buffer+i)->r<<=shift;
			(buffer+i)->g<<=shift;
			(buffer+i)->b<<=shift;
		}
	}
	return amount;
}
static int read_ascii(FILE*stream, int shift, int amount, struct pixel*buffer)
{
	// not so ez
	int i=0;
	unsigned char b[1];	// don't like the &b
	for(i=0;i<amount;i++)
	{
		int j=0;
		unsigned char a[3] = {0,0,0};
		while(j<3)
		{
			if(fread(b,1,1,stream)!=1)
			{
				if(feof(stream))
				{
					if(j==2)
						goto fini;
					// else
					return i;
				}
				// else
				return READ_ERROR;
			}
			if(*b<0x30)
			{
				j++;
				continue;
			}
			a[j] = (a[j]<<3)+(a[j]<<1);	// times 10
			a[j] += (*b)-0x30;
		}
fini:
		(buffer+i)->r=a[0]<<shift;
		(buffer+i)->g=a[1]<<shift;
		(buffer+i)->b=a[2]<<shift;
		if(feof(stream))
		{
			i++;
			break;
		}
	}
	return i;
}

/*
	helper functions
*/
static int fm(void*a,void*b,int ret)
{
	/* frees buffers and returns ret */
	if(a)
		free(a);
	if(b)
		free(b);
	return ret;
}
// a simple define to it
#define RET(e) return fm(buffer_0,buffer_1,(e))

/*
	Main (/exposed) function
*/
int apply_transparency(FILE*ci,FILE*tp,FILE*out)
{
	unsigned char flags = 0, gen_buffer[32];
	int (*functions[2])(FILE*,int,int,struct pixel*);
	int i=0,j=0;
	int shifts[2];

	functions[0] = read_binary;
	functions[1] = read_ascii;

	// buffers
	struct pixel *buffer_0 = (struct pixel*)malloc(32*sizeof(struct pixel));
	if(!buffer_0)
		return MALLOC_ERROR;
	struct pixel *buffer_1 = (struct pixel*)malloc(32*sizeof(struct pixel));
	if(!buffer_1)
	{
		free(buffer_0);
		return MALLOC_ERROR;
	}

	// file header
	// always ascii
	int a[3] = {0,0,0},b[3] = {0,0,0};
	if(fread(gen_buffer,1,3,ci)!=3)
		RET(READ_ERROR);
	flags |= gen_buffer[1]=='3'?1:0;
	if(fread(gen_buffer,1,3,tp)!=3)
		RET(READ_ERROR);
	flags |= gen_buffer[1]=='3'?2:0;
	// color image
	while(j<3)
	{
		if(fread(gen_buffer,1,1,ci)!=1)
			RET(READ_ERROR);
		if(*gen_buffer<0x30)
		{
			j++;
			continue;
		}
		a[j]=(a[j]<<3)+(a[j]<<1);
		a[j]+=(*gen_buffer)-0x30;
	}
	// map image
	j=0;
	while(j<3)
	{
		if(fread(gen_buffer,1,1,tp)!=1)
			RET(READ_ERROR);
		if(*gen_buffer<0x30)
		{
			j++;
			continue;
		}
		b[j]=(b[j]<<3)+(b[j]<<1);	// times 10
		b[j]+=(*gen_buffer)-0x30;
	}
	// these should leave us ready to read the DATA

	if(*((unsigned short*)a)!=*((unsigned short*)b))
		RET(SEEK_ERROR);

	// shifts count
	// GCC ONLY!
	unsigned int _t=0;
	_t = a[2];
	shifts[0] = __builtin_clz((unsigned int)_t)-24;
	_t = b[2];
	shifts[1] = __builtin_clz((unsigned int)_t)-24;

	// write file header
	unsigned char header[128];
	sprintf(header,"P6\n%d %d\n255\n",a[0],a[1]);
	int len = strlen(header);
	if(fwrite(header,1,len,out)!=len)
		RET(WRITE_ERROR);

	// now do the actual work
	do
	{
		i = functions[flags&1](ci,shifts[0],32,buffer_0);
		j = functions[(flags&2)>>1](tp,shifts[1],32,buffer_1);
		int ret = i<0?i:j;
		if(i<0||j<0)
			RET(ret);
		if(i!=j)
			RET(SIZE_ERROR);

		for(int l=0;l<i;l++)
		{
			buffer_0[l].r |= ~buffer_1[l].r;
			buffer_0[l].g |= ~buffer_1[l].g;
			buffer_0[l].b |= ~buffer_1[l].b;
		}
		// write pixel to file
		if(fwrite(buffer_0,sizeof(struct pixel),j,out)!=j)
			RET(WRITE_ERROR);
		fflush(out);
	}
	while(i==32&&j==32);

	return 0;
}
