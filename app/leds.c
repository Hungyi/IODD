#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "tq2440_7seg.h"
int _7SEG_put_led(int fd, unsigned which, unsigned long data);
int Delay(int ms);
int main(int argc, char **argv)
{
	int on;
	int led_no;
	int i=0, number[10]={0,1,2,3,4,5,6,7,8,9};
	int fd;
/*
	if(	argc != 3 ||
		sscanf(argv[1], "%d", &led_no)!=1 ||
		sscanf(argv[2], "%d", &on)!=1 ||
		on < 0 ||
		on > 1 ||
		led_no < 1 ||
		led_no > 4)
	{
		fprintf(stderr, "Usage: leds led_no 0|1\n");
		exit(1);
	}
*/
	fd = open("/dev/tq2440_7seg", 0);
	if(fd < 0){
		perror("open device 7seg");
		exit(1);
	}
	//ioctl(fd, on, (led_no-1));
	printf("hello hungyi\n");
/*
	while(1)
	{
		//讓7 段顯示器由右向左依序顯示0~9 數字
		_7SEG_put_led(fd, _7SEG_D5_INDEX, number[i%10]);
		_7SEG_put_led(fd, _7SEG_D6_INDEX, number[i%10]);
		_7SEG_put_led(fd, _7SEG_D7_INDEX, number[i%10]);
		_7SEG_put_led(fd, _7SEG_D8_INDEX, number[i%10]);
		i++;
		Delay(3);
	}*/
	for(;;);
	//close(fd);
	return 0;
}

int _7SEG_put_led(int fd, unsigned which, unsigned long data)
{
	_7seg_info_t	Segment;	
	int 		ret;

	Segment.Mode = _7SEG_MODE_HEX_VALUE;
	Segment.Which = which;
	Segment.Value = data;
	ret = ioctl(fd, _7SEG_IOCTL_SET, &Segment);
    	
	return ((ret >= 0) ? OK : UM);    	
}

int Delay(int ms)
{
	usleep(ms*1000);
}

