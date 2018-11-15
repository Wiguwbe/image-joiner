/*
 * Some kind of reverse image joiner ?
 *
 *
 * Main source file
 *
 *
 *
 * Copyright Tiago Teixeira, 2018
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "applier.h"

int main(int argc,char**argv)
{
	// return code
	int ret = 0;

	if(argc!=4)
	{
		printf("usage: %s <color_image> <map_image> <out_image>\n",*argv);
		printf("\tcurrently no convertion is done, so all images will use P3/P6 bitmap format\n");
		printf("\tand 'out_image' will use P6\n");
		exit(1);
	}

	// color image, map image, out img
	FILE *ci,*mi,*out;

	// open files
	ci = fopen(argv[1],"rb");
	if(!ci)
	{
		printf("failed to open '%s'\n",argv[1]);
		ret = 2;
		goto end_z;
	}
	mi = fopen(argv[2],"rb");
	if(!mi)
	{
		printf("failed to open '%s'\n",argv[2]);
		ret = 3;
		goto end_x;
	}
	out = fopen(argv[3],"wb");
	if(!out)
	{
		printf("failed to open/create '%s'\n",argv[3]);
		ret = 4;
		goto end_y;
	}

	// verify file magics
	char buf[2];
	if(fread(buf,1,2,ci)!=2)
	{
		printf("failed to read from color_image\n");
		ret = 6;
		goto end_w;
	}
	if(strncmp(buf,"P6",2)&&strncmp(buf,"P3",2))
	{
		printf("color_image is not a valid bitmap image\n");
		ret  = 7;
		goto end_w;
	}
	fseek(ci,0,SEEK_SET);
	if(fread(buf,1,2,mi)!=2)
	{
		printf("failed to read from map_image\n");
		ret = 8;
		goto end_w;
	}
	if(strncmp(buf,"P6",2)&&strncmp(buf,"P3",2))
	{
		printf("map_image is not a valid bitmap image\n");
		ret = 9;
		goto end_w;
	}
	fseek(mi,0,SEEK_SET);

	// apply map
	ret = apply_transparency(ci,mi,out);

	switch(ret)
	{
		case MALLOC_ERROR:
			printf("failed to allocate memory\n");
			ret = 10;
			break;
		case READ_ERROR:
			printf("failed to read from file\n");
			ret = 11;
			break;
		case WRITE_ERROR:
			printf("failed to write to file\n");
			ret = 12;
			break;
		case SIZE_ERROR:
			printf("files not the same size\n");
			ret = 13;
			break;
		case SEEK_ERROR:
			printf("failed to seek file\n");
			ret = 14;
			break;
		case PARAM_ERROR:
			printf("parameters error\n");
			ret = 15;
			break;
		case 0:
			// all okay
			printf("map applied\n");
			break;
		default:
			// unknown error
			printf("unknown error '%d'\n",ret);
	}

// end sequence
// code "optimization"
end_w:
	fclose(out);
end_x:
	fclose(mi);
end_y:
	fclose(ci);
end_z:
	return ret;
}
