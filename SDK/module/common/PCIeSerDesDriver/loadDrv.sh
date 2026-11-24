#!/bin/bash

# Variables
CHMOD_BIN=chmod
CP_BIN=cp
ECHO_BIN=echo
MAKE_BIN=make
MKDIR_BIN=mkdir
MV_BIN=mv
POP_BIN=popd
PUSHD_BIN=pushd

HOST_VERSION=`uname -r`
OUT_FOLDER=$PWD
SRC_FOLDER=$PWD/drivers
KERNEL_FOLDER=
PREFIX=
PATH_TOOLS=

HOST_ARCH=`uname -m | sed -e 's/i.86/i686/' -e 's/^armv.*/arm/'`
if [ $HOST_ARCH = "x86_64" ]; then
  platform=m64x86
  traceLev=11
elif [ $HOST_ARCH = "i686" ]; then
  platform=m32x86
  traceLev=11
else
  platform=arm
  traceLev=11
fi

DRIVER_NAME=imvSerDesFrameGrabber

################################################################################
#
# loadDrv.sh
#
################################################################################

#Display the help for this script
DisplayHelp()
{
    echo ""
    echo "NAME"
    echo "    loadDrv.sh - Load the SerDes Frame Grabber driver module "
    echo ""
    echo "SYNOPSIS"
    echo "    bash loadDrv.sh [--help]"
    echo ""
    echo "DESCRIPTION"
    echo "    Load the SerDes Frame Grabber module and configure the system to be ready to use"
    echo ""
    echo "    This script can only used by root or sudoer"
    echo "    --help             Display this help"
    echo ""
    echo ""
}

#Print out the error and exit 
# $1 Error message
# $2 Exit code
ErrorOut()
{
	echo ""
	echo "Error: $1"
	echo ""
	exit $2
}

# Parse the input arguments
for i in $*
do
    case $i in        
        --help)
            DisplayHelp
            exit 0
        ;;
        *)
        # unknown option
        DisplayHelp
        exit 1
        ;;
    esac
done

# Check required priviledge
if [ `whoami` != root ]; then
	ErrorOut "This script can only be run by root user or sudoer" 1
fi

DRIVER_FILE=$DRIVER_NAME.ko
if [ ! -e $DRIVER_FILE ]; then
	ErrorOut "$DRIVER_NAME.ko file is not exist!" 1
fi

# Load the module
echo "Loading SerDes Frame Grabber Driver Module..."
insmod ./$DRIVER_NAME.ko $* trace_param=$traceLev
if [ $? != 0 ]; then
	linuxVermagic=`uname -r`
	driverInfo=`modinfo $DRIVER_NAME.ko`
	driverInfo=`echo ${driverInfo#*vermagic:}`
	driverVermagic=`echo ${driverInfo%%SMP*}`
	if [ "$linuxVermagic" != "$driverVermagic" ];then
		echo ""
		echo "Linux Vermagic [$linuxVermagic]"
		echo "Driver Vermagic [$driverVermagic]"
		ErrorOut "Vermagic is not match!" 1
	else
		exit 1
	fi
fi

echo "Loading SerDes Frame Grabber drivers successfully!"