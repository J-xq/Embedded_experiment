#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc,char **argv){
	int fd=open("/dev/gpio",O_RDWR);
	ioctl(fd,1);
	char buf=1;
	
	write(fd,&buf,1);
	close(fd);
	return 0;
}
