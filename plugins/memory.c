
/*
 * File: memory.c
 * Implementation for fetching the memory information
 */

#define _GNU_SOURCE

#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>


#include <errno.h>


//#include "memory.h"

// lokasi informasi memory
#define MEMORY_INFO "/proc/meminfo"


void fail(const char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
}


//int_fast64_t FETCH_MEMORY(int_fast64_t fetch[]) {
void FETCH_MEMORY(int_fast64_t fetch[]) {

	FILE* f;
	char buff[256];

	
	int_fast64_t mem_total = -1;
	int_fast64_t mem_free = -1;
	int_fast64_t mem_buffers = -1;
	int_fast64_t mem_cached = -1;
	int_fast64_t mem_slab = -1;
	int_fast64_t mem_page_tables = -1;
	int_fast64_t mem_swap_cached = -1;
	
	//int_fast64_t fetch[3];
		
	int error = 0;
	
	if (!(f = fopen(MEMORY_INFO, "r"))) {
		error = 1;
		fail("cannot open " MEMORY_INFO);
	}
	
	if (error == 0) {


		// DEBUG
		//printf("ERROR = 0\n");
		////////

		
		char key[256];
		int_fast64_t value;
		
		while (fgets(buff, 256, f)) {
		
			if (!sscanf(buff, "%s %" SCNdFAST64, key, &value)) {
			
				error = 1;
				//fclose(f);
				fail("cannot parse " MEMORY_INFO " line");
			}

			if (error == 0) {

				
				// DEBUG
				//printf("ERROR 0 - %s - %s - value - %" PRIdFAST64 "\n", key, kb, value);
				////////


				if(!strcmp(key, "MemTotal:")) {
					mem_total = value * 1024;
				} else if(!strcmp(key, "MemFree:")) {
					mem_free = value * 1024;
				} else if(!strcmp(key, "Buffers:")) {
					mem_buffers = value * 1024;
				} else if(!strcmp(key, "Cached:")) {
					mem_cached = value * 1024;
				} else if(!strcmp(key, "Slab:")) {
					mem_slab = value * 1024;
				} else if(!strcmp(key, "PageTables:")) {
					mem_page_tables = value * 1024;
				} else if(!strcmp(key, "SwapCached:")) {
					mem_swap_cached = value * 1024;
				}
				
			} else {
				
				// error: cannot parse file
				break;
				
			}
			
		}
		
		
		/*
		if (fclose(f) == EOF) {
			// Handle the error!
			fprintf(stderr, "Error closing input file. and errno = %d, and error = %s\n", errno, strerror(errno));
			exit(1);
		} else {
			printf("FCLOSE IS OK\n");
		}
		*/
		
		fclose (f);
		
		
		printf("\n\n-----------------------------\n");
		printf("INSIDE memory plugin\n");
		printf("-----------------------------\n");
		
		printf("total.value %" PRIdFAST64 "\n", mem_total);
		printf("used.value %" PRIdFAST64 "\n", mem_total - mem_free - mem_buffers - mem_cached - mem_slab - mem_page_tables - mem_swap_cached);
		printf("free.value %" PRIdFAST64 "\n", mem_free);
		printf("buffers.value %" PRIdFAST64 "\n", mem_buffers);
		printf("cached.value %" PRIdFAST64 "\n", mem_cached);
		printf("slab.value %" PRIdFAST64 "\n", mem_slab);
		printf("page_tables.value %" PRIdFAST64 "\n", mem_page_tables);
		printf("swap_cached.value %" PRIdFAST64 "\n", mem_swap_cached);
		
		printf("-----------------------------\n\n");
		
		if(mem_total < 0 || mem_free < 0 || mem_buffers < 0 || mem_cached < 0 || mem_slab < 0 || mem_page_tables < 0 || mem_swap_cached < 0) {
			fail("missing fileds in " MEMORY_INFO);
		}
	
	}
	
	
	fetch[0] = mem_total;
	fetch[1] = mem_total - mem_free - mem_buffers - mem_cached - mem_slab - mem_page_tables - mem_swap_cached;
	fetch[2] = mem_free;
	
	
	//return fetch;
	
}

void CONFIG_MEMORY(char* config_result[]) {
	
	int_fast64_t MemInfo[3];
	char *config_zero;
	
	// get total memory	
	FETCH_MEMORY(MemInfo);

	// convert total memory into string
	asprintf(&config_zero, "%s%ld", "graph_args --base 1024 -l 0 --upper-limit ", MemInfo[0]);
	config_result[0] = config_zero;
	config_result[1] = "graph_vlabel Bytes";
	config_result[2] = "graph_title Memory usage";
	config_result[3] = "graph_category system";
	config_result[4] = "graph_info This graph shows this machine memory.";
	config_result[5] = "graph_order used free";
	config_result[6] = "used.label used";
	config_result[7] = "used.draw STACK";
	config_result[8] = "used.info Used memory.";
	config_result[9] = "free.label free";
	config_result[10] = "free.draw STACK";
	config_result[11] = "free.info Free memory.";
	
}

