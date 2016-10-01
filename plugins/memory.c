
/*
 * File: memory.c
 * Implementation for fetching the memory information
 */

#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include "memory.h"

// lokasi informasi memory
#define MEMORY_INFO "/proc/meminfo"


void fail(const char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
}

void CONFIG_MEMORY(char* config_result[]) {
		
	config_result[0] = "graph_args --base 1024 -l 0 --upper-limit 8271892480";
	config_result[1] = "graph_vlabel Bytes";
	config_result[2] = "graph_title Memory usage";
	config_result[3] = "graph_category system";
	config_result[4] = "graph_info This graph shows this machinery memory.";
	config_result[5] = "graph_order used free";
	config_result[6] = "used.label used";
	config_result[7] = "used.draw STACK";
	config_result[8] = "used.info Used memory.";
	config_result[9] = "free.label free";
	config_result[10] = "free.draw STACK";
	config_result[11] = "free.info Free memory";
	
}

int_fast64_t* FETCH_MEMORY() {
		
	FILE* f;
	char buff[256];
	
	int_fast64_t mem_total = -1;
	int_fast64_t mem_free = -1;
	
	int_fast64_t* fetch;
		
	int error = 0;
	
	if (!(f = fopen(MEMORY_INFO, "r"))) {
		error = 1;
		fail("cannot open " MEMORY_INFO);
	}
	
	if (error == 0) {

		while (fgets(buff, 256, f)) {
			
			char key[256];
			int_fast64_t value;
			
			if (!sscanf(buff, "%s %" SCNdFAST64, key, &value)) {
				error = 1;
				fclose(f);
				fail("cannot parse " MEMORY_INFO " line");
			}

			if (error == 0) {

				if(!strcmp(key, "MemTotal:")) {
					mem_total = value * 1024;
				} else if(!strcmp(key, "MemFree:")) {
					mem_free = value * 1024;
				} 
				
			} else {
				
				// error: cannot parse file
				break;
				
			}
			
		}
		
		fclose(f);
		
		
		printf("INSIDE\n");
		printf("total.value %" PRIdFAST64 "\n", mem_total);
		printf("used.value %" PRIdFAST64 "\n", mem_total - mem_free);
		printf("free.value %" PRIdFAST64 "\n", mem_free);
		printf("END OF INSIDE\n");
		
		
		if(mem_total < 0 || mem_free < 0) {
			fail("missing fileds in " MEMORY_INFO);
		}
	
	}
	
	fetch[0] = mem_total;
	fetch[1] = mem_total - mem_free;
	fetch[2] = mem_free;
		
	return fetch;
	
}

