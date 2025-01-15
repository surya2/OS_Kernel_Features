## TODO: 
Working on new feature for TLB Cache lookup and coherency shootdown as well as code refactoring for 4-level page table. Thank you for your patience!

# Multi-Level Page Table Implementation

## Overview
This project implements a multi-level page table system for virtual memory management. The implementation supports virtual-to-physical address translation and dynamic page allocation. This implementation along with TLB Cache and TLB shootdown implementations yet to come make up the MMU portion of my OS from Scratch project. It will be paired with my [*OS_Firmware_Project*](https://github.com/surya2/OS_Firmware_Project) repository which is also in its initial stages of development.

### Key Features
- Multi-level page table support (configurable number of levels)
- Dynamic page allocation
- Virtual-to-physical address translation
- Page-aligned memory management
- Configurable page size (via POBITS)

## Implementation Details

### Page Table Structure
- Uses a configurable number of page table levels (defined by `LEVELS`)
- Page size is 2^POBITS bytes (default: 12 bits = 4KB pages). Thus each page table will be formed from 4KB pages.
- Each page table entry is 8 bytes which supports 64 bit addresses
- Valid bit is stored in the least significant bit of each entry

### Key Components

1. **Page Table Base Register (PTBR)**
   - Stores the base address of the root page table
   - Initially set to 0 (null)

2. **Virtual Address Translation**
   - Breaks virtual address into VPN parts and offset
   - Walks through page table levels
   - Returns physical address or 0xFFFFFFFFFFFFFFFF if invalid

3. **Page Allocation**
   - Allocates page tables as needed
   - Uses posix_memalign for page-aligned allocation
   - Maintains valid bits for page table entries

## Usage

### Building the Project
```
bash
make
```

# Clean build files
```
make clean
```

# Build with debug symbols
```
make debug
```

# Build and run
```
make run
```

### Running Tests
```
bash
make test
```

### Code Examples

1. **Allocating a Page**
```c
// Allocate a page for virtual address
page_allocate(0x456789abcdef);
```

2. **Translating an Address**
```c
// Translate virtual to physical address
size_t phys_addr = translate(0x456789abcdef);
if (phys_addr == 0xFFFFFFFFFFFFFFFF) {
    printf("Invalid address\n");
}
```

## Configuration

### config.h
```c
#define LEVELS  4     // Number of page table levels
#define POBITS  12    // Number of page offset bits (4KB pages)
```

### Memory Requirements
- Each page table requires 2^POBITS bytes
- Total memory usage depends on number of allocated pages
- Page tables are allocated on demand

## Error Handling
- Returns 0xFFFFFFFFFFFFFFFF for invalid translations
- Memory allocation failures are handled gracefully
- All newly allocated pages are zeroed

## Technical Notes
- Assumes little-endian architecture
- Requires 64-bit system support
- Uses POSIX-compliant memory alignment

## Directory Structure
```
.
├── README.md
├── Makefile
├── main.c         # Main program and tests
├── lookup.c       # Page table implementation
├── mlpt.h         # Interface declarations
└── config.h       # Configuration constants
```

## Limitations and future TODOS
- No support for page deallocation
- No TLB implementation
- No page permissions (read/write/execute)
- Virtual address space limited by LEVELS and POBITS configuration
