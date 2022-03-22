#ifndef Moca_VMM
#define Moca_VMM

#define PAGES_PER_TABLE		1024
#define PAGES_PER_DIR		1024
#define PAGE_SIZE		4096
#define PTABLE_ADDR_SPACE_SIZE	0x400000
#define DTABLE_ADDR_SPACE_SIZE	0x100000000
#define PD_INDEX(address) ((address) >> 22)
#define PT_INDEX(address) (((address) >> 12) & 0x3FF)
#define PAGE_PHYS_ADDR(dir_entry) ((*dir_entry) & ~0xFF)
#define SET_ATTRIB(entry, attr) (*entry |= attr)
#define CLEAR_ATTRIB(entry, attr) (*entry &= ~attr)
#define TEST_ATTRIB(entry, attr) (*entry & attr)
#define SET_FRAME(entry, addr) (*entry = (*entry & ~0x7FFFF000))

enum pte_flags
{
	pte_present		= 1,
	pte_writeable		= 2,
	pte_user		= 4,
	pte_writethough		= 8,
	pte_not_cacheable	= 0x10,
	pte_accessed		= 0x20,
	pte_dirty		= 0x40,
	pte_pat			= 0x80,
	pte_cpu_global		= 0x100,
	pte_lv4_global		= 0x200,
	pte_frame		= 0x7FFFF000
};

enum pde_flags
{
	pde_present		= 1,
	pde_writable		= 2,
	pde_user		= 4,
	pde_pwt			= 8,
	pde_pcd			= 0x10,
	pde_accessed		= 0x20,
	pde_dirty		= 0x40,
	pde_4mb			= 0x80,
	pde_cpu_global		= 0x100,
	pde_lv4_global		= 0x200,
	pde_frame		= 0x7FFFF000
};

typedef struct ptable
{
	uint32 m_entries[PAGES_PER_TABLE];
} _ptable;

typedef struct pdir
{
	uint32 m_entries[PAGES_PER_DIR];
} _pdir;

uint32 pt_entry;
uint32 pd_entry;
uint32 virtual_address;

MProcess allocations;
_pdir *current_dir;

void alloc_page(uint32 *e)
{
	/* PMM handles errors of no physical block is available. */
	MProcess p = m_alloc(1);

	SET_FRAME(e, (uint32)p.address);
	SET_ATTRIB(e, pte_present);

	allocations = p;
}

void free_page(uint32 *e)
{
	void *addr = (void *)PAGE_PHYS_ADDR(e);
	if(addr)
		m_free(&allocations);

	CLEAR_ATTRIB(e, pte_present);
}

uint32 *page_lookup_entry(_ptable *p, uint32 addr)
{
	if(p)
		return &p->m_entries[ PT_INDEX(addr) ];
	return 0;
}

uint32 *dir_lookup_entry(_pdir *p, uint32 addr)
{
	if(p)
		return &p->m_entries[ PT_INDEX(addr) ];
	return 0;
}

uint8 set_page_dir(_pdir *pd)
{
	if(!pd) return 1;

	current_dir = pd;

	__asm volatile("movl %%EAX, %%CR3" : : "a"(current_dir));

	return 0;
}

void flush_tlb_entry(uint32 addr)
{
	__asm volatile("cli; invlpg (%0); sti" : : "r"(addr));
}

void map_page(void *phys, void *virt)
{
	_pdir *page_dir = current_dir;

	uint32 *e = &page_dir->m_entries[ PD_INDEX((uint32) virt) ];

	if((*e & pte_present) != pte_present)
	{
		MProcess p = m_alloc(1);
		_ptable *table = (_ptable *)p.address;
		
		memset(table, 0, sizeof(_ptable));

		uint32 *entry = &page_dir->m_entries[ PD_INDEX((uint32) virt) ];

		SET_ATTRIB(entry, pde_present);
		SET_ATTRIB(entry, pde_writable);
		SET_FRAME(entry, (uint32) table);
	}

	_ptable *table = (_ptable *) PAGE_PHYS_ADDR(e);
	uint32 *page = &table->m_entries[ PT_INDEX((uint32) virt) ];

	SET_ATTRIB(page, pte_present);
	SET_FRAME(page, (uint32)phys);
}

#endif
