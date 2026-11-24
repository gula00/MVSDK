// RayPCIe.h

#ifndef INCLUDED_RAYPCIE_H
#define INCLUDED_RAYPCIE_H

#define RAY_BUILD_BUG_ON(condition) extern char (*RAY_Bug(void)) [sizeof(char[1-2*!!(condition)])]

// <1> io

extern void RAY_writeb(void* RegBase, unsigned char val);
extern void RAY_writew(void* RegBase, unsigned short val);
extern void RAY_writel(void* RegBase, unsigned int val);
extern void RAY_writeq(void* RegBase, unsigned long long val);

extern void RAY_writeb_relaxed(void* RegBase, unsigned char val);
extern void RAY_writew_relaxed(void* RegBase, unsigned short val);
extern void RAY_writel_relaxed(void* RegBase, unsigned int val);
extern void RAY_writeq_relaxed(void* RegBase, unsigned long long val);

extern unsigned char RAY_readb(void* RegBase);
extern unsigned short RAY_readw(void* RegBase);
extern unsigned int RAY_readl(void* RegBase);
extern unsigned long long RAY_readq(void* RegBase);

extern unsigned char RAY_readb_relaxed(void* RegBase);
extern unsigned short RAY_readw_relaxed(void* RegBase);
extern unsigned int RAY_readl_relaxed(void* RegBase);
extern unsigned long long RAY_readq_relaxed(void* RegBase);

extern void* RAY_pci_get_drvdata(void *pdev);
extern void RAY_pci_set_drvdata(void *dev, void *pdev);

extern int RAY_request_irq(
	void *dev,
	unsigned int msi_count,
	int *InterruptIrq,
	unsigned int(*interrputfn)(int irq, void *data),
	unsigned int(*thread_fn)(int irq, void *data),
	const char *name,
	void* pData
);
extern int RAY_release_request_irq(void *dev, unsigned int msi_count, int *InterruptIrq, void* pData);

extern void* RAY_dma_alloc_coherent(void *dev, unsigned int size, void *dma_handle);
extern void RAY_dma_free_coherent(void *dev, unsigned int size, void *cpu_addr, void* dma_handle);

extern int RAY_get_user_pages(unsigned long long userAddr, int pageCount, void **pageList);
extern void RAY_put_page(void **pageList, int pageCount);

extern long long unsigned int RAY_virt_to_phys(void* addr);
extern int RAY_remap_pfn_range(void *vma, unsigned long long start, unsigned long long phys, unsigned long size);

// <2> vmap
// include/linux/vmalloc.h

#define RAY_VM_MAP		0
#define RAY_PAGE_KERNEL 0

extern void* RAY_vmap(void **pageList, int pageCount, int mapFlag, int pageFlag);
extern void RAY_vunmap(void *address);

extern int RAY_get_pid(void);

#endif // INCLUDED_RAYPCIE_H
