#pragma once
#include <stdint.h>

/**
 * Returns sizes of FLASH and RAM on SoC. Pointers may be NULL,
 * if some values are not needed.
 */
void get_memory_ranges(const char *mem_postfix,
                       uint32_t *flash_start, uint32_t *flash_size,
                       uint32_t *ram_start, uint32_t *ram_size);
