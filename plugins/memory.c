
/*
 * File: memory.c
 * Implementation for fetching the memory information
 */

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
		
		
		printf("INSIDE memory\n");
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
	
	
	//return fetch;
	
}

void CONFIG_MEMORY(char* config_result[]) {
	
	int_fast64_t MemInfo[3];
	
	
	//MemInfo = FETCH_MEMORY();
	FETCH_MEMORY(MemInfo);

	
	// convert total memory into string
	/*
	char buffer[256];
	snprintf(buffer, 256, "%ld", MemInfo[0]);
	
	config_result[0] = strdup("graph_args --base 1024 -l 0 --upper-limit ");
	strcat(config_result[0],  buffer);
	*/
	
	char buffer[256];
	sprintf(buffer, "%s%ld", "graph_args --base 1024 -l 0 --upper-limit ", MemInfo[0]);
	//sprintf(buffer, "%s%%s", buffer, "\n");
	
	config_result[0] = buffer;	
	config_result[1] = "\ngraph_vlabel Bytes\n";
	config_result[2] = "graph_title Memory usage\n";
	config_result[3] = "graph_category system\n";
	config_result[4] = "graph_info This graph shows this machine memory.\n";
	config_result[5] = "graph_order used free\n";
	config_result[6] = "used.label used\n";
	config_result[7] = "used.draw STACK\n";
	config_result[8] = "used.info Used memory.\n";
	config_result[9] = "free.label free\n";
	config_result[10] = "free.draw STACK\n";
	config_result[11] = "free.info Free memory\n.";
	
}

