#ifndef MY_VM_H_INCLUDED
#define MY_VM_H_INCLUDED
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

//Assume the address space is 32 bits, so the max memory size is 4GB
//Page size is 4KB

//Add any important includes here which you may need

#define PGSIZE 4096

// Maximum size of virtual memory
#define MAX_MEMSIZE 4ULL*1024*1024*1024

// Size of "physcial memory"
#define MEMSIZE 1024*1024*1024

// Represents a page table entry
typedef unsigned long pte_t;

// Represents a page directory entry
typedef unsigned long pde_t;

#define TLB_ENTRIES 512

//Structure to represents TLB
struct tlb {
    /*Assume your TLB is a direct mapped TLB with number of entries as TLB_ENTRIES
    * Think about the size of each TLB entry that performs virtual to physical
    * address translation.
    */

};
struct tlb tlb_store;

typedef struct page{
    unsigned long pageEntries[PGSIZE/(sizeof(unsigned long))];
} page;


/*struct top_Virtual//Top level virtual address, first 10 bits
    int tag;
    struct top_Virtual *next;
    struct bottom_Virtual *level; 
}top;

struct bottom_Virtual{//Bottom level virtual address, second 10 bits
    int tag;
    struct bottom_Virtual *next;
    struct physical_Page *physical;

}bot;

struct physical_Page{//Physical page max size equal to PGSIZE or 4096 by default
    int size;
    struct physical_Page *next;//If size < PGSIZE more pages can fit within this section of memory.
}page;
*/
void set_physical_mem();
pte_t* translate(pde_t *pgdir, void *va);
int page_map(pde_t *pgdir, void *va, void* pa);
bool check_in_tlb(void *va);
void put_in_tlb(void *va, void *pa);
void *a_malloc(unsigned int num_bytes);
void a_free(void *va, int size);
void put_value(void *va, void *val, int size);
void get_value(void *va, void *val, int size);
void mat_mult(void *mat1, void *mat2, int size, void *answer);
void print_TLB_missrate();
static unsigned int get_top_bits(unsigned int value,  int num_bits);
static unsigned int get_mid_bits (unsigned int value, int num_middle_bits, int num_lower_bits);
static unsigned int get_bottom_bits(unsigned int value,  int num_bits);
static int get_bit_at_index(char *bitmap, int index);
static void set_bit_at_index(char *bitmap, int index);
void set_page_directory();

#endif
