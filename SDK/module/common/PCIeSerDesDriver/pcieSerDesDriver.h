#ifndef __PCIE_DRIVER_H__
#define __PCIE_DRIVER_H__

#include "RayDriverApi/RayApi.h"
#include "RayDriverApi/RayPCIe.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/rwsem.h>
#include <linux/dma-mapping.h>
#include <linux/pagemap.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <linux/cdev.h>
#include <linux/mm.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)
#define DEFINE_PCI_DEVICE_TABLE(_table) const struct pci_device_id _table[]
#endif

#define DEVICE_CLASS					"imvSerDesFrameGrabber"

#define FILTER_MODULE_NAME 				"Driver For SerDes Vision Devices"
#define FILTER_COMPANY					"Machine Vision"
#define FILTER_LICENSE					"GPL"

#define PCIE_SERDES_VENDOR_ID			0x1EDB
#define PCIE_SERDES_PRODUCT_ID			0x6037

#define PCIE_BAR_CONFIG_LENGTH			0x8000

#define DRIVER_DEFAULT_NAME				"imvSerDesFrameGrabber"
#define DRIVER_DEFAULT_NAME_PATH		"imvSerDesFrameGrabber/0"

#define PCIE_DEVICE_ID_LENGTH			256

#define LOG_DEBUG(msg, ...) \
	printk(KERN_INFO "%s: [DEBUG][%s(%d) %s] " msg "\n", DEVICE_CLASS, __FILE__, __LINE__, __func__,  ##__VA_ARGS__);

#define LOG_INFO(msg, ...) \
	printk(KERN_INFO "%s: [INFO][%s(%d) %s] " msg "\n", DEVICE_CLASS, __FILE__, __LINE__, __func__,  ##__VA_ARGS__);

#define LOG_WARN(msg, ...) \
	printk(KERN_WARN "%s: [WARN][%s(%d) %s] " msg "\n", DEVICE_CLASS, __FILE__, __LINE__, __func__,  ##__VA_ARGS__);

#define LOG_ERROR(msg, ...) \
	printk(KERN_ERR "%s: [ERROR][%s(%d) %s] " msg "\n", DEVICE_CLASS, __FILE__, __LINE__, __func__,  ##__VA_ARGS__);

typedef struct _PCI_DEVICE_CONTEXT {
	struct pci_dev			*PciDevice;

	unsigned long long		BarPAddr;
	void __iomem			*BarVAddr;
	unsigned long long		RegsLength;
	unsigned long long		RegsFlags;

	unsigned int			Domain;
	unsigned int			BusNumber;
	unsigned int			Devfn;

	unsigned int			MaxPayloadSize;
	unsigned int			LinkSpeed;
	unsigned int			LinkWidth;

	struct class *			MymoduleClass;
	unsigned char			Name[16];
	int						DevMajor;
	unsigned int			DeviceNum;

	unsigned char			DeviceID[PCIE_DEVICE_ID_LENGTH];
}PCI_DEVICE_CONTEXT, *PPCI_DEVICE_CONTEXT;

void File_OperateInit(void);

void File_OperateExit(void);

int File_OperateProbe(void* pDevContext);

int File_OperateResume(void* pDevContext);

int File_OperateRemove(void* pDevContext);

int File_OperateOpen(void** pData, unsigned char *fileName);

int File_OperateClose(void** pData, unsigned char *fileName);

int File_OperateIoCtrl(void* pData, unsigned int IoControlCode, unsigned long long ioctlparam);

int File_OperateMmap(void** pData, void* vma, unsigned long long start, unsigned long size, unsigned long offset);

struct class * _class_create(const char *name);

int _create_device(struct class **pClass, int *major, int index, const char* name, const char* path);

void _destroy_device(struct class *pClass, int major, int index, const char* name);

#endif