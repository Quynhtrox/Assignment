obj-m += Asm1.o
obj-m += Asm2.o

KDIR := /home/troc/workplace/buildroot/output/build/linux-6.0.9
CROSS_COMPILE := /home/troc/workplace/buildroot/output/host/bin/arm-none-linux-gnueabihf-
ARCH := arm

all:
	make -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
