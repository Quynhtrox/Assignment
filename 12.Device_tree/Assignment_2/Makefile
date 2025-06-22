obj-m := Asm2.o

KDIR := /home/troc/workplace/Buildroot/buildroot/output/build/linux-6.0.9
CROSS_COMPILE := /home/troc/workplace/Buildroot/buildroot/output/host/bin/arm-none-linux-gnueabihf-
ARCH := arm

all:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	make -C $(KDIR) M=$(PWD) clean