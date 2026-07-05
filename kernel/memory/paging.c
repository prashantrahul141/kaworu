#include "memory/paging.h"
#include "aarch64/aarch64.h"
#include "common_defs.h"
#include "debug/log.h"
#include "error.h"
#include "limine.h"
#include "limine_responses.h"
#include "memlayout.h"
#include "memory/kmem.h"
#include "common_defs.h"
#include "memlayout.h"
#include "string.h"
#include "types.h"
#include "debug/panic.h"
#include "memory/kmem.h"

constexpr usize PAGE_SHIFT = 12;
constexpr usize PT_INDEX_MASK = 0x1FF;

/* convert address to index for a translation level  */
#define L0_INDEX(va) (((u64)(va) >> 39) & PT_INDEX_MASK)
#define L1_INDEX(va) (((u64)(va) >> 30) & PT_INDEX_MASK)
#define L2_INDEX(va) (((u64)(va) >> 21) & PT_INDEX_MASK)
#define L3_INDEX(va) (((u64)(va) >> 12) & PT_INDEX_MASK)

/* convert */
#define PA_TO_PAGE_DESC(pa)	   (((u64)(pa)) >> PAGE_SHIFT)
#define PAGE_DESC_TO_PA(desc_addr) (((u64)(desc_addr)) << PAGE_SHIFT)

/*
 * Kernel root page table, start of address translation tree for kernel.
 * On aarch64, the virtual address space is divided into user space (each
 * process has its own) and kernel space.
 * The kernel virtual address space's root translation table is stored in
 * TTB1_EL1
 * And user space address space's root translation table is stored in
 * TTB0_EL1
 */
TableDescriptor *kernel_page_table;

/* static declarations */

static PageDescriptor *walk_pagetable(TableDescriptor *page, u64 va,
				      bool allocate);

static inline errno_t map_device(TableDescriptor *table, usize va, usize pa,
				 usize size);
static inline errno_t map_text(TableDescriptor *table, usize va, usize pa,
			       usize size);

static inline errno_t map_data(TableDescriptor *table, usize va, usize pa,
			       usize size);
static errno_t map_va_to_pa(TableDescriptor *page, usize va, usize pa,
			    usize size, PagePerms perms, AttrIndex attr_index,
			    PageShareability shareability,
			    ExecPerms privilege_execution,
			    ExecPerms underprivilege_execution);

static MUST_CHECK TableDescriptor *table_next_table(TableDescriptor *td,
						    bool allocate);

static inline void switch_table();

void kpages_init(void)
{
	INFO("Initializing virtual memory manager");

	/* this is the root of address translation tree for kernel */
	kernel_page_table = (TableDescriptor *)kmem_alloc();
	if (IS_ERR(kernel_page_table)) {
		panic("could not allocate for kernel page table, returned = %s",
		      str_err(PTR_TO_ERR(kernel_page_table)));
	}

	memset(kernel_page_table, 0, PAGE_SIZE);

	/* map kernel's text section */
	DEBUG("mapping kernel text section");
	map_text(kernel_page_table, __KERNEL_START,
		 kernel_virt_to_phys(__KERNEL_START),
		 __KERNEL_TEXT_END - __KERNEL_START);

	/* map kernel's data sections */
	DEBUG("mapping kernel data section");
	map_data(kernel_page_table, __KERNEL_DATA_START,
		 kernel_virt_to_phys(__KERNEL_DATA_START),
		 __KERNEL_DATA_END - __KERNEL_DATA_START);

	/* map hhdm entries */
	DEBUG("mapping hhdm entries");
	for (usize index = 0; index < limine_memmap()->entry_count; index++) {
		struct limine_memmap_entry *entry =
			limine_memmap()->entries[index];
		switch (entry->type) {
		case LIMINE_MEMMAP_FRAMEBUFFER: {
			map_device(kernel_page_table,
				   (usize)phys_to_virt(entry->base),
				   entry->base, entry->length);
			break;
		}

		case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
		case LIMINE_MEMMAP_USABLE: {
			map_data(kernel_page_table,
				 (usize)phys_to_virt(entry->base), entry->base,
				 entry->length);
			break;
		}
		default:
			break;
		}
	}

	/* mapping mmios */
	map_device(kernel_page_table, UART_BASE_VIRT, UART_BASE_PHY, PAGE_SIZE);

	DEBUG("mapping complete");

	/* make sure all writes are complete at this point. */
	dsb(BARRIER_ALL);
	isb();

	switch_table();
}

static inline void switch_table()
{
	DEBUG("swapping page table");

	/* write kernel pages to TTB1_EL1 */
	w_ttbr1_el1(virt_to_phys(kernel_page_table));

	/* flush flush flush! */
	dsb(BARRIER_ALL);
	isb();

	tlb_flush();

	dsb(BARRIER_ALL);
	isb();
}

static inline errno_t map_device(TableDescriptor *page, usize va, usize pa,
				 usize size)
{
	DEBUG("mapping device");
	return map_va_to_pa(page, va, pa, size, EL1_READ_WRITE_EL0_NONE,
			    ATTR_INDEX_DEVICE, SHAREABLE_NON_SHAREABLE,
			    NOT_EXECUTABLE, NOT_EXECUTABLE);
}

static inline errno_t map_text(TableDescriptor *page, usize va, usize pa,
			       usize size)
{
	DEBUG("mapping text");
	return map_va_to_pa(page, va, pa, size, EL1_READ_ONLY_EL0_NONE,
			    ATTR_INDEX_NORMAL, SHAREABLE_INNER_SHAREABLE,
			    EXECUTABLE, NOT_EXECUTABLE);
}

static inline errno_t map_data(TableDescriptor *page, usize va, usize pa,
			       usize size)
{
	DEBUG("mapping data");
	return map_va_to_pa(page, va, pa, size, EL1_READ_WRITE_EL0_NONE,
			    ATTR_INDEX_NORMAL, SHAREABLE_INNER_SHAREABLE,
			    NOT_EXECUTABLE, NOT_EXECUTABLE);
}

/* maps given virtual address to physical address to */
static errno_t map_va_to_pa(TableDescriptor *page, usize va, usize pa,
			    usize size, PagePerms perms, AttrIndex attr_index,
			    PageShareability shareability,
			    ExecPerms privilege_execution,
			    ExecPerms underprivilege_execution)
{
	DEBUG("Map va = %p, pa = %p, size = %p", va, pa, size);

	if (!IS_PAGE_ALIGNED(va)) {
		panic("va = %p is not page aligned (%p)", va, PAGE_SIZE);
	}

	if (!IS_PAGE_ALIGNED(size)) {
		panic("size = %p is not page aligned (%p)", size, PAGE_SIZE);
	}

	for (usize end = va + size; va < end;
	     va += PAGE_SIZE, pa += PAGE_SIZE) {
		/* this contains a leaf node which we can map */
		PageDescriptor *pde = walk_pagetable(page, va, true);
		if (pde->field.is_valid) {
			panic("remaping an already existing page va = %p, pde "
			      "= %p",
			      va, pde);
		}

		pde->field.is_page = 0b1;
		pde->field.is_valid = PAGE_IS_VALID;
		pde->field.attr_index = attr_index;
		pde->field.ap = perms;
		pde->field.sh = shareability;
		pde->field.af = AF_SET;
		pde->field.output_address = PA_TO_PAGE_DESC(pa);
		pde->field.pxn = privilege_execution;
		pde->field.uxn_xn = underprivilege_execution;
	}

	return EOK;
}

/* walks the given page table and return leaf descriptor */
static PageDescriptor *walk_pagetable(TableDescriptor *table, u64 va,
				      bool allocate)
{
	TRACE("walking table = %p, va = %p, allocate = %b", table, va,
	      allocate);

	usize l0_index = L0_INDEX(va);
	TableDescriptor *l1 =
		table_next_table((TableDescriptor *)&table[l0_index], allocate);

	TRACE("\tl0_index =  %d, l1_table = %p", l0_index, l1);
	if (IS_ERR(l1)) {
		panic("failed to get next table for l1 va = %p, l0 index = %d, "
		      "allocate = %b",
		      va, l0_index, allocate);
	}

	usize l1_index = L1_INDEX(va);
	TableDescriptor *l2 =
		table_next_table((TableDescriptor *)&l1[l1_index], allocate);

	TRACE("\tl1_index =  %d, l2_table = %p", l1_index, l2);
	if (IS_ERR(l2)) {
		panic("failed to get next table for l2 va = %p, l1 index = %d, "
		      "allocate = %b",
		      va, l1_index, allocate);
	}

	usize l2_index = L2_INDEX(va);
	TableDescriptor *l3 =
		table_next_table((TableDescriptor *)&l2[l2_index], allocate);

	TRACE("\tl2_index =  %d, l3_table = %p", l2_index, l3);
	if (IS_ERR(l3)) {
		panic("failed to get next table for l3 va = %p, l2 index = %d, "
		      "allocate = %b",
		      va, l2_index, allocate);
	}

	return (PageDescriptor *)&l3[L3_INDEX(va)];
}

/* gets next level */
static MUST_CHECK TableDescriptor *table_next_table(TableDescriptor *td,
						    bool allocate)
{
	/* table already exists */
	if (td->field.is_valid) {
		return phys_to_virt(
			PAGE_DESC_TO_PA(td->field.next_level_address));
	}

	if (!allocate) {
		return ERR_TO_PTR(-ENOMEM);
	}

	TableDescriptor *page = kmem_alloc();
	td->raw = 0;
	td->field.is_valid = true;
	td->field.is_table = true;
	td->field.next_level_address = PA_TO_PAGE_DESC(virt_to_phys(page));
	return page;
}
