#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "pecoff.h"
#include "print.h"


/* Upper align */
#define UPPER_ALIGN(value, align) (((value) % (align)) ? \
	(((value)/(align))*(align) + (align)) : (value))

/* Error buffer */
static char errbuf[2048];


/*
 * Return last error as a string
 */
const char *
pecoff_error(void)
{
	return(errbuf);
}


/*
 * Find the offset to the end of the PE-COFF binary.
 * Returns the offset on success, -1 on error.
 */
int64_t
pecoff_offset(const char *file)
{
	HANDLE thisfile;
	HANDLE map;
	char *mapaddr;
	int64_t file_size;
	struct pe_hdr *pe;
	struct section_hdr *section;
	unsigned int pe_offset;
	unsigned int big_section;
	unsigned int last_section;
	unsigned int alignment;
	u_int i;

	file_size = 0;
	pe_offset = 0;
	last_section = 0;

	/* Open target */
	if ( (thisfile = CreateFile(file, GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		snprintf(errbuf, sizeof(errbuf), "CreateFile failed: %s", 
			winerror());
		return(-1);
	}

	if ( (map = CreateFileMapping(thisfile, NULL,
			PAGE_READONLY, 0, 0, NULL)) == NULL) {
		snprintf(errbuf, sizeof(errbuf), "CreateFileMapping failed: %s", 
			winerror());
		return(-1);
	}

	if ( (mapaddr = MapViewOfFile(map,
			FILE_MAP_READ, 0, 0, 0)) == NULL) {
		snprintf(errbuf, sizeof(errbuf), "MapViewOfFile failed: %s", 
			winerror());
		return(-1);
	}


	/* Make sure this is a PE-COFF file, check for 'MZ' */
	if (*((short *)mapaddr) != 0x5a4d) {
		snprintf(errbuf, sizeof(errbuf), "Not a PECOFF file");
		return(-1);
	}

	/* Get PE header offset from index 0x3c */
	pe_offset = *((unsigned int *)((char *)mapaddr + 0x3c));
	
	/* Construct PE header */
	pe = (struct pe_hdr *)(mapaddr + pe_offset);
	if (memcmp(pe->signature, "PE\x00\x00", 4)) {
		snprintf(errbuf, sizeof(errbuf), "Target file misses PE tag");
		return(-1);
	}

	section = (struct section_hdr *)((char *)pe + 
		sizeof(struct pe_hdr) + pe->sizeof_optional_header);
	
	/* Loop through all sections to find the one at the end, the one
	 * with the largest pointer to raw data */
	for (i=0, big_section = 0; i < pe->number_of_sections; i++) {

		if (section[i].pointer_to_raw_data >= big_section) {
			big_section = (unsigned int)section[i].pointer_to_raw_data;
			last_section = i;
		}
	}

#if 0
    /* sizeof_raw_data might be larger in some situations */
    if (section[last_section].sizeof_raw_data > section[last_section].virtual_size)
        file_size += section[last_section].sizeof_raw_data;
    else
        file_size += section[last_section].virtual_size;
#else
    file_size += section[last_section].pointer_to_raw_data;
    file_size += section[last_section].sizeof_raw_data;

#endif

	if (pe->sizeof_optional_header > 0) {
		/* Fetch alignment from optional header (at offset 0x38 from PE) */
		alignment = *((unsigned int *)((char *)pe + 0x3C));
		file_size = UPPER_ALIGN(file_size, alignment);
	}

	if (pe->pointer_to_symbol_table >= file_size) {
		file_size = pe->pointer_to_symbol_table;
		file_size += pe->number_of_symbols * 18;
		file_size += *(unsigned long *) (mapaddr + file_size);
	}

	/* We are finished, unmap and close the file */
	UnmapViewOfFile(mapaddr);
	CloseHandle(thisfile);
	return(file_size);
}

#ifdef TEST

int main(int argc, char **argv)
{
	int64_t n;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}

	n = pecoff_offset(argv[1]);

	if (n == -1)
		fprintf(stderr, "%s\n", pecoff_error());
	else
		printf("%llu\n", pecoff_offset(argv[1]));

	return 0;
}

#endif
