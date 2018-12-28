/*
 *  tslib/src/ts_test.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the GPL.  Please see the file
 * COPYING for more details.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 *
 * Basic test program for touchscreen library.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <errno.h>

#include "tslib.h"
#include "fbutils.h"
#include "testutils.h"


#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<linux/fb.h>
#include<sys/ioctl.h>
#include <unistd.h>

#define RGB(r,g,b) ((b>>3)+((g>>2)<<5)+((r>>3)<<11))
#define ABS(x) x>0?x:-x

struct fb_var_screeninfo vinfo;
unsigned char * fbp = 0;
int ssize;

int display_init(){
	int fd;
	fd=open("/dev/fb0" , O_RDWR);
	ioctl(fd, FBIOGET_VSCREENINFO,&vinfo);
	ssize=vinfo.xres*vinfo.yres*vinfo.bits_per_pixel/8;
	fbp = (unsigned char *) mmap(0,ssize,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	return 0;
}

int draw_point(int x,int y,unsigned char r,unsigned char g,unsigned char b){
	int offset=(x+y*vinfo.xres)*vinfo.bits_per_pixel/8;
	int color=RGB(r,g,b);
	*(fbp+offset+0)=color & 0xFF;
	*(fbp+offset+1)=color>>8 & 0xFF;
	return 0;
}

int draw_line(int x1,int y1,int x2,int y2,unsigned char r,unsigned char g,unsigned char b){
	if(y1<y2){
		for(int i=0;i<y2-y1+1;i++)
		{
			draw_point(x1+(i*(x2-x1)/(y2-y1)),y1+i,r,g,b);
		}
	}
	else if(y1>y2)
	{
		for(int i=0;i<y1-y2+1;i++)
		{
			draw_point(x2+(i*(x1-x2)/(y1-y2)),y2+i,r,g,b);
		}
	}
	else
	{
		int min=x1<x2?x1:x2;
		for(int i=0;i<ABS(x1-x2)+1;i++)
		{
			draw_point(min+i,y1,r,g,b);
		}	
	}
	return 0;
}

int draw_circle(int x,int y,int ra,unsigned char r,unsigned char g,unsigned char b)
{
	for(int m=(y-ra)>0?(y-ra):0;m<((y+ra+1)<vinfo.yres?(y+ra+1):vinfo.yres);m++)
	{
		for(int n=(x-ra)>0?(x-ra):0;n<((x+ra+1)<vinfo.xres?(x+ra+1):vinfo.xres);n++)
		{
			if((m-y)*(m-y)+(n-x)*(n-x)<=(ra+1)*ra)
			{
				draw_point(n,m,r,g,b);
			}
		}
	}
}

int draw_rect(int x,int y,int h,int w,unsigned char r,unsigned char g,unsigned char b)
{
	for(int m=(y-h)>0?(y-h):0;m<((y+h+1)<vinfo.yres?(y+h+1):vinfo.yres);m++)
	{
		for(int n=(x-w)>0?(x-w):0;n<((x+w+1)<vinfo.xres?(x+w+1):vinfo.xres);n++)
		{
			draw_point(n,m,r,g,b);
		}
	}
}




int main(int argc, char **argv)
{
	struct tsdev *ts;
	
	ts = ts_setup(NULL, 1);
	if (!ts) {
		perror("ts_open");
		exit(1);
	}
	display_init();
	memset(fbp,0,ssize);
	int v=0;
	int i=0,j=0;
	int b_x=vinfo.xres/2;
	int b_y=vinfo.yres/2;
	struct ts_sample samp;
	samp.pressure=0;
	
	while (1) 
	{
		ts_read(ts,&samp,1);
		printf("%d\n",samp.pressure);
		if (samp.pressure > 0) 
		{
			v=-1;
			samp.pressure=0;
		}else
		{
			v+=1;
		}
		draw_circle(b_x,b_y,15,0,0,0);
		b_y=b_y+v;
		draw_circle(b_x,b_y,15,0,255,0);
		
		draw_rect(vinfo.xres-50-2*j,90,90,10,255,0,0);
		draw_rect(vinfo.xres-50-2*j,vinfo.yres-90,90,10,255,0,0);
		usleep(50000);
		draw_rect(vinfo.xres-50-2*j,90,90,10,0,0,0);
		draw_rect(vinfo.xres-50-2*j,vinfo.yres-90,90,10,0,0,0);	
		
		i++;
		if(i%300==0)
		{
			j=0;
		}
		j++;
		if(i==2147483647)break;
	}

	return 0;
}
