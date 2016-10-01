
/*
 * File: memory.h
 */

#ifndef MEMORY_H
#define MEMORY_H

void fail(const char *message);

void CONFIG_MEMORY(char* config_result[]);

int_fast64_t* FETCH_MEMORY();

#endif
