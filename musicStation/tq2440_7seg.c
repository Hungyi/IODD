#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>

#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <plat/regs-timer.h>
#include <plat/regs-rtc.h>
#include <asm/io.h>
#include <mach/irqs.h>
#include <linux/interrupt.h>
#include <mach/regs-irq.h>
#include <linux/clk.h>

#define SEG_MAJOR	235
#define DEVICE_NAME	"tq2440_7seg"

unsigned long gpbcon;
unsigned long gpbdat;

unsigned long tcfg0;
unsigned long tcfg1;
unsigned long tcon;
unsigned long tcntb0;
unsigned long tcntb1;
unsigned long tcmpb0;
unsigned long tcnto0;
unsigned long rtccon;
unsigned long ticnt;

unsigned long srcpnd;
unsigned long intmask;
unsigned long intpnd;
unsigned long srcpnd1;
unsigned long intmask1;
unsigned long intpnd1;
unsigned long intoffset;
unsigned long pclk;
unsigned long toggle_data;
unsigned long toggle_data1;
int led_count=0;
int timer1_count=0;
int seg_tbl_index=0;
static unsigned char seven_seg_buf[4],seven_seg_idx;

void ch7segnum(int seg_tbl_index);
static irqreturn_t timer2_interrupt(int irq, void *dev_id, struct pt_regs *regs);
static irqreturn_t timer1_interrupt(int irq, void *dev_id, struct pt_regs *regs);

static void Initial_7SEG (void)
{
        seven_seg_buf[0] = 0xc0;
        seven_seg_buf[1] = 0xc0;
        seven_seg_buf[2] = 0xc0;
        seven_seg_buf[3] = 0xc0;		
};

static unsigned long led_tbl[]=
{
	0xc0,0xf9,0xa4,0xb0,0x99,
	0x92,0x82,0xf8,0x80,0x90,
};
static unsigned long chled[]=
{
	S3C2410_GPF3,
	S3C2410_GPG0,
	S3C2410_GPG6,
	S3C2410_GPG13,	
};

static unsigned long seg_table[]=
{
	S3C2410_GPG10,  //a
	S3C2410_GPF4,	//b
	S3C2410_GPG5,	//c
	S3C2410_GPG1,	//d
	S3C2410_GPG14,	//e
	S3C2410_GPG11,	//f
	S3C2410_GPG3,	//g
	S3C2410_GPG7,	//h
	S3C2410_GPF3,	//0
	S3C2410_GPG0,	//1
	S3C2410_GPG6,	//2
	S3C2410_GPG13,	//3
	
};

static unsigned int seg_cfg_table[]=
{
	S3C2410_GPG10_OUTP,	
	S3C2410_GPF4_OUTP,	
	S3C2410_GPG5_OUTP,	
	S3C2410_GPG1_OUTP,	
	S3C2410_GPG14_OUTP,	
	S3C2410_GPG11_OUTP,	
	S3C2410_GPG3_OUTP,	
	S3C2410_GPG7_OUTP,
	S3C2410_GPF3_OUTP,
	S3C2410_GPG0_OUTP,
	S3C2410_GPG6_OUTP,
	S3C2410_GPG13_OUTP,
};

void ch7segnum(int seg_tbl_index)
{
	unsigned int temp,lsb,i;
	//printk("in ch7segnum seg_tbl_index=%d\n",seg_tbl_index);
	temp=seven_seg_buf[seg_tbl_index];
	//printk("temp=%d\n",temp);
	for(i=0;i<8;i++)
	{
		lsb = temp & 1;
		//printk("lsb=%d\n",lsb);
		s3c2410_gpio_setpin(seg_table[i], lsb);
		temp = temp >> 1;	
	}
}
static void timer_1_2_init(void)
{
	tcfg0 = __raw_readl(S3C2410_TCFG0);
	tcfg0 &= ~(S3C2410_TCFG_PRESCALER1_MASK | S3C2410_TCFG_PRESCALER0_MASK );
	tcfg0 |= ((255<<8)|(255));
	__raw_writel(tcfg0, S3C2410_TCFG0);

	tcfg1 = __raw_readl(S3C2410_TCFG1);
	tcfg1 &= ~(S3C2410_TCFG1_MUX2_MASK | S3C2410_TCFG1_MUX1_MASK);
	tcfg1 |= ((S3C2410_TCFG1_MUX2_DIV16) | (S3C2410_TCFG1_MUX1_DIV16));
	__raw_writel(tcfg1, S3C2410_TCFG1);

	/* prescaler = 255; divider = 1/16; so 50MHz/(255+1)/16 = 12.2070kHz(81.9188 us) */ 
	/* 4ms = 4000us, 4000/81.9188 = 48.82 */
	/* 49 * 81.9188 ~= 4014 */

	pclk = clk_get_rate(clk_get(NULL, "pclk"));
	tcntb0 = (pclk/(256 * 16 * 1000))*4;		//1ms*4
	__raw_writel(tcntb0, S3C2410_TCNTB(2));

	tcntb1 = (pclk/(256 * 16));		//1s
	__raw_writel(tcntb1, S3C2410_TCNTB(1));

	//tcmpb0 = 49>>1;
	//__raw_writel(tcmpb0, S3C2410_TCMPB(2));

	/* start the Timer2 */
	tcon = __raw_readl(S3C2410_TCON);
	tcon &= ~((15<<12) | (15<<8));
	tcon |= ((0xb<<12) | (0xb<<8)); 
	//S3C2410_TCON_T2RELOAD | S3C2410_TCON_T2INVERT | S3C2410_TCON_T2MANUALUPD | S3C2410_TCON_T2START;
	__raw_writel(tcon, S3C2410_TCON);	
	tcon &= ~((2<<12) | (2<<8));
	__raw_writel(tcon, S3C2410_TCON);	

	
	printk(DEVICE_NAME" timer1 and time2 initialized\n");
}


static struct cdev seg_cdev;

static int tq2440_seg_open(struct inode *inode,struct file *filp)
{
	int i, err1, err2;
	for(i=0;i<12;i++)
	{
		s3c2410_gpio_cfgpin(seg_table[i],seg_cfg_table[i]);
	}
	
	err1 = request_irq(IRQ_TIMER2, timer2_interrupt,IRQF_DISABLED, "TIMER2", NULL);
	if(err1)
	{
		disable_irq(IRQ_TIMER2);
		free_irq(IRQ_TIMER2, NULL);
		return -EBUSY;
	}
	else
		printk("drive open ok\n");
	
	err2 = request_irq(IRQ_TIMER1, timer1_interrupt,IRQF_DISABLED, "TIMER1", NULL);
	if(err2)
	{
		disable_irq(IRQ_TIMER1);
		free_irq(IRQ_TIMER1, NULL);
		return -EBUSY;
	}
	else
		printk("drive open ok\n");	

	/* enable timer interrupt*/
	intmask = __raw_readl(S3C2410_INTMSK);
	intmask &= ~((1<<12)|(1<<11));	//timer2 and timer1
	__raw_writel(intmask, S3C2410_INTMSK);


	return 0;
}

static int tq2440_seg_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static int tq2440_seg_release(struct inode *inode,struct file *filp)
{
	free_irq(IRQ_TIMER2, NULL);
	free_irq(IRQ_TIMER1, NULL);
	printk("drive close ok\n");
	return 0;
}	

static irqreturn_t timer1_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{		
	srcpnd1 = __raw_readl(S3C2410_SRCPND);
	srcpnd1 &= ~(1<<11);
	__raw_writel(srcpnd1, S3C2410_SRCPND);

	intpnd1 = __raw_readl(S3C2410_INTPND);
	intpnd1 &= ~(1<<11);
	__raw_writel(intpnd1, S3C2410_INTPND);

	intmask1 = __raw_readl(S3C2410_INTMSK);  //disable interrupt
	intmask1 |= (1<<11);
	__raw_writel(intmask1, S3C2410_INTMSK);
	
	toggle_data1 ^= 1;
	s3c2410_gpio_setpin(S3C2410_GPB6, toggle_data1); //leds

	timer1_count++;	
 	int temp,temp1;

	temp=timer1_count % 60;
	temp1=temp % 10;		//second's number 0~9	
	seven_seg_buf[0]= led_tbl[temp1];
	temp1=temp / 10;		//second's number 0~5
	seven_seg_buf[1]= led_tbl[temp1];

	temp=timer1_count / 60;
	temp1=temp % 10;			//minute's number
	seven_seg_buf[2]= led_tbl[temp1];
	temp1=temp / 10;			//minuter's number
	seven_seg_buf[3]= led_tbl[temp1];

//	ch7segnum(seg_tbl_index);	
//	seg_tbl_index++;

//	if(seg_tbl_index>3) 
//	{
//		seg_tbl_index=0;
//	}
	

	intmask1 = __raw_readl(S3C2410_INTMSK);   //enable interrupt
	intmask1 &= ~(1<<11);
	__raw_writel(intmask1, S3C2410_INTMSK);

	return IRQ_RETVAL(IRQ_HANDLED);
}
	
static irqreturn_t timer2_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	
	srcpnd = __raw_readl(S3C2410_SRCPND);
	srcpnd &= ~(1<<12);
	__raw_writel(srcpnd, S3C2410_SRCPND);

	intpnd = __raw_readl(S3C2410_INTPND);
	intpnd &= ~(1<<12);
	__raw_writel(intpnd, S3C2410_INTPND);

	intmask = __raw_readl(S3C2410_INTMSK);  //disable interrupt
	intmask |= (1<<12);
	__raw_writel(intmask, S3C2410_INTMSK);
	
	toggle_data ^= 1;
	s3c2410_gpio_setpin(S3C2410_GPB5, toggle_data); //leds

	s3c2410_gpio_setpin(chled[led_count%4], 1);	//change led
	s3c2410_gpio_setpin(chled[(led_count+1)%4], 0);
	s3c2410_gpio_setpin(chled[(led_count+2)%4], 0);
	s3c2410_gpio_setpin(chled[(led_count+3)%4], 0);
	led_count++;
	if(led_count==4)
	{
		led_count=0;
	}

	ch7segnum(seg_tbl_index);	
	seg_tbl_index++;
	if(seg_tbl_index>3) 
	{
		seg_tbl_index=0;
	}
	
	intmask = __raw_readl(S3C2410_INTMSK);   //enable interrupt
	intmask &= ~(1<<12);
	__raw_writel(intmask, S3C2410_INTMSK);

	return IRQ_RETVAL(IRQ_HANDLED);
}

static struct file_operations tq2440_seg_fops =
{
	owner:		THIS_MODULE,
	open:		tq2440_seg_open,
	ioctl:		tq2440_seg_ioctl,
	release:	tq2440_seg_release,
};

static int __init seg_init (void)
{

	int ret;
	//printk("tq2440-seg test\n");
	ret=register_chrdev_region(MKDEV(SEG_MAJOR,0),1,DEVICE_NAME);
     	if(ret < 0){
		printk(DEVICE_NAME" can't register major number\n");
		return ret;
	}
	cdev_init(&seg_cdev,&tq2440_seg_fops);
	seg_cdev.owner = THIS_MODULE;
	cdev_add(&seg_cdev,MKDEV(SEG_MAJOR,0),1);
	printk(DEVICE_NAME" initialized\n");

	Initial_7SEG();
	timer_1_2_init();	
	
	return 0;
}

static void __exit seg_exit(void)
{
	cdev_del(&seg_cdev);
	unregister_chrdev_region(MKDEV(SEG_MAJOR, 0), 1);
	printk("exit goodbyte\n");
}

module_init(seg_init);
module_exit(seg_exit);
