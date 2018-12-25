#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<errno.h>
#include<fcntl.h>
#include<linux/fb.h>
#include<sys/ioctl.h>
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

void main()
{
	display_init();
	memset(fbp,0,ssize);
}
