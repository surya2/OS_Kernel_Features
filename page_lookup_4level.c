//
// Created by Surya Selvam on 2/13/2023.
//

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "mlpt.h"

size_t ptbr = 0;

unsigned int countBits(unsigned int n) {
    unsigned int count = 0;
    while (n) {
        count++;
        n = n >> 1;
    }
    return count;
}

size_t translate(size_t virtual_addr) {
    if (ptbr == 0) {
        return 0xFFFFFFFFFFFFFFFF;
    }
    size_t page_offset = virtual_addr & ((1 << POBITS) - 1);

    /* --Below is getting the VPN parts for each level from the most significant bit-- */
    size_t vpn_parts[LEVELS];
    size_t virtual_page_num = (virtual_addr >> POBITS); size_t vpn_copy = virtual_page_num;
    size_t vpn_size = POBITS - 3;  //2^3 = 8 bytes
    int num_bits_in_vpn = countBits(virtual_page_num); int num_bits_existing = num_bits_in_vpn;
    size_t vpn_parts_mask = ((1 << vpn_size)-1) << (num_bits_existing - vpn_size);
    size_t vpn_masked;
    size_t vpn_part;
    for(int i = 0; i<LEVELS; i++){
        vpn_masked = vpn_copy & vpn_parts_mask;
        num_bits_existing -= vpn_size;
        vpn_part = (vpn_masked >> num_bits_existing) & ((1 << vpn_size)-1);

        //vpn_part = vpn_copy & vpn_parts_mask;
        vpn_parts[i] = vpn_part;
        //vpn_copy = vpn_copy << vpn_size;
        //num_bits_existing -= vpn_size;
        vpn_parts_mask = ((1 << vpn_size)-1) << (num_bits_existing - vpn_size);
    }

    size_t current_vpn;
    size_t base_address = ptbr;
    size_t *table_entry_addr;
    size_t page_table_entry;
    int valid_bit;
    size_t phys_page_num;
    size_t phys_addr;
    size_t offset;
    for(int i = 0; i<LEVELS; i++){
        current_vpn = vpn_parts[i];
        table_entry_addr = (current_vpn * 8) + base_address;
        page_table_entry = *table_entry_addr;

        valid_bit = page_table_entry & 0x1;
        if (valid_bit) {
            phys_page_num = page_table_entry >> POBITS;
            base_address = (phys_page_num << POBITS);
            /* Last revision*/

            if(i == LEVELS-1){
                phys_addr = base_address+page_offset;
                return phys_addr;
            }
        }
        else {
            return 0xFFFFFFFFFFFFFFFF;
        }
    }


    /*size_t vir_page_num = (virtual_addr >> POBITS) & ((1 << (POBITS - 3)) - 1);

    size_t *table_entry_addr = (vir_page_num * 8) + ptbr;  //Translation by multiplying by page table entry size and adding by base register for page table (ptbr which was stored earlier in allocation)
    size_t page_table_entry = *table_entry_addr;  // Accesses the page pointer we stored in the page table entry previously

    int valid_bit = page_table_entry & 1;
    if (valid_bit) {
        size_t phys_page_num = page_table_entry >> POBITS;
        size_t phys_addr = (phys_page_num << POBITS) + page_offset;


        return phys_addr;
    }
    else {
        return 0xFFFFFFFFFFFFFFFF;
    }*/
}

void page_allocate(size_t virtual_addr) {
    if (ptbr == 0) {
        int this_val = posix_memalign((void **) &ptbr, 4096, 1 << POBITS);
        memset((size_t *)ptbr, 0, 1 << POBITS);

        if (this_val!=0) {
            printf("Error occurred with status code: %d", this_val);
            return;
        }
    }

    /* --Below is getting the VPN parts for each level from the most significant bit-- */
    size_t vpn_parts[LEVELS];
    size_t virtual_page_num = (virtual_addr >> POBITS); size_t vpn_copy = virtual_page_num;
    size_t vpn_size = POBITS - 3;  //2^3 = 8 bytes
    int num_bits_in_vpn = countBits(virtual_page_num);
    int multiple_bits_for_roundup;
    int num_bits_existing;
    if(num_bits_in_vpn % 4 != 0){
        multiple_bits_for_roundup = (num_bits_in_vpn + 3)/4;
        num_bits_existing = multiple_bits_for_roundup * 4;
    }
    else{
        num_bits_existing = num_bits_in_vpn;
    }
    //int num_bits_existing = num_bits_in_vpn;
    size_t vpn_parts_mask = ((1 << vpn_size)-1) << (num_bits_existing - vpn_size);
    size_t vpn_masked;
    size_t vpn_part;
    for(int i = 0; i<LEVELS; i++){
        vpn_masked = vpn_copy & vpn_parts_mask;
        num_bits_existing -= vpn_size;
        vpn_part = (vpn_masked >> num_bits_existing) & ((1 << vpn_size)-1);

        vpn_parts[i] = vpn_part;
        //vpn_copy = vpn_copy << vpn_size;
        //num_bits_existing -= vpn_size;
        vpn_parts_mask = ((1 << vpn_size)-1) << (num_bits_existing - vpn_size);
    }

    //size_t virtual_page_num = (virtual_addr >> POBITS) & ((1 << (POBITS - 3)) - 1); //1 unisgned
    size_t current_vpn;
    size_t base_addr = ptbr;
    size_t *pte_ptr;
    int valid_bit;
    size_t new_page_ptr;
    size_t phys_page_num;
    for(int i = 0; i<LEVELS; i++){
        current_vpn = vpn_parts[i];
        pte_ptr = base_addr + (current_vpn * 8);

        valid_bit = (*pte_ptr) & 0x1;
        if (valid_bit == 1) {
            //printf("Memory is already allocated for this virtual address\n");
            return;
        }

        //size_t new_page_ptr;
        posix_memalign((void **) &new_page_ptr, 4096, 1 << POBITS); //1ull << POBITS

        if(new_page_ptr % 0x1 == 0){
            *pte_ptr = new_page_ptr+1;
        }
        else{
            *pte_ptr = new_page_ptr;
        }
        //printf("pte pointer: %d", *pte_ptr);

        phys_page_num = (new_page_ptr >> POBITS);  //<--- is this correct???????????????
        base_addr = phys_page_num << POBITS;  //OR  base_addr = phys_page_num << POBITS; //<--- multiplying by page size (4096) is the same as multiplying by 0x1000

        base_addr = new_page_ptr;  //Think the above one is correct

        printf("Level %d Page Table Entry pointer's address: %zx \n", i, *pte_ptr);
    }

    /*size_t *pte_ptr = ptbr + (virtual_page_num * 8);

    int valid_bit = (*pte_ptr) & 0x1;
    if (valid_bit == 1) {
        //printf("Memory is already allocated for this virtual address\n");
        return;
    }

    size_t new_page_ptr;
    posix_memalign((void **) &new_page_ptr, 4096, 1 << POBITS); //1ull << POBITS

    *pte_ptr = new_page_ptr+1;
    //printf("pte pointer: %d", *pte_ptr);

    printf("Page Table Entry pointer's address: %zx \n", *pte_ptr);*/
}

int main() {
    // 0 pages have been allocated
    assert(ptbr == 0);

    page_allocate(0x456789abcdef);
    // 5 pages have been allocated: 4 page tables and 1 data
    assert(ptbr != 0);

    page_allocate(0x456789abcd00);
    // no new pages allocated (still 5)
    
    int *p1 = (int *)translate(0x456789abcd00);
    printf("%zx\n", p1);
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