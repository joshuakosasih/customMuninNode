
/*
 * File: node.c
 * - receives request data from munin-master
 * - call the corresponding plugin to fetch the data (in our case is memory)
 * - receives data resource from plugin
 * - send the data resource to munin-master
 */
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include "plugins/memory.h"

int main(int argc, char** argv) {
	
	// get hostname
	char hostname[100]; //magic number
	int result;
	result = gethostname(hostname, 100); //magic number
	if (result == 0) {
		printf("You are logged in to: %s\n", hostname);
	} else {
		printf("Error: cannot get hostname\n");	
	}
	
	/*
	 * TODO:
	 * Socket:
	 * - receives request from munin-master
	 * - sends initial response to munin-master (# munin node at hostname)
	 * - receives request command from munin-master (cap, fetch, dll)
	 */
	
	

	// assume the command is taken from argv
	char* config_result[12];
	int_fast64_t* fetch_result;
	
	if (strcmp(argv[1], "config") == 0) {
		
		// call memory plugin to get the config data
		CONFIG_MEMORY(config_result);
		
		
		// test return value
		for (int i = 0; i < 12; i++) {
			printf("%s\n", config_result[i]);
		}
		
		
		// TODO - send the config_result through socket
		// ...
		
		
	} else if (strcmp(argv[1], "fetch") == 0)  {
		
		// call memory plugin to get the fetch data
		fetch_result = FETCH_MEMORY();
		
		
		// test return value
		printf("total.value %" PRIdFAST64 "\n", fetch_result[0]);
		printf("used.value %" PRIdFAST64 "\n", fetch_result[1]);
		printf("free.value %" PRIdFAST64 "\n", fetch_result[2]);


		// TODO - send the fetch_result through socket
		// ...
		
		
	}
	
	return 0;
}

