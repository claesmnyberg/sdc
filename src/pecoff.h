#ifndef _SDC_PECOFF
#define _SDC_PECOFF

#include <stdint.h>

/* coff header */
struct pe_hdr {
    uint8_t signature[4]; /* PE\0\0 */
    uint16_t machine;
    uint16_t number_of_sections;
    uint32_t timestamp;
    uint32_t pointer_to_symbol_table;
    uint32_t number_of_symbols;
    uint16_t sizeof_optional_header;
    uint16_t characteristics;
} __attribute__((packed));


/* Section table entry */
struct section_hdr {
    uint8_t section_name[8];
    uint32_t virtual_size;
    uint32_t virtual_address;
    uint32_t sizeof_raw_data;
    uint32_t pointer_to_raw_data;
    uint32_t pointer_to_relocations;
    uint32_t pointer_to_linenumbers;
    uint16_t number_of_relocations;
    uint16_t number_of_linenumbers;
    uint32_t characteristics;
} __attribute__((packed));


/* pecoff.c */
extern const char *pecoff_error();
extern int64_t pecoff_offset(const char *);

#endif /* _SDC_PECOFF */
