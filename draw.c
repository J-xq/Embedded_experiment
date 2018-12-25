#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<linux/fb.h>
#include<sys/ioctl.h>
#include <unistd.h>
#include <string.h>

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
			if((m-y)*(m-y)+(n-x)*(n-x)<=(ra+1)*ra&&(m-y)*(m-y)+(n-x)*(n-x)>=(ra-1)*ra)
			{
				draw_point(n,m,r,g,b);
			}
		}
	}
}



void main(){
    display_init();
    while(1)
    {
	for(int i=0;i<450;i++)
	{
		draw_circle(0+i,360,80,0,255,255);
		//draw_circle(900-i,360,80,0,255,255);
		sleep(0.05);
		memset(fbp,0,ssize);		
	}
    }
}
