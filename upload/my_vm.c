#include "my_vm.h"
page *physicalMemory;
int pageNum;
char *physicalBitmap; 
char *virtualBitmap;
int startOfPhysicalMemory;
/*
Function responsible for allocating and setting your physical memory 
*/
void set_physical_mem() {

    //Allocate physical memory using mmap or malloc; this is the total size of
    //your memory you are simulating

    
    //HINT: Also calculate the number of physical and virtual pages and allocate
    //virtual and physical bitmaps and initialize them
    pageNum = MAX_MEMSIZE/PGSIZE;
    //printf("%d\n",pageNum);
    physicalMemory = (page *)malloc(pageNum*(sizeof(page)));
    physicalBitmap = (char *) malloc(pageNum/8);
    virtualBitmap = (char *) malloc(pageNum/8);
    memset(physicalBitmap,0,4);
    memset(virtualBitmap,0,4);
    //printf("It actually got here!\n");
    physicalMemory[0].pageEntries[0] = &physicalMemory[1];
    set_bit_at_index(physicalBitmap,0,1);
    set_bit_at_index(physicalBitmap,1,1);
    set_bit_at_index(virtualBitmap,0,1);
}


/*
 * Part 2: Add a virtual to physical page translation to the TLB.
 * Feel free to extend the function arguments or return type.
 */
int
add_TLB(void *va, void *pa)
{

    /*Part 2 HINT: Add a virtual to physical page translation to the TLB */

    return -1;
}


/*
 * Part 2: Check TLB for a valid translation.
 * Returns the physical page address.
 * Feel free to extend this function and change the return type.
 */
pte_t *
check_TLB(void *va) {

    /* Part 2: TLB lookup code here */

}


/*
 * Part 2: Print TLB miss rate.
 * Feel free to extend the function arguments or return type.
 */
void
print_TLB_missrate()
{
    double miss_rate = 0;	

    /*Part 2 Code here to calculate and print the TLB miss rate*/




    fprintf(stderr, "TLB miss rate %lf \n", miss_rate);
}



/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
pte_t *translate(pde_t *pgdir, void *va) {
    /* Part 1 HINT: Get the Page directory index (1st level) Then get the
    * 2nd-level-page table index using the virtual address.  Using the page
    * directory index and page table index get the physical address.
    *
    * Part 2 HINT: Check the TLB before performing the translation. If
    * translation exists, then you can return physical address from the TLB.
    */

    //part 1 implementaion
    /*unsigned long topBits = get_top_bits(*pgdir,10);
    unsigned long address = (physicalMemory[0].pageEntries[topBits]);
    unsigned long midBits = *va;
    midBits = get_mid_bits(*va,10,12);
    unsigned long actual = (physicalMemory[address].pageEntries[midBits]); */
    int offsetBits = (int)(log10(PGSIZE)/log10(2));
    int virtualBits = (int)((32 -offsetBits)/2);
    unsigned int virtAdd = (unsigned int) va;
    unsigned long topBits = get_top_bits(virtAdd,virtualBits);
    unsigned long *address = pgdir[topBits];
    unsigned long midBits = get_mid_bits(virtAdd,virtualBits,offsetBits);
    pte_t *ret = address[midBits];
    unsigned long lowerBits = get_bottom_bits(virtAdd,offsetBits);
    ret = ret+lowerBits;
    return ret;
    //If translation not successfull
    //return NULL; 
}


/*
The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added
*/
int
page_map(pde_t *pgdir, void *va, void *pa)
{

    /*HINT: Similar to translate(), find the page directory (1st level)
    and page table (2nd-level) indices. If no mapping exists, set the
    virtual to physical mapping */
    /*unsigned int virtAdd = *(unsigned int *) va;
    unsigned long topBits = get_top_bits(virtAdd,10);
    unsigned long address = *(pgdir + topBits);
    unsigned long midBits = get_mid_bits(virtAdd,10,12);
    pte_t *ret = (pte_t *)(address + midBits);
    unsigned long lowerBits = get_bottom_bits(virtAdd,10);
    if((pte_t *)(*ret + lowerBits) == NULL){
        ret = (pte_t *) (*ret + lowerBits);
        ret = (unsigned long *) pa;
        return 1;
    }
    else{
        return -1;
    }
    */
    unsigned int virtAdd = *(unsigned int *) va;
    unsigned long topBits = get_top_bits(virtAdd,10);
    if(physicalMemory[0].pageEntries[topBits]==0){
        for(int i = 0; i < pageNum; i ++){
            if(get_bit_at_index(physicalBitmap,i)==0){
                physicalMemory[0].pageEntries[topBits] = &physicalMemory[i];
            }
        }
    }
    unsigned long *address = physicalMemory[0].pageEntries[topBits];
    
    unsigned long midBits = get_mid_bits(virtAdd,10,12);
    pte_t *ret = &address[midBits];
    if(*ret == 0){
        *ret = (unsigned long *) pa;
        return 1;
    }
    else{
        return -1;
    }
}


/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {
    void * ret;
    int counter = 0;
    char * tempMap;
    tempMap = virtualBitmap;
    bool space = false;
    unsigned long * pa = 0;
    unsigned long  va = 0;
    //Calculates offset and virtual bits as pagesize may varry.
    int offsetBits = (int)(log10(PGSIZE)/log10(2));
    int virtualBits = (int)((32 -offsetBits)/2);
    //printf("\nOffset Bits: %d\n",offsetBits);
    //Use virtual address bitmap to find the next free page
    
    for(int i = 0; i < pageNum; i++){
        
        if(get_bit_at_index(tempMap, i)==0){
            counter++;
            if(counter == num_pages){
                //we have found an open page table entry now to convert to a virtual address
                //first lets determine which page directory it belongs too.
                unsigned long pageDir;//integer division helps us as it will give us the correct pageDir entry
                unsigned long pageTabEl;//The mod gets us its element in that page table.
                space = true;
                //printf("\nVirtual Page Number: %d is free\n",i);
                //va will have PageDir added to it and left shifted by virtualBits
                int x =0;
                for(int j = num_pages-1; j >= 0; j--){ 
                    x = i-j;
                    pageDir = x/(PGSIZE/(sizeof(unsigned long)));
                    pageTabEl = x % (PGSIZE/(sizeof(unsigned long)));
                    for(int h = 0; h < pageNum; h++){
                        tempMap = physicalBitmap;
                        if(get_bit_at_index(tempMap, h)==0){            
                            //Now to figure out its location in memeory.
                            //This can be done by taking the i value and adding to the known virtual page offset.
                            pa = (pte_t *)&physicalMemory[h];
                            set_bit_at_index(physicalBitmap,h,1);
                            //printf("\nPhysical Page Number: %d is free\n",i);
                            //Bit map is set to one at current and previous indexes to represent it is now full.
                            break;
                        }
                    }
                    va = pageDir;
                    va = va << virtualBits;
                    va = va + pageTabEl;
                    va = va << offsetBits;
                    if(j == num_pages-1){
                        ret = va;
                    }
                    //Bit map is set to one at current index to represent it is now full.
                    set_bit_at_index(virtualBitmap, i,1);
                    page_map((unsigned long *)&physicalMemory[0],(unsigned long *)&va,pa);

                //Now we have to find a space in physical address for it.
                }
                break;
        }
        else{
           counter = 0; 
        }
        }
    }
    if(!space){
        //No space remains. Return null
        return NULL;
    }
    //Now to find a continguous space in physical memeory   
   else{
        page_map((pte_t *)&physicalMemory[0],&va,pa);
        //void * temp = &va;
        return(ret);
   }
}


/* Function responsible for allocating pages
and used by the benchmark
*/
void *a_malloc(unsigned int num_bytes) {

    /* 
     * HINT: If the physical memory is not yet initialized, then allocate and initialize.
     */

   /* 
    * HINT: If the page directory is not initialized, then initialize the
    * page directory. Next, using get_next_avail(), check if there are free pages. If
    * free pages are available, set the bitmaps and map a new page. Note, you will 
    * have to mark which physical pages are used. 
    */
   // printf("So it didn't get here?!\n");
    if(physicalMemory == NULL){
        //sets physical memeory and creates page directory
        set_physical_mem();
        //set_page_directory();
    }
    int numPages;
    if (num_bytes % PGSIZE == 0){
        numPages = num_bytes/PGSIZE;
    } 
    else{
        numPages = (num_bytes/PGSIZE) + 1;
    }
    //printf("\nNeed to allocate %d page(s)", numPages);
    return(get_next_avail(numPages));
}

/* Responsible for releasing one or more memory pages using virtual address (va)
*/
void a_free(void *va, int size) {

    /* Part 1: Free the page table entries starting from this virtual address
     * (va). Also mark the pages free in the bitmap. Perform free only if the 
     * memory from "va" to va+size is valid.
     *
     * Part 2: Also, remove the translation from the TLB
     */
    int offsetBits = (int)(log10(PGSIZE)/log10(2));
    int virtualBits = (int)((32 -offsetBits)/2);
    int num_pages;
    if(size == PGSIZE){
        num_pages = size/PGSIZE;
    }
    else{
        num_pages = size/PGSIZE;
        num_pages++;
    }
    pte_t * phys;
    int index=0;
    int midBits=0;
    int topBits =0;
    unsigned long *address;
    for(int i = 0; i < num_pages; i++){
        //sets the appropriate physical page to 0
        phys = translate((pte_t *)&physicalMemory[0],va);
        index = ((*(unsigned long *) phys)/PGSIZE);
        set_bit_at_index(physicalBitmap,index,0);
        //finds the address of the virtual page to set it to point to 0
        midBits = get_mid_bits(va,virtualBits,offsetBits);
        topBits = get_top_bits(va,virtualBits);
        address = physicalMemory[0].pageEntries[topBits];
        pte_t *ret = &address[midBits];
        *ret = 0;
        set_bit_at_index(virtualBitmap,midBits,0);

    }
    
}


/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
*/
void put_value(void *va, void *val, int size) {

    /* HINT: Using the virtual address and translate(), find the physical page. Copy
     * the contents of "val" to a physical page. NOTE: The "size" value can be larger 
     * than one page. Therefore, you may have to find multiple pages using translate()
     * function.
     */
    pte_t * address = translate((pte_t *)&physicalMemory[0],va);
    //*address = *(unsigned long *)val;
    //memcpy dest sourse size.
    memcpy((void *)address, val, size);
    //printf("Placed value: %ld\n",*address);
    //memsize() USE IT


}


/*Given a virtual address, this function copies the contents of the page to val*/
void get_value(void *va, void *val, int size) {

    /* HINT: put the values pointed to by "va" inside the physical memory at given
    * "val" address. Assume you can access "val" directly by derefencing them.
    */

    //Works in theory have to test with put.
    /*pte_t * address = translate((pte_t *)&physicalMemory[0],va);
    unsigned long ret = 0;
    for(int i = 0; i < size; i++){
        ret= ret + *(address + i);
    }
    *(unsigned long *)val = ret;
    */
    pte_t * address = translate((pte_t *)&physicalMemory[0],va);
    //printf("Retriving: %ld\n", *address);
    //*(unsigned long *) val = *address;
    //char * ret = 0;
    //for(int i =0; i < size; i++){
     //   ret = ret + *(address + i);
    //}
    memcpy(val, address, size);
}



/*
This function receives two matrices mat1 and mat2 as an argument with size
argument representing the number of rows and columns. After performing matrix
multiplication, copy the result to answer.
*/
void mat_mult(void *mat1, void *mat2, int size, void *answer) {

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
     * matrix accessed. Similar to the code in test.c, you will use get_value() to
     * load each element and perform multiplication. Take a look at test.c! In addition to 
     * getting the values from two matrices, you will perform multiplication and 
     * store the result to the "answer array"
     */

       
}
unsigned int get_top_bits(unsigned int value,  int num_bits)
{
    int num_bits_to_prune = 32 - num_bits;
    return (value >> num_bits_to_prune);
}
unsigned int get_bottom_bits(unsigned int value,  int num_bits)
{
    int num_bits_to_prune = 32 - num_bits;
    unsigned long ret = (value << num_bits_to_prune);
    ret = (ret >> num_bits_to_prune);
//    twelve_bits = 0xfff;
//    get_bottom_bits = value & twelve_bits;
    return ret;
}
unsigned int get_mid_bits (unsigned int value, int num_middle_bits, int num_lower_bits)
{
   unsigned int mid_bits_value = 0;   
   value = value >> num_lower_bits; 
   unsigned int outer_bits_mask =   (1 << num_middle_bits);
   outer_bits_mask = outer_bits_mask-1;
   mid_bits_value =  value &  outer_bits_mask;
  return mid_bits_value;
}
int get_bit_at_index(char *bitmap, int index)
{
    //Same as example 3, get to the location in the character bitmap array
    char *region = ((char *) bitmap) + (index / 8);
    
    //Create a value mask that we are going to check if bit is set or not
    char bit = 1 << (index % 8);
    
    return (int)(*region >> (index % 8)) & 0x1;
}

void set_page_directory(){
    int counter = 0;
    for(int i = 0; i < sizeof(physicalMemory[0].pageEntries)/sizeof(*physicalMemory[0].pageEntries); i++){
        physicalMemory[0].pageEntries[i] = (pte_t) &physicalMemory[i+1];
        //printf("%lu\n",physicalMemory[0].pageEntries[i]);
        counter++;
    }
    startOfPhysicalMemory = counter+1;
    //printf("No Segfault?!\n");
}

void set_bit_at_index(char *bitmap, int index, int type)
{
    // We first find the location in the bitmap array where we want to set a bit
    // Because each character can store 8 bits, using the "index", we find which 
    // location in the character array should we set the bit to.
    char *region = ((char *) bitmap) + (index / 8);
    
    // Now, we cannot just write one bit, but we can only write one character. 
    // So, when we set the bit, we should not distrub other bits. 
    // So, we create a mask and OR with existing values
    char bit = 1 << (index % 8);

    // just set the bit to 1. NOTE: If we want to free a bit (*bitmap_region &= ~bit;)
    if(type == 1){
        *region |= bit;
    }
    else{
        *region &= ~bit;
    }
}
/*
int main(){
    //unsigned long * out;
   // out = (unsigned long *)a_malloc(10);
   // printf("First Virtual Pointer: %ld\n", *out);
   // out = (unsigned long *)a_malloc(12289);
   // printf("Second Virtual Pointer: %ld\n", *out);
   // out = (unsigned long *)a_malloc(4098);
   // printf("Third Virtual Pointer: %ld\n", *out);
    
    int ARRAY_SIZE = 400;
    void *a = a_malloc(ARRAY_SIZE);
    int old_a = (int)a;
    void *b = a_malloc(ARRAY_SIZE);
    void *c = a_malloc(ARRAY_SIZE);
    printf("Addresses of the allocations: %x, %x, %x\n", (int)a, (int)b, (int)c);
}
*/


