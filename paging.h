#ifndef PAGING_H
#define PAGING_H

#include "isr.h"

/* page directory/table entry for 4k pages */
union page {
	struct {
		unsigned int present	: 1;	/* page/pagetable in memory */
		unsigned int rw		: 1;	/* read/write if set */
		unsigned int user	: 1;	/* user access */
		unsigned int pwt	: 1;	/* page-level write trough */
		unsigned int pcd	: 1;	/* page-level cache disable */
		unsigned int accessed	: 1;	/* set if accessed */
		unsigned int dirty	: 1;	/* pde: ignored */
		unsigned int pat	: 1;	/* pde: must be zero */
		unsigned int g		: 1;	/* pde: ignored, pte: global */
		unsigned int ignored	: 3;	/* pte/pde: ignored */
		unsigned int address	: 20;	/* pde: address of page table
			                           pte: frame address
					   	   physical addresses */
	} __attribute__((packed));
	unsigned int raw;
} __attribute__((packed));

struct page_table {
	union page pages[1024];
} __attribute__((packed));

struct page_directory {
	/* virtual addresses of page tables */
	struct page_table *tables[1024];
	/* actual page directory */
	union page tables_physical[1024];
	/* physical address of tables_physical */
	unsigned int physical_address;
} __attribute__((packed));

void init_paging();

#endif
