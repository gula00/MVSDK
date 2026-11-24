#!/bin/bash
# ***************************************************************************************
#     Copyright (c) 2015-2019
# ***************************************************************************************

# Default variables
HOST_ARCH=`uname -m | sed -e 's/i.86/i686/' -e 's/^armv.*/arm/'`
INSTALL_DIR_OVERWRITE=yes
LOG_PATH=/var/log/MVSDK

START_DIR=`dirname $0`
START_DIR=`cd $START_DIR; pwd`
USER_ID=`id -u`
CP_BIN=cp

CURRENT_KERNEL_VERSION=`uname -r`

#CentOS6.5 native kernel
LOWEST_KERNEL_MAJOR_VERSION=2
LOWEST_KERNEL_MINOR_VERSION=6
LOWEST_KERNEL_REVISION_VERSION=32

#Ubuntu18.10 native kernel
HIGHEST_KERNEL_MAJOR_VERSION=5
HIGHEST_KERNEL_MINOR_VERSION=0
HIGHEST_KERNEL_REVISION_VERSION=0

# ***************************************************************************************
#  Noto Sans SC font (used by the Qt UI)
#  Use the font already installed on the system, otherwise install it from this package.
# ***************************************************************************************
FONT_FAMILY="Noto Sans SC"
FONT_FILE_NAME="NotoSansSC-Regular.ttf"
FONT_SRC_PATH="$START_DIR/font/$FONT_FILE_NAME"
FONT_INSTALL_DIR=/usr/share/fonts/noto-sans-sc

font_is_installed()
{
	if [ -f "$FONT_INSTALL_DIR/$FONT_FILE_NAME" ]; then
		return 0
	fi

	if command -v fc-list >/dev/null 2>&1; then
		if fc-list : family 2>/dev/null | tr ',' '\n' | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' | grep -qix "$FONT_FAMILY"; then
			return 0
		fi
	fi

	return 1
}

install_noto_sans_sc()
{
	echo ""
	if font_is_installed; then
		echo "Check font ($FONT_FAMILY)..........OK (already installed)"
		return 0
	fi

	if [ ! -f "$FONT_SRC_PATH" ]; then
		echo "Check font ($FONT_FAMILY)..........SKIP (font file is not included in this package)"
		return 0
	fi

	echo -n "Installing font ($FONT_FAMILY)"
	mkdir -p "$FONT_INSTALL_DIR"
	cp -f "$FONT_SRC_PATH" "$FONT_INSTALL_DIR/$FONT_FILE_NAME"
	chmod 755 "$FONT_INSTALL_DIR"
	chmod 644 "$FONT_INSTALL_DIR/$FONT_FILE_NAME"

	if command -v fc-cache >/dev/null 2>&1; then
		fc-cache -f "$FONT_INSTALL_DIR" >/dev/null 2>&1
	fi

	if font_is_installed; then
		echo "..........OK"
	else
		echo "..........WARN (not detected by fontconfig, please run 'fc-cache -f' manually)"
	fi

	return 0
}

# Check required priviledge
if [ "$USER_ID" != "0" ]; then
	echo ""
	echo "Check execution permissions..........NG"
	echo "ERROR: You need to run the installer as superuser (root account)."
	echo ""	
	exit 1
else
	echo "Check execution permissions..........OK"
fi

#check arguments(*.run)
if [ "$4" = "" ]; then
	install=normal
elif [ $4 = "force" ]; then
	install=$4
else
	echo ""
	echo "ERROR: Arguments is invalid."
	echo ""
	exit 1
fi

if [ $install = "normal" ]; then
	#check platform 
	if [ $HOST_ARCH != "x86_64" ] && [ $HOST_ARCH != "i686" ]; then
		echo ""
		echo "Check installation platform..........NG"
		echo "ERROR: The current installation package is only for the x86 architecture."
		echo "ERROR: The current architecture is $HOST_ARCH, please replace the installation for $HOST_ARCH."
		echo ""	
		exit 1
	else
		echo "Check installation platform..........OK"
	fi
fi

#check rc.local
RC_LOCAL=rc.local
RC_LOCAL_LOCATION=/etc/$RC_LOCAL
RC_D_LOCAL_LOCATION=/etc/rc.d/$RC_LOCAL

#CentOS6.5 and Ubuntu use 'lsb_release' command to get $LINUX_OS and $LINUX_OS_VERSION
if [ ! -f "/etc/os-release" ]; then
	LINUX_OS=`lsb_release -d | awk '{print $2}'`
	if [ $LINUX_OS = "CentOS" ]; then
		SYSTEM=CentOS
		LINUX_OS_VERSION=`lsb_release -d | awk '{print $4}'`
		if [ ! -f $RC_D_LOCAL_LOCATION ]; then
			echo ""	
			echo "Check driver loading service file..........NG"
			echo "WARN: $RC_D_LOCAL_LOCATION does not exit."
			echo ""	
		else
			echo "Check driver loading service file..........OK"
		fi	
	elif [ $LINUX_OS = "Ubuntu" ]; then
		SYSTEM=Ubuntu
		LINUX_OS_VERSION=`lsb_release -d | awk '{print $3}'`
		if [ ! -f $RC_LOCAL_LOCATION ]; then
			echo ""	
			echo "Check driver loading service file..........NG"
			echo "WARN: $RC_LOCAL_LOCATION does not exit."
			echo ""	
		else
			echo "Check driver loading service file..........OK"
		fi
	fi
else
	LINUX_OS_TMP=`cat /etc/os-release | grep PRETTY_NAME | awk '{print $1}'`
	if [[ $LINUX_OS_TMP =~ "Ubuntu" ]]; then
		SYSTEM=Ubuntu
		LINUX_OS=`lsb_release -d | awk '{print $2}'`
		LINUX_OS_VERSION=`lsb_release -d | awk '{print $3}'`
		if [ ! -f $RC_LOCAL_LOCATION ]; then
			echo ""	
			echo "Check driver loading service file..........NG"
			echo "WARN: $RC_LOCAL_LOCATION does not exit."
			echo ""	
		else
			echo "Check driver loading service file..........OK"
		fi		
	elif [[ $LINUX_OS_TMP =~ "CentOS" ]]; then
		#CentOS7 use '/etc/os-release' to get $LINUX_OS_VERSION
		SYSTEM=CentOS
		LINUX_OS=CentOS
		LINUX_OS_VERSION=`cat /etc/os-release | grep PRETTY_NAME | awk '{print $3}'`
		if [ ! -f $RC_D_LOCAL_LOCATION ]; then
			echo ""	
			echo "Check driver loading service file..........NG"
			echo "WARN: $RC_D_LOCAL_LOCATION does not exit."
			echo ""	
		else
			echo "Check driver loading service file..........OK"
		fi			
	fi
fi

#check kernel driver sign
if [ $install = "normal" ]; then
	DRIVERSIGN=`grep "CONFIG_MODULE_SIG_FORCE" /boot/config-$CURRENT_KERNEL_VERSION`
	if [[ $DRIVERSIGN =~ "CONFIG_MODULE_SIG_FORCE=y" ]]; then	
		echo ""	
		echo "Check kernel driver sign..........NG"
		echo "ERROR: The current kernel allow only signed driver to be loaded."
		echo ""	
		exit 1
	else
		echo "Check kernel driver sign..........OK"
	fi
fi

#check kernel header file
if [ $install = "normal" ]; then	
	KERNEL_HEADER=/lib/modules/$CURRENT_KERNEL_VERSION/build
	if [ ! -d $KERNEL_HEADER ]; then
		echo ""
		echo "Check kernel header file..........NG"
		echo "ERROR: The kernel header($KERNEL_HEADER) corresponding to the kernel version is missing."
		echo "Prompt: If using USB camera, you can use this command to force installation."
		echo "bash *.run force"
		echo ""	
		exit 1
	else
		echo "Check kernel header file..........OK"
	fi
fi

#check kernel version
if [ $install = "normal" ]; then
	MAJOR=$(echo ${CURRENT_KERNEL_VERSION%%.*})
	if [[ $MAJOR -lt $LOWEST_KERNEL_MAJOR_VERSION ]] || [[ $MAJOR -gt $HIGHEST_KERNEL_MAJOR_VERSION ]]; then
		echo ""
		echo "Check the major version of the current kernel($CURRENT_KERNEL_VERSION)..........NG"
		echo "WARN: The minimum kernel version supported by this installation package is $LOWEST_KERNEL_MAJOR_VERSION.$LOWEST_KERNEL_MINOR_VERSION.$LOWEST_KERNEL_REVISION_VERSION."
		echo "WARN: The maximum kernel version supported by this installation package is $HIGHEST_KERNEL_MAJOR_VERSION.$HIGHEST_KERNEL_MINOR_VERSION.$HIGHEST_KERNEL_REVISION_VERSION."
		echo ""		
		#exit 1
	else
		MINOR_TMP=$(echo ${CURRENT_KERNEL_VERSION#*.})
		MINOR=$(echo ${MINOR_TMP%%.*})
		if [[ $MAJOR -eq $LOWEST_KERNEL_MAJOR_VERSION && $MINOR -lt $LOWEST_KERNEL_MINOR_VERSION ]] || 
		[[ $MAJOR -eq $HIGHEST_KERNEL_MAJOR_VERSION && $MINOR -gt $HIGHEST_KERNEL_MINOR_VERSION ]]; then
			echo ""
			echo "Check the minor version of the current kernel($CURRENT_KERNEL_VERSION)..........NG"
			echo "WARN: The minimum kernel version supported by this installation package is $LOWEST_KERNEL_MAJOR_VERSION.$LOWEST_KERNEL_MINOR_VERSION.$LOWEST_KERNEL_REVISION_VERSION."
			echo "WARN: The maximum kernel version supported by this installation package is $HIGHEST_KERNEL_MAJOR_VERSION.$HIGHEST_KERNEL_MINOR_VERSION.$HIGHEST_KERNEL_REVISION_VERSION."
			echo ""			
			#exit 1
		else
			VERSION_TMP=$(echo ${MINOR_TMP#*.})
			VERSION=$(echo ${VERSION_TMP%%-*})
			if [[ $MAJOR -eq $LOWEST_KERNEL_MAJOR_VERSION && $VERSION -lt $LOWEST_KERNEL_REVISION_VERSION ]] || 
			[[ $MAJOR -eq $HIGHEST_KERNEL_MAJOR_VERSION && $VERSION -gt $HIGHEST_KERNEL_REVISION_VERSION ]]; then
				echo ""
				echo "Check the revision version of the current kernel($CURRENT_KERNEL_VERSION)..........NG"
				echo "WARN: The minimum kernel version supported by this installation package is $LOWEST_KERNEL_MAJOR_VERSION.$LOWEST_KERNEL_MINOR_VERSION.$LOWEST_KERNEL_REVISION_VERSION."
				echo "WARN: The maximum kernel version supported by this installation package is $HIGHEST_KERNEL_MAJOR_VERSION.$HIGHEST_KERNEL_MINOR_VERSION.$HIGHEST_KERNEL_REVISION_VERSION."
				echo ""					
				#exit 1
			else
				echo "Check kennel version..........OK"
			fi
		fi
	fi
else
	echo ""
	echo "Entry mandatory installation mode."
fi

HOST_ARCH=`uname -m | sed -e 's/i.86/i686/' -e 's/^armv.*/arm/'`

INSTALL_ROOT=/opt/$1/$2
INSTALL_ROOT_OLD_PATH1=/opt/DahuaTech/MVViewer
INSTALL_ROOT_OLD_PATH2=/opt/DahuaTech/$2

# Initial screen
clear
echo "  ( $LINUX_OS-$LINUX_OS_VERSION-$HOST_ARCH )"
echo "========================================"
echo ""

#log path create
mkdir -p $LOG_PATH
chmod 777 -R $LOG_PATH

# This section is to remove the old directory of the SDK...
# We take care of moving the licenses...
if [ -d $INSTALL_ROOT/lib ] || [ -d $INSTALL_ROOT_OLD_PATH1/lib ] || [ -d $INSTALL_ROOT_OLD_PATH2/lib ]; then
	echo -n "An older version of the MV SDK has been found. Replace [$INSTALL_DIR_OVERWRITE]? "
	read ANSWER
	until [ "$ANSWER" = "yes" -o "$ANSWER" = "no" -o "$ANSWER" = "" ]; do
		echo "Please type yes or no."
		echo -n "An older version of the MV SDK has been found. Replace  [$INSTALL_DIR_OVERWRITE]? "
		read ANSWER
	done
	
	if [ ! "$ANSWER" = "" ]; then
	INSTALL_DIR_OVERWRITE=$ANSWER
	fi
	
	if [ "$INSTALL_DIR_OVERWRITE" = "yes" ]; then
		# Cleanup the old installation if any
		if [ -f "$INSTALL_ROOT/bin/uninstall.sh" ]; then
			$INSTALL_ROOT/bin/uninstall.sh
			
			# Here delete the sub-folder because of the potential existence of a 
			# cross-compiled version of the SDK
			# This version only has license
			rm -rf $INSTALL_ROOT/*	
		elif [ -f "$INSTALL_ROOT_OLD_PATH1/bin/uninstall.sh" ]; then
			$INSTALL_ROOT_OLD_PATH1/bin/uninstall.sh

			# Here delete the sub-folder because of the potential existence of a 
			# cross-compiled version of the SDK
			rm -rf $INSTALL_ROOT_OLD_PATH1/*	
		elif [ -f "$INSTALL_ROOT_OLD_PATH2/bin/uninstall.sh" ]; then
			$INSTALL_ROOT_OLD_PATH2/bin/uninstall.sh

			# Here delete the sub-folder because of the potential existence of a 
			# cross-compiled version of the SDK
			rm -rf $INSTALL_ROOT_OLD_PATH2/*
		fi		
	fi
fi

echo ""
echo "Installing software on ${INSTALL_ROOT}."

mkdir -p $INSTALL_ROOT

#bin directory copy
if [ ! "`ls $START_DIR/bin/* 2>/dev/null`" = "" ]; then
	mkdir -p $INSTALL_ROOT/bin
	if [ $HOST_ARCH = "x86_64" ]; then
		cp -rf $START_DIR/bin/m64x86/* $INSTALL_ROOT/bin
		cp -rf $START_DIR/bin/common/* $INSTALL_ROOT/bin
	elif [ $HOST_ARCH = "i686" ]; then
		cp -rf $START_DIR/bin/m32x86/* $INSTALL_ROOT/bin
		cp -rf $START_DIR/bin/common/* $INSTALL_ROOT/bin
	fi
fi

chmod -R 777 "$INSTALL_ROOT/bin"

#doc directory copy
if [ ! "`ls $START_DIR/doc/* 2>/dev/null`" = "" ]; then
	mkdir -p $INSTALL_ROOT/doc
	cp -rf $START_DIR/doc/* $INSTALL_ROOT/doc
fi

#include directory copy
if [ ! "`ls $START_DIR/include/* 2>/dev/null`" = "" ]; then
	mkdir -p $INSTALL_ROOT/include
	cp -rf $START_DIR/include/* $INSTALL_ROOT/include
fi

#lib directory copy
if [ ! "`ls $START_DIR/lib/* 2>/dev/null`" = "" ]; then
	mkdir -p $INSTALL_ROOT/lib
	if [ $HOST_ARCH = "x86_64" ]; then
		cp -rf $START_DIR/lib/m64x86/* $INSTALL_ROOT/lib
		cp -rf $START_DIR/lib/common/* $INSTALL_ROOT/lib
	elif [ $HOST_ARCH = "i686" ]; then
		cp -rf $START_DIR/lib/m32x86/* $INSTALL_ROOT/lib
		cp -rf $START_DIR/lib/common/* $INSTALL_ROOT/lib
	fi
fi

#license directory copy
if [ -d "$START_DIR/license/" ]; then
	mkdir -p $INSTALL_ROOT/license
	cp -rf $START_DIR/license/* $INSTALL_ROOT/license/
	chmod 777 -R $INSTALL_ROOT/license
fi

#module directory copy
if [ ! "`ls $START_DIR/module/* 2>/dev/null`" = "" ]; then
	#USBDriver director copy
	#mkdir -p $INSTALL_ROOT/module/USBDriver
	#cp -rf $START_DIR/module/common/USBDriver/* $INSTALL_ROOT/module/USBDriver/
	#mkdir -p $INSTALL_ROOT/module/USBDriver/RayDriverApi
	#cp -rf $START_DIR/module/common/RayDriverApi/* $INSTALL_ROOT/module/USBDriver/RayDriverApi
	
	#GigEDriver director copy
	mkdir -p $INSTALL_ROOT/module/GigEDriver
	cp -rf $START_DIR/module/common/GigEDriver/* $INSTALL_ROOT/module/GigEDriver/
	mkdir -p $INSTALL_ROOT/module/GigEDriver/RayDriverApi
	cp -rf $START_DIR/module/common/RayDriverApi/* $INSTALL_ROOT/module/GigEDriver/RayDriverApi

	cp -rf $START_DIR/module/common/loadAllDrv.sh $INSTALL_ROOT/module/
	if [ $HOST_ARCH = "x86_64" ]; then
		#cp -rf $START_DIR/module/m64x86/USBDriver/* $INSTALL_ROOT/module/USBDriver/libModule
		cp -rf $START_DIR/module/m64x86/GigEDriver/* $INSTALL_ROOT/module/GigEDriver/libModule
	elif [ $HOST_ARCH = "i686" ]; then
		#cp -rf $START_DIR/module/m32x86/USBDriver/* $INSTALL_ROOT/module/USBDriver/libModule
		cp -rf $START_DIR/module/m32x86/GigEDriver/* $INSTALL_ROOT/module/GigEDriver/libModule
	fi

	if [ -d $START_DIR/module/common/PCIeCXPDriver/ ]; then
		#PCIeCXPDriver director copy
		mkdir -p $INSTALL_ROOT/module/PCIeCXPDriver
		cp -rf $START_DIR/module/common/PCIeCXPDriver/* $INSTALL_ROOT/module/PCIeCXPDriver/
		mkdir -p $INSTALL_ROOT/module/PCIeCXPDriver/RayDriverApi
		cp -rf $START_DIR/module/common/RayDriverApi/* $INSTALL_ROOT/module/PCIeCXPDriver/RayDriverApi

		if [ $HOST_ARCH = "x86_64" ]; then
			cp -rf $START_DIR/module/m64x86/PCIeCXPDriver/* $INSTALL_ROOT/module/PCIeCXPDriver/libModule
		elif [ $HOST_ARCH = "i686" ]; then
			cp -rf $START_DIR/module/m32x86/PCIeCXPDriver/* $INSTALL_ROOT/module/PCIeCXPDriver/libModule
		fi	
	fi
	
	if [ -d $START_DIR/module/common/PCIeGigEDriver/ ]; then
		#PCIeGigEDriver director copy
		mkdir -p $INSTALL_ROOT/module/PCIeGigEDriver
		cp -rf $START_DIR/module/common/PCIeGigEDriver/* $INSTALL_ROOT/module/PCIeGigEDriver/
		mkdir -p $INSTALL_ROOT/module/PCIeGigEDriver/RayDriverApi
		cp -rf $START_DIR/module/common/RayDriverApi/* $INSTALL_ROOT/module/PCIeGigEDriver/RayDriverApi

		if [ $HOST_ARCH = "x86_64" ]; then
			cp -rf $START_DIR/module/m64x86/PCIeGigEDriver/* $INSTALL_ROOT/module/PCIeGigEDriver/libModule
		elif [ $HOST_ARCH = "i686" ]; then
			cp -rf $START_DIR/module/m32x86/PCIeGigEDriver/* $INSTALL_ROOT/module/PCIeGigEDriver/libModule
		fi	
	fi
	
	if [ -d $START_DIR/module/common/PCIeSerDesDriver/ ]; then
		#PCIeSerDesDriver director copy
		mkdir -p $INSTALL_ROOT/module/PCIeSerDesDriver
		cp -rf $START_DIR/module/common/PCIeSerDesDriver/* $INSTALL_ROOT/module/PCIeSerDesDriver/
		mkdir -p $INSTALL_ROOT/module/PCIeSerDesDriver/RayDriverApi
		cp -rf $START_DIR/module/common/RayDriverApi/* $INSTALL_ROOT/module/PCIeSerDesDriver/RayDriverApi

		if [ $HOST_ARCH = "x86_64" ]; then
			cp -rf $START_DIR/module/m64x86/PCIeSerDesDriver/* $INSTALL_ROOT/module/PCIeSerDesDriver/libModule
		elif [ $HOST_ARCH = "i686" ]; then
			cp -rf $START_DIR/module/m32x86/PCIeSerDesDriver/* $INSTALL_ROOT/module/PCIeSerDesDriver/libModule
		fi	
	fi
	
	if [ -d $START_DIR/module/common/PCIeCXPDriverEx/ ]; then
		#PCIeCXPDriverEx director copy
		mkdir -p $INSTALL_ROOT/module/PCIeCXPDriverEx
		cp -rf $START_DIR/module/common/PCIeCXPDriverEx/* $INSTALL_ROOT/module/PCIeCXPDriverEx/	
	fi
fi

#share directory copy
if [ ! "`ls $START_DIR/share/* 2>/dev/null`" = "" ]; then
	mkdir -p $INSTALL_ROOT/share
	cp -rf $START_DIR/share/* $INSTALL_ROOT/share

	#####Qt Sample#####
	if [ -d "$START_DIR/share/QT/ShowImage/" ]; then
		rm $INSTALL_ROOT/share/QT/ShowImage/ShowImage_aarch64.pro
		rm $INSTALL_ROOT/share/QT/ShowImage/ShowImage_armhf.pro
		if [ $HOST_ARCH = "x86_64" ]; then
			rm $INSTALL_ROOT/share/QT/ShowImage/ShowImage_m32x86.pro
			mv $INSTALL_ROOT/share/QT/ShowImage/ShowImage_m64x86.pro $INSTALL_ROOT/share/QT/ShowImage/ShowImage.pro
		elif [ $HOST_ARCH = "i686" ]; then
			rm $INSTALL_ROOT/share/QT/ShowImage/ShowImage_m64x86.pro
			mv $INSTALL_ROOT/share/QT/ShowImage/ShowImage_m32x86.pro $INSTALL_ROOT/share/QT/ShowImage/ShowImage.pro
		fi
	fi

	if [ -d "$START_DIR/share/QT/ShowQImage/" ]; then
		rm $INSTALL_ROOT/share/QT/ShowQImage/ShowQImage_aarch64.pro
		rm $INSTALL_ROOT/share/QT/ShowQImage/ShowQImage_armhf.pro
		if [ $HOST_ARCH = "x86_64" ]; then
			rm $INSTALL_ROOT/share/QT/ShowQImage/ShowQImage_m32x86.pro
			mv $INSTALL_ROOT/share/QT/ShowQImage/ShowQImage_m64x86.pro $INSTALL_ROOT/share/QT/ShowQImage/ShowQImage.pro
		elif [ $HOST_ARCH = "i686" ]; then
			rm $INSTALL_ROOT/share/QT/ShowQImage/ShowQImage_m64x86.pro
			mv $INSTALL_ROOT/share/QT/ShowQImage/ShowQImage_m32x86.pro $INSTALL_ROOT/share/QT/ShowQImage/ShowQImage.pro
		fi
	fi
	
	chmod 777 -R $INSTALL_ROOT/share
fi

# Install the Noto Sans SC font used by the Qt UI (skipped when already installed)
install_noto_sans_sc

if [ -d $INSTALL_ROOT/lib/ ]; then
	echo ""
	echo "MVSDK flexible connection"
	pushd $INSTALL_ROOT/lib/ >>/dev/null
	#取版本号最大的库文件创建链接，避免多版本共存时 ln 把链接名当成目标目录而失败
	MVSDK_SO_FILE=`ls -1 libMVSDK.so.*.*.*.* 2>/dev/null | sort -V | tail -1`
	if [ -n "$MVSDK_SO_FILE" ]; then
		ln -sf $MVSDK_SO_FILE libMVSDK.so
	else
		echo "WARN: libMVSDK.so.*.*.*.* not found, libMVSDK.so is not created."
	fi
	ln -sf libusb-1.0.so libusb-1.0.so.0
	ln -sf libcompress_decode.so libcompress_decode.so.1
	popd >>/dev/null
fi

if [ -d $INSTALL_ROOT/lib/Qt ]; then
	echo "Qt flexible connection"
	pushd $INSTALL_ROOT/lib/Qt >>/dev/null
	
	if [ $HOST_ARCH = "x86_64" ]; then
		ln -sf libicudata.so.56.1 libicudata.so.56
		ln -sf libicui18n.so.56.1 libicui18n.so.56
		ln -sf libicuuc.so.56.1 libicuuc.so.56

		ln -sf libQt5Concurrent.so.5.6.3 libQt5Concurrent.so.5
		ln -sf libQt5Core.so.5.6.3 libQt5Core.so.5
		ln -sf libQt5DBus.so.5.6.3 libQt5DBus.so.5
		ln -sf libQt5Gui.so.5.6.3 libQt5Gui.so.5
		ln -sf libQt5Network.so.5.6.3 libQt5Network.so.5
		ln -sf libQt5PrintSupport.so.5.6.3 libQt5PrintSupport.so.5
		ln -sf libQt5Widgets.so.5.6.3 libQt5Widgets.so.5
		ln -sf libQt5XcbQpa.so.5.6.3 libQt5XcbQpa.so.5
		ln -sf libQt5Xml.so.5.6.3 libQt5Xml.so.5
	elif [ $HOST_ARCH = "i686" ]; then
		ln -sf libicudata.so.54.1 libicudata.so.54
		ln -sf libicui18n.so.54.1 libicui18n.so.54
		ln -sf libicuuc.so.54.1 libicuuc.so.54

		ln -sf libQt5Core.so.5.5.1 libQt5Core.so.5
		ln -sf libQt5DBus.so.5.5.1 libQt5DBus.so.5
		ln -sf libQt5Gui.so.5.5.1 libQt5Gui.so.5
		ln -sf libQt5Network.so.5.5.1 libQt5Network.so.5
		ln -sf libQt5Widgets.so.5.5.1 libQt5Widgets.so.5
		ln -sf libQt5XcbQpa.so.5.5.1 libQt5XcbQpa.so.5
		ln -sf libQt5Xml.so.5.5.1 libQt5Xml.so.5
	fi
	
	popd >>/dev/null 
fi

# check if we need to add the libraries to be added to the path
echo ""
echo -n "Add libraries to the path"
pushd $INSTALL_ROOT/bin >>/dev/null
./install.sh --install
popd >>/dev/null

# check if we need to open firewall source port 3956
echo -n "open firewall source port 3956"
iptables -I INPUT -p udp --sport 3956 -j ACCEPT

# Build the Universal Pro For Ethernet module when installing
if [ -d $INSTALL_ROOT/module/GigEDriver ]; then
	echo ""
	echo -n "Install MV Universal Pro For Ethernet"
	pushd $INSTALL_ROOT/module/GigEDriver >>/dev/null
	bash build.sh
	bash loadDrv.sh
	popd >>/dev/null 
fi

# Build the Universal Pro For U3V module when installing
#if [ -d $INSTALL_ROOT/module/USBDriver ]; then    
#	echo ""
#	echo -n "Install MV Universal Pro For U3V"
#	pushd $INSTALL_ROOT/module/USBDriver >>/dev/null
#	bash build.sh
	#bash loadDrv.sh
#	popd >>/dev/null
#fi

if [ -d $INSTALL_ROOT/module/PCIeCXPDriver ]; then
	echo ""
	echo -n "Install MV Universal Pro For PCIeCXP"
	echo ""
	pushd $INSTALL_ROOT/module/PCIeCXPDriver >>/dev/null
	bash build.sh
	bash loadDrv.sh
	popd >>/dev/null 
fi

if [ -d $INSTALL_ROOT/module/PCIeGigEDriver ]; then
	echo ""
	echo -n "Install MV Universal Pro For PCIeGigE"
	pushd $INSTALL_ROOT/module/PCIeGigEDriver >>/dev/null
	bash build.sh
	bash loadDrv.sh
	popd >>/dev/null 
fi

if [ -d $INSTALL_ROOT/module/PCIeSerDesDriver ]; then
	echo ""
	echo -n "Install MV Universal Pro For PCIeSerDes"
	echo ""
	pushd $INSTALL_ROOT/module/PCIeSerDesDriver >>/dev/null
	bash build.sh
	bash loadDrv.sh
	popd >>/dev/null 
fi

if [ -d $INSTALL_ROOT/module/PCIeCXPDriverEx ]; then
	echo ""
	echo -n "Install MV Universal Pro For PCIeCXPEx"
	echo ""
	pushd $INSTALL_ROOT/module/PCIeCXPDriverEx >>/dev/null
	bash loadDrv.sh
	popd >>/dev/null 
fi

# Set usbfs_memory_mb to 1000 for libusb
if [ -f "$INSTALL_ROOT/bin/set_usb_stack.sh" ]; then
	echo ""
	echo "Set usbfs_memory_mb to 1000"
	bash $INSTALL_ROOT/bin/set_usb_stack.sh
fi

# start autoSetNegotiation
if [ -f "$INSTALL_ROOT/bin/autoSetNegotiation" ]; then
	echo ""
	echo "start autoSetNegotiation"
	$INSTALL_ROOT/bin/autoSetNegotiation
fi
  
# Ask to run the rp_filter configuration
echo ""
echo "The Linux network stack does not allow the delivery of outbound "
echo "packets by default."
echo ""
echo "For this reason, an interface cannot enumerate devices on a different "
echo "subnet if not on a default gateway. The installer can run a script "
echo "to change the default configuration and work around this issue. "
echo ""
echo -n "Do you want the installer to run the script [yes]?"
read ANSWER
until [ "$ANSWER" = "yes" -o "$ANSWER" = "no" -o "$ANSWER" = "" ]; do
	echo -n "Do you want the installer to run the script [yes]?"
	read ANSWER
done
if [ "$ANSWER" = "" ]; then
	ANSWER="yes"
fi

if [ "$ANSWER" = "yes" ]; then
	bash $INSTALL_ROOT/bin/set_rp_filter.sh --mode=0
else
	echo ""
	echo "INFO: This operation can be done later using the following script:"
	echo "$INSTALL_ROOT/bin/set_rp_filter.sh "
	echo ""
fi

#修正安装目录权限：打包时的 0760 经 umask(022) 复制后变成 0740，
#会导致非 root 用户无法读取库文件（libMVSDK.so: cannot open shared object file）
chmod -R a+rX $INSTALL_ROOT

echo "Installed on $INSTALL_ROOT"
echo "Installation complete."
echo ""
echo ""
