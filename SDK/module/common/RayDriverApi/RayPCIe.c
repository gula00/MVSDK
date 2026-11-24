// RayPCIe.c

#include "RayPCIe.h"

#include <linux/version.h>
#include <linux/pci.h>
#include <linux/vmalloc.h>

///////////////////////////////////////////////////////////
// <1> io

extern struct file_operations pcie_fops;

void RAY_writeb(void* RegBase, unsigned char val)
{
	writeb(val, RegBase);
}

void RAY_writew(void* RegBase, unsigned short val)
{
	writew(val, RegBase);
}

void RAY_writel(void* RegBase, unsigned int val)
{
	writel(val, RegBase);
}

void RAY_writeq(void* RegBase, unsigned long long val)
{
	writeq(val, RegBase);
}

void RAY_writeb_relaxed(void* RegBase, unsigned char val)
{
	writeb_relaxed(val, RegBase);
}

void RAY_writew_relaxed(void* RegBase, unsigned short val)
{
	writew_relaxed(val, RegBase);
}

void RAY_writel_relaxed(void* RegBase, unsigned int val)
{
	writel_relaxed(val, RegBase);
}

void RAY_writeq_relaxed(void* RegBase, unsigned long long val)
{
	writeq_relaxed(val, RegBase);
}

unsigned char RAY_readb(void* RegBase)
{
	return readb(RegBase);
}

unsigned short RAY_readw(void* RegBase)
{
	return readw(RegBase);
}

unsigned int RAY_readl(void* RegBase)
{
	return readl(RegBase);
}

unsigned long long RAY_readq(void* RegBase)
{
	return readq(RegBase);
}

unsigned char RAY_readb_relaxed(void* RegBase)
{
	return readb_relaxed(RegBase);
}

unsigned short RAY_readw_relaxed(void* RegBase)
{
	return readw_relaxed(RegBase);
}

unsigned int RAY_readl_relaxed(void* RegBase)
{
	return readl_relaxed(RegBase);
}

unsigned long long RAY_readq_relaxed(void* RegBase)
{
	return readq_relaxed(RegBase);
}

void* RAY_pci_get_drvdata(void *pdev)
{
	return pci_get_drvdata((struct pci_dev *)pdev);
}

void RAY_pci_set_drvdata(void *dev, void *pdev)
{
	pci_set_drvdata((struct pci_dev *)dev, pdev);
}

int RAY_request_irq(void *dev, unsigned int msi_count, int *InterruptIrq, unsigned int(*interrputfn)(int irq, void *data), unsigned int(*thread_fn)(int irq, void *data), const char *name, void* pData)
{
	int nRet = 0, i = 0;

	msi_count = pci_alloc_irq_vectors((struct pci_dev *)dev, 1, msi_count, PCI_IRQ_MSI);
	
	for (i = 0; i < msi_count; ++i)
	{
		InterruptIrq[i] = pci_irq_vector((struct pci_dev *)dev, i);

		nRet = request_threaded_irq(InterruptIrq[i], interrputfn, thread_fn, 0, name, pData);
		if (nRet != 0)
		{
			RAY_release_request_irq(dev, i, InterruptIrq, pData);
			return nRet;
		}
	}
	
	return msi_count;
}

int RAY_release_request_irq(void *dev, unsigned int msi_count, int *InterruptIrq, void* pData)
{
	int i = 0;

	for (i = 0; i < msi_count && InterruptIrq[i] > 0; ++i)
	{
		free_irq(InterruptIrq[i], pData);
		InterruptIrq[i] = 0;
	}
	pci_free_irq_vectors(dev);

	return 0;
}

void* RAY_dma_alloc_coherent(void *pciDev, unsigned int size, void *dma_handle)
{
	return dma_alloc_coherent(&((struct pci_dev*)pciDev)->dev, size, (dma_addr_t *)dma_handle, GFP_ATOMIC);
}

void RAY_dma_free_coherent(void *pciDev, unsigned int size, void *cpu_addr, void* dma_handle)
{
	dma_free_coherent(&((struct pci_dev*)pciDev)->dev, size, cpu_addr, (dma_addr_t)dma_handle);
}

int RAY_get_user_pages(unsigned long long userAddr, int pageCount, void **pageList)
{
	int nRet = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
	mmap_read_lock(current->mm);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,5,0)
	nRet = get_user_pages(userAddr & PAGE_MASK, pageCount, FOLL_WRITE, (struct page **)pageList, NULL);
#else
	nRet = get_user_pages(userAddr & PAGE_MASK, pageCount, FOLL_WRITE, (struct page **)pageList);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
	mmap_read_unlock(current->mm);
#endif

	return nRet;
}

void RAY_put_page(void **pageList, int pageCount)
{
	int pageNum = 0;

	for (pageNum = 0; pageNum < pageCount; ++pageNum)
	{
		put_page((struct page *)pageList[pageNum]);
	}
}

long long unsigned int RAY_virt_to_phys(void* addr)
{
	return virt_to_phys(addr);
}

int RAY_remap_pfn_range(void *vma, unsigned long long start, unsigned long long phys, unsigned long size)
{
	return remap_pfn_range((struct vm_area_struct*)vma, start, phys >> PAGE_SHIFT, size, PAGE_SHARED);
}

///////////////////////////////////////////////////////////
// <2> vmap

void* RAY_vmap(void **pageList, int pageCount, int mapFlag, int pageFlag)
{
	int			myMapFlag = VM_MAP;
	pgprot_t	myPageFlag = PAGE_KERNEL;

	switch (mapFlag)
	{
	case RAY_VM_MAP:
		myMapFlag = VM_MAP;
	default:
		break;
	}

	switch (pageFlag)
	{
	case RAY_PAGE_KERNEL:
		myPageFlag = PAGE_KERNEL;
	default:
		break;
	}

	return vmap((struct page **)pageList, pageCount, myMapFlag, myPageFlag);
}

void RAY_vunmap(void *address)
{
	vunmap((void*)(((unsigned long)address) & PAGE_MASK));
}

int RAY_get_pid(void)
{
	return current->tgid;
}
