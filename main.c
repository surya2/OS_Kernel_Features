//
// Created by Surya Selvam on 2/13/2023.
//
#define LEVELS  1
#define POBITS  12

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
//#include "mlpt.h"
//#include "config.h"
#include <string.h>

//size_t** memptr = 0x234234234234;

size_t ptbr = 0;

size_t translate(size_t va);

void page_allocate(size_t va);

int main(void){
    // 0 pages have been allocated
    printf("Flag 1");
    assert(ptbr == 0);

    printf("Before Seg");

    page_allocate(0x456789abcdef);

    printf("After page allocate");
    //48 bits for virtual address
    //12 bit page offset
    //36 bits for virtual page numbr
    // 36/LEVELS = bits for each page lookup
    //Physical page number - 36 bits


    // 5 pages have been allocated: 4 page tables and 1 data
    assert(ptbr != 0);

    page_allocate(0x456789abcd00);
    // no new pages allocated (still 5)

    int *p1 = (int *)translate(0x456789abcd00);
    printf("Physical Address of 0x456789abcd00: %d", p1);
    *p1 = 0xaabbccdd;
    short *p2 = (short *)translate(0x456789abcd02);
    printf("%04hx\n", *p2); // prints "aabb\n"

    assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

    page_allocate(0x456789ab0000);
    // 1 new page allocated (now 6; 4 page table, 2 data)

    assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);

    page_allocate(0x456780000000);
    // 2 new pages allocated (now 8; 5 page table, 3 data)
}

void page_allocate(size_t va){
    if(ptbr == 00) {
        posix_memalign((void **) &ptbr, 32, 1ull << POBITS);
    }
}

size_t translate(size_t va){
    size_t vpn = va >> POBITS;
    int size_of_parial_pn = (64 - POBITS)/LEVELS;

    if(vpn == NULL){
        return 0xFFFFFFFFFFFFFFFF;
    }

    if(ptbr == NULL){
        ptbr = 0;
    }
    //size_t bavpn se_addr;

    size_t *pte = (vpn * 8) + ptbr;
    size_t ppn = *pte;
    size_t pa = (ppn << POBITS) + (va & 0x000000000FFF);

    /*for(int i = 1; i<=LEVELS; i++){

    }*/

    return pa;
}

