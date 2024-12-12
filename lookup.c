//
// Created by Surya Selvam on 2/15/2023.
//
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "mlpt.h"
#define PTB_EXIST (1ull << (POBITS - 1))
#define PTB_NOT_EXIST 0
#define TABLE_SIZE (1ull << POBITS)
#define PHYSICAL_ADDRESS_MASK (TABLE_SIZE - 1)
size_t *this_level_table = NULL;
size_t ptbr = 0;

size_t translate(size_t va) {
    size_t vpn = (va >> POBITS) & ((1ull << (POBITS - 3)) - 1);
    size_t offset = va & ((1ull << POBITS) - 1);

    printf("VPN: %zx, offset: %zx\n", vpn, offset);
    // int size_of_parial_pn = (64 - POBITS)/LEVELS;  <-- for multi-level

    if (ptbr == 0) {
        return 0x3FFFFFFF;
        // return 0;
    }

    size_t *pte_addr = ()((vpn * 8) + ptbr);
    size_t pte = *pte_addr;  // Accesses the page pointer we stored in the page table entry previously
    if (pte & 0x1 != 1) {
        return 0x3FFFFFFF;
    }

    size_t ppn = pte >> POBITS;
    size_t pa = (ppn << POBITS) + offset;

    /*for(int i = 1; i<=LEVELS; i++){//multi-level lookup}*/

    return pa;
}

void page_allocate(size_t va) {
    // size_t *base_register = (size_t*) ptbr;
    // size_t level_index;
    // size_t i;

    // Allocate the base page table if it doesn't exist yet, essentially the starting point for the base table
    if (ptbr == 0) {
        int this_val = posix_memalign((void **)&ptbr, 4096, 1ull << POBITS);
        memset((size_t *)ptbr, 0, 1 << POBITS);
        printf("this_val %d \n", this_val);
        printf("Test\n");
        if (this_val != 0) {
            printf("I'm in this place\n");
            return;
        }
        printf("Test2\n");
        // this_level_table = (size_t *) ptbr;
    }

    size_t vpn = (va >> POBITS) & ((1ull << (POBITS - 3)) - 1);
    size_t *PTE_pointer = ptbr + (vpn * 8);

    size_t PTE = *PTE_pointer;

    size_t data_pointer;

    int valid_bit = PTE & 0x1;

    if (valid_bit == 1) {
        printf("Already allocated\n");
        return;
    }
    printf("WE are here\n");

    posix_memalign((void **)&data_pointer, 4096, 1ull << POBITS);
    printf("We risin!!!!!!! \n");

    // PPN = PPN_pointer;
    *PTE_pointer = data_pointer + 1;

    printf("PTE pointer has address listed to the right, take a look at it: %zx \n", *PTE_pointer);
    /*size_t addr = *page_pointer;
     *page_pointer = addr + 1;*/
    // size_t append_val = PPN + 1;
    // this_level_table[vpn*8] = append_val;
}

int main() {
    assert(ptbr == 0);

    page_allocate(0x456789abcdef);
    // 5 pages have been allocated: 4 page tables and 1 data

    assert(ptbr != 0);

    page_allocate(0x456789abcd00);

    translate(0x456789abcdef);
    translate(0x456789abcd00);

    size_t *p1 = (size_t *)translate(0x456789abcdef);
    printf("Physical address: %zx \n", translate(0x456789abcdef));
    size_t new_val = *p1;
    printf("new_val: %zx\n", new_val);

    *p1 = 0xaabbccdd;

    short *p2 = (short *) translate(0x456789abcd02);
    printf("%04hx\n", *p2); // prints "aabb\n"

    
    assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

    page_allocate(0x456789ab0000);
    // 1 new page allocated (now 6; 4 page table, 2 data)

    assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);

    page_allocate(0x456780000000);
    // 2 new pages allocated (now 8; 5 page table, 3 data)


    int *p1 = (int *)translate(0x456789abcd00);
    *p1 = 0xaabbccdd;
    short *p2 = (short *)translate(0x456789abcd02);
    printf("%04hx\n", *p2); // prints "aabb\n"

    assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

    page_allocate(0x456789ab0000);
    // 1 new page allocated (now 6; 4 page table, 2 data)

    assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);

    page_allocate(0x456780000000);
    // 2 new pages allocated (now 8; 5 page table, 3 data)
    // 2 pages have been allocated: 1 page table and 1 data
    assert(ptbr != 0);

    return 0;

}

