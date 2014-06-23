S3C2440X_KERNEL_DIR=/opt/EmbedSky/linux-2.6.30.4

ifneq ($(KERNELRELEASE),)
	obj-m := tq2440_7seg.o
else
	KERNELDIR ?= $(S3C2440X_KERNEL_DIR)
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -f *.o *.mod.* *.ko *.o.*
endif
