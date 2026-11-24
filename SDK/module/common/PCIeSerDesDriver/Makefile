PWD = $(shell pwd)

#(1) 交叉编译工具
CROSS_COMPILE =
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld

#(2) 操作指令
RM = rm
CHMOD = chmod
CP = cp
MV = mv

#(3) 依赖头文件和lib路径 
KERNEL_SRC := /lib/modules/$(shell uname -r)/build 

KERNEL_CORE := pcieSerDesDriver.o RayDriverApi/RayApi.o RayDriverApi/RayPCIe.o

KERNEL_LIB := libModule/libpcieSerDesDriver-core.a

EXTRA_CFLAGS += -D_UNIX_ -D_PT_KERNEL_

#(4) 清除编译过程中生成的文件
KERNEL_DEL := *.mod.c *.order *.symvers *~ *.unsigned .*.cmd .tmp* *.o

KERNEL_DELS := libModule/*.mod.c libModule/*.order libModule/*.symvers libModule/*~ libModule/*.unsigned libModule/.*.cmd libModule/.tmp* libModule/*.o RayDriverApi/*.o RayDriverApi/.*.cmd

#(5) 目标文件名 
#HW_TYPE:=$(shell uname -m | sed -e 's/i.86/i686/' -e 's/^armv.*/arm/')
ifeq ($(platform),m32x86)
	driverName=m32x86
else ifeq ($(platform),m64x86)
	driverName=m64x86
else ifeq ($(platform),loongarch64)
	driverName=loongarch64
else
	driverName=arm
endif

obj-m += imvSerDesFrameGrabber.o

MODULE_NAME := imvSerDesFrameGrabber

$(MODULE_NAME)-objs := $(KERNEL_CORE) $(KERNEL_LIB)
			
#(6) 编译 
all:
	$(MAKE) -C $(KERNEL_SRC) M=$(shell pwd) modules
	$(CHMOD) 755 *.ko 
	$(RM) -rf $(KERNEL_DEL) $(KERNEL_DELS)
ifeq ($(platform),ArbitraryArm)
	$(CP) -rf *.ko $(DST_PATH)
endif

#(7) 清除
clean:
	$(RM) -rf $(KERNEL_DEL) $(KERNEL_DELS) *.ko 
