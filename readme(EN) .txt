■Installation Package

  1. Suitable for X86 Linux platform with 32bit/64bit OS that confirms to below conditions: 
     a. glibc 2.12 Version and above;
     b. Linux kernal from 2.6.32(including) to 7.0.0-14-generic(including) （Ubuntu 2604）

■Notes

  1. Use root to install or uninstall SDK;
  2. Run install file;
  3. During installing progress, input 【Yes】 to all options to use avoiding other App script again;
  4. After finish, files content will be /opt/HuarayTech/MVviewer, structure of contents as below:
     MVviewer
       ├ bin		: GUI Tool, discover/connect camera, image acquisition, properties setting, uninstall script and etc,;
       ├ include	: SDK head files, image convert head files;
       ├ lib		: SDK dll, third-party library, image converting lib; 
       ├ licenses	: licenses note; 
       ├ module
       │  └ GigEDriver : GigE camera driver;
       │  └ PCIeCXPDriver : CXP capture card driver;
       │  └ PCIeSerDesDriver : SerDes capture card driver;
       └ share
           ├ C          : C samples;     
           ├ Qt         : Qt samples;
           └ Python     : Python samples; 

■Notes for running a script

  1. Execute run.sh in path of /opt/HuarayTech/MVviewer/bin to boot client software;
  2. After plug-in USB camera, please connect camera after LED on rear case turns to blue.Some Malfunctions will be avoided; 
  3. If usb camera is already connected to PC before install *.run file, plese reconnect camera physically again;
  4. If usb camera can't be discovered after connected to PC, please use “lsusb” command to assure camera is recognized by PC correctly;
  5. Before connect GigE camera, please assure camera and PC are sharing same network segment; 

■Special note

  1. Because it will be more strict when new version of libpng inspects png images; below log will show but program can run normally;
     "libpng warning: iCCP: know incorrect sRGB profile"

  2. GigE/USB drivers will be created based on version of kernel while installing. If Kernel is upgraded after install SDK, 
     drivers won't be loaded into new kernel directly. Users should reinstall SDK to create new drivers. 

  3. In the Ubuntu 23.04 system, there might be an issue where the compilation of the kernel cannot find the stdarg.h header file, 
     even though the file actually exists in the system. If this problem occurs, you need to create a symbolic link for the header file. Enter the command:
     sudo ln -s /usr/lib/gcc/x86_64-linux-gnu/12/include/stdarg.h stdarg.h
     Note: The specific path should match the actual location of stdarg.h in your environment.

-END-

