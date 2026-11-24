#include "pcieCXPDriver.h"

#ifndef __devinit
#define __devinit
#define __devexit
#define __devexit_p
#endif

static struct class *	gs_MymoduleClass;
static int				gs_nDevMajor;

unsigned int trace_param = 0;
module_param(trace_param, uint, S_IRUGO | S_IWUSR);

/**
 * Called by the OS when the device is ready for access. Returns 0 on success,
 * negative value on failure.
 */
static int __devinit pcie_probe(struct pci_dev *dev, const struct pci_device_id *id) 
{
	PPCI_DEVICE_CONTEXT pDevContext = NULL;
	int					nRet = 0;

	// Setup the PCIe device.
	nRet = pci_enable_device(dev);
	if (nRet < 0)
	{
		LOG_ERROR("pci_enable_device returned %d", nRet);
		return -ENODEV;
	}

	// Enable bus master
	pci_set_master(dev);

	// Setup the BAR memory regions
	nRet = pci_request_regions(dev, DEVICE_CLASS);
	if (nRet < 0)
	{
		LOG_ERROR("pci_request_regions returned error: %d", nRet);
		pci_disable_device(dev);
		return -ENODEV;
	}

	// Allocate device structure.
	pDevContext = (PPCI_DEVICE_CONTEXT)kzalloc(sizeof(PCI_DEVICE_CONTEXT), GFP_KERNEL);
	if (NULL == pDevContext)
	{
		LOG_ERROR("not enough memory to allocate DevContext");
		pci_release_regions(dev);
		pci_disable_device(dev);
		return -ENODEV;
	}

	pDevContext->BarPAddr = pci_resource_start(dev, 0) | (pci_resource_start(dev, 1) << 32);
	pDevContext->RegsLength = PCIE_BAR_CONFIG_LENGTH;
	pDevContext->RegsFlags = pci_resource_flags(dev, 0);

	pDevContext->BarVAddr = ioremap(pDevContext->BarPAddr, pDevContext->RegsLength);
	if (NULL == pDevContext->BarVAddr)
	{
		LOG_ERROR("could not ioremap BAR 0");
		kfree(pDevContext);
		pci_release_regions(dev);
		pci_disable_device(dev);
		return -ENODEV;
	}

	pDevContext->MaxPayloadSize = pcie_get_mps(dev);

	pDevContext->Domain = pci_domain_nr(dev->bus);
	pDevContext->BusNumber = dev->bus->number;
	pDevContext->Devfn = dev->devfn;

	pDevContext->PciDevice = dev;

	do
	{
		dma_set_mask_and_coherent(&dev->dev, DMA_BIT_MASK(32));
		nRet = File_OperateProbe(pDevContext);
		if (nRet < 0)
		{
			LOG_ERROR("load fail");
			nRet = -ENODEV;
			break;
		}

		snprintf(pDevContext->DeviceID, PCIE_DEVICE_ID_LENGTH, "%s/%s", DEVICE_CLASS, pDevContext->Name);
		nRet = create_cxp_device( &pDevContext->MymoduleClass,
								  &pDevContext->DevMajor,
								  pDevContext->DeviceNum,
								  pDevContext->Name,
								  pDevContext->DeviceID
								  );
		if (nRet < 0)
		{
			break;
		}
		dma_set_mask_and_coherent(&dev->dev, DMA_BIT_MASK(64));
	} while (0);
	
	if (nRet < 0)
	{
		File_OperateRemove(pDevContext);
		iounmap(pDevContext->BarVAddr);
		kfree(pDevContext);
		pci_release_regions(dev);
		pci_disable_device(dev);
		return -ENODEV;
	}
	
	return 0;
}

static int pcie_resume(struct pci_dev *dev)
{
	void** pData = NULL;

	pData = pci_get_drvdata(dev);
	if (NULL == pData)
	{
		return -ENOTTY;
	}

	return File_OperateResume((PPCI_DEVICE_CONTEXT)(*pData));
}

/**
 * Called when the device is unloaded.
 */
static void __devexit pcie_remove(struct pci_dev *dev) 
{
	PPCI_DEVICE_CONTEXT pDevContext = NULL;
	void**				pData = NULL;

	pData = pci_get_drvdata(dev);
	if (NULL != pData)
	{
		pDevContext = (PPCI_DEVICE_CONTEXT)(*pData);

		File_OperateRemove(pDevContext);

		destroy_cxp_device(pDevContext->MymoduleClass, pDevContext->DevMajor, pDevContext->DeviceNum, pDevContext->Name);

		iounmap(pDevContext->BarVAddr);
		kfree(pDevContext);

		pci_release_regions(dev);
		pci_disable_device(dev);
	}

	return;
}

static int pcie_open(struct inode *inode, struct file *filp)
{
	int nRet = 0;

	if (NULL == filp)
	{
		return -ENOTTY;
	}

	nRet = File_OperateOpen(&filp->private_data, filp->f_path.dentry->d_iname);
	if (nRet < 0)
	{
		return -EINVAL;
	}

	return 0;
}

static long pcie_ioctl(struct file *filp, unsigned int ioctlnum, unsigned long ioctlparam)
{
	if (NULL == filp)
	{
		return -ENOTTY;
	}

	return File_OperateIoCtrl(filp->private_data, ioctlnum, ioctlparam);
}

static int pcie_mmap(struct file *filp, struct vm_area_struct *vma)
{
	if (NULL == filp)
	{
		return -ENOTTY;
	}

	return File_OperateMmap(&filp->private_data, (void*)vma, vma->vm_start, PAGE_ALIGN(vma->vm_end - vma->vm_start), vma->vm_pgoff);
}

static int pcie_close(struct inode *inode, struct file *filp)
{
	if (NULL == filp)
	{
		return -ENOTTY;
	}

	return File_OperateClose(&filp->private_data, filp->f_path.dentry->d_iname);
}

///////////////////////////////////////////////////////
// MODULE INIT/EXIT FUNCTIONS
///////////////////////////////////////////////////////

static DEFINE_PCI_DEVICE_TABLE(pcie_ids) = {
	{ PCI_DEVICE(PCIE_CXP6_VENDOR_ID, PCIE_CXP6_PRODUCT_ID) },
	{ PCI_DEVICE(PCIE_CXP12_VENDOR_ID, PCIE_CXP12_PRODUCT_ID) },
	{ 0 },
};

MODULE_DEVICE_TABLE(pci, pcie_ids);
static struct pci_driver PciDriver = {
	.name			= DEVICE_CLASS,
	.id_table		= pcie_ids,
	.probe			= pcie_probe,
	.resume			= pcie_resume,
	.remove			= __devexit_p(pcie_remove),
};

struct file_operations pcie_fops = {
	.owner			= THIS_MODULE,
	.open			= pcie_open,
	.unlocked_ioctl = pcie_ioctl,
	.mmap			= pcie_mmap,
	.release		= pcie_close,
};

/**
 * Called to initialize the PCI device. 
 */
static int __init pcie_init(void) 
{
	int nRet = 0;

	File_OperateInit();

	nRet = create_cxp_device(&gs_MymoduleClass, &gs_nDevMajor, 0, DRIVER_DEFAULT_DEVICE_NAME, DRIVER_DEFAULT_DEVICE_PATH);
	if (nRet < 0)
	{
		return -1;
	}

	return pci_register_driver(&PciDriver);
}

/**
 * Called to destroy the PCI device.
 */
static void __exit pcie_exit(void)
{
	pci_unregister_driver(&PciDriver);

	destroy_cxp_device(gs_MymoduleClass, gs_nDevMajor, 0, DRIVER_DEFAULT_DEVICE_NAME);

	File_OperateExit();

	return;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,2,0)
static int RAY_uevent(struct device *dev, struct kobj_uevent_env *env)
#else 
static int RAY_uevent(const struct device *dev, struct kobj_uevent_env *env)
#endif
{
	add_uevent_var(env, "DEVMODE=%#o", 0777);
	return 0;
}
#endif

struct class * create_cxp_class(const char *name)
{
	struct class *pClass = NULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,4,0)
	pClass = class_create(THIS_MODULE, name);
#else 
	pClass = class_create(name);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
	if (pClass != NULL)
	{
		pClass->dev_uevent = RAY_uevent;
	}
#endif

	return pClass;
}

int create_cxp_device(struct class **pClass, int *major, int index, const char* name, const char* path)
{
	struct class *_pClass = NULL;
	int _major = 0;

	_pClass = create_cxp_class(name);
	if (IS_ERR(_pClass))
	{
		return PTR_ERR(_pClass);
	}

	_major = register_chrdev(0, name, &pcie_fops);
	device_create(_pClass, NULL, MKDEV(_major, index), NULL, path);

	*pClass = _pClass;
	*major = _major;

	return 0;
}

void destroy_cxp_device(struct class *pClass, int major, int index, const char* name)
{
	if (NULL != pClass)
	{
		device_destroy(pClass, MKDEV(major, index));
		unregister_chrdev(major, name);
		class_destroy(pClass);
	}

	return;
}

module_init(pcie_init);
module_exit(pcie_exit);

MODULE_AUTHOR(FILTER_COMPANY);
MODULE_LICENSE(FILTER_LICENSE);
MODULE_DESCRIPTION(FILTER_MODULE_NAME);
