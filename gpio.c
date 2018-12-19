#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define GPIO1_BASE 0x4804C000
#define GPIO_OE (GPIO1_BASE +0x134)
#define GPIO_IN (GPIO1_BASE +0x138)
#define GPIO_OUT (GPIO1_BASE +0x13C)

#define OUT1(x) *pDataOut |= (1<<x)
#define OUT0(x) *pDataOut &= ~(1<<x)

#define SETOUT(x) *pConf &= ~(1<<x);


volatile int * pConf,* pDataIn,* pDataOut;
int gpio_open(struct inode* inode,struct file* filp);
int gpio_close(struct inode *node,struct file *fd);

ssize_t gpio_write(struct file* filp,const char * buf,size_t count,loff_t * f_pos);

long gpio_ioctl(struct file *fp,unsigned int cmd, unsigned long arg);

struct file_operations fops=
{
	
	write:gpio_write,
	unlocked_ioctl:gpio_ioctl,
	open:gpio_open,
	release:gpio_close, 
};
int dev=0;
int init_module(void)
{
	register_chrdev(123, "gpio", &fops);
	printk("<1>Module installed.\n");
	return 0;
}
int gpio_open(struct inode* inode,struct file* filp)
{
	pConf=ioremap(GPIO_OE,4);
	pDataIn=ioremap(GPIO_IN,4);
	pDataOut=ioremap(GPIO_OUT,4);
	printk("<1>gpio opened!.\n");
	return 0;
}
ssize_t gpio_write(struct file* filp,const char * buf,size_t count,loff_t * f_pos)
{
	char localbuf=0;
	__copy_from_user(&localbuf,buf,count);
	if(localbuf==1)
		OUT1(24);
	else
		OUT0(24);
	*f_pos+=count;
	return count;
	printk("<1>Write completed!\n");
}
long gpio_ioctl(struct file *fp,unsigned int cmd, unsigned long arg)
{
	if(cmd==1){
		SETOUT(24);
		printk("ioctl success.\n");
		}	
	return 0;
}
int gpio_close(struct inode *node,struct file *fd)
{
	iounmap(pConf);
	iounmap(pDataOut);
	iounmap(pDataIn);
	printk("<1>gpio closed!.\n");
	return 0;
}

void cleanup_module(void)
{
	unregister_chrdev(123,"gpio");
	printk("<1>Module removed.\n");
}


