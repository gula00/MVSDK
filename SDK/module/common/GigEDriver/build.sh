#!/bin/bash

KERNEL_RELEASE=
LINUX_VERSION_CODE=

#the version code of kernel 4.20.0 ((4 << 16) + (20 << 8) + 0)
KERNEL_4_20_0_VERSION_CODE=267264
KERNEL_5_15_0_VERSION_CODE=331520

function GetKernelCode(){
	#例:4.18.0-10-generic
	KERNEL_RELEASE=`uname -r`
	KERNEL_VERSION=$KERNEL_RELEASE
	#例:VERSION(4)
	VERSION=${KERNEL_VERSION%%.*}
	KERNEL_VERSION=${KERNEL_VERSION#*.}
	#例:PATCHLEVEL(18)
	PATCHLEVEL=${KERNEL_VERSION%%.*}
	
	#(VERSION << 16) + (PATCHLEVEL << 8) + SUBLEVEL
	LINUX_VERSION_CODE=`expr $VERSION \* 65536 + 0$PATCHLEVEL \* 256`
}

HOST_ARCH=`uname -m | sed -e 's/i.86/i686/' -e 's/^armv.*/arm/'`
if [ $HOST_ARCH = "x86_64" ]; then
	driverName=m64x86
elif [ $HOST_ARCH = "i686" ]; then
	driverName=m32x86
else
	driverName=arm
fi

cp -rf ./libModule/*.a ./
make platform=$driverName clean

#获取KernelCode
GetKernelCode

if [ $? == 0 ]; then
	echo kernel release:$KERNEL_5_15_0_VERSION_CODE    linux version code:$LINUX_VERSION_CODE
	# 为stdarg.h文件创建软连接,判断版本下是否有stdarg.h文件
	if [ -f "/lib/modules/$KERNEL_RELEASE/build/include/stdarg.h" ]; then
		echo "stdarg.h file exit in /lib/modules/$KERNEL_RELEASE/build/include."
	else
		# 检查文件是否存在
		if [ -f "/lib/modules/$KERNEL_RELEASE/build/include/linux/stdarg.h" ]; then

			# 建立软链接到此文件位置
			ln -s "/lib/modules/$KERNEL_RELEASE/build/include/linux/stdarg.h" "/lib/modules/$KERNEL_RELEASE/build/include/stdarg.h"

			echo "Symbolic link created successfully."

		else
			echo "/lib/modules/$KERNEL_RELEASE/build/include/linux/stdarg.h is not exit"
		fi
	fi
fi

if [ $? == 0 ]; then
	echo kernel release:$KERNEL_RELEASE    linux version code:$LINUX_VERSION_CODE
	if [ $LINUX_VERSION_CODE -ge $KERNEL_4_20_0_VERSION_CODE ]; then
		#kernel 4.20.0及以上的内核版本编译使用.o文件，否则用静态库
		cp -rf libModule/libuniversalEthernet-core.a libModule/libuniversalEthernet-core.o
		touch libModule/.libuniversalEthernet-core.o.cmd
		makeFile=./Makefile
		if [ -a $makeFile ]; then
			sed -i 's/libuniversalEthernet-core.a/libuniversalEthernet-core.o/g' $makeFile
		fi
	fi
fi

make platform=$driverName
chmod 777 *.ko
