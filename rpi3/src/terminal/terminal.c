#include "terminal.h"
#include <stdbool.h> // Needed for bool
#include <stdint.h>
#include <string.h>
#include <stdio.h>

char* buffer;
uint8_t bpos;

void trm_init(){
    bpos = 0;
}

uint8_t trm_capture(uint8_t ch){
    switch(ch){
    case '\r':
    case '\n':
    { 
        if(trm_parse_buffer())
            return trm_exec_run();

        clear_buffer();
    } break;

    case '\t':{           //replace tabs in the buffer with spaces 
        buffer_add_c(' ');
    } break;

    default:{             //simply add ch to the end of the buffer
        buffer_add_c(ch);
    }
    }
	return 0;
}

bool trm_parse_buffer() { //return true if program requested is recognized 
	char *firstToken;
	for (uint8_t i = 0; i <= bpos; ++i){
		if(buffer[i] == ' ' || buffer[i] == '\0'){
			strncpy(firstToken, buffer, i);
			return true;
		}
	}
    return false;
}

char* buffer_out(){
    return buffer;
}

void buffer_add_c(uint8_t c){
    buffer[bpos++] = c;
    buffer[bpos] = '\0';
}

void clear_buffer(){
    bpos = 0;
    buffer[0] = '\0';
}

uint8_t trm_exec_run (){
	//call the appropriate command or program 
	//and return the response code of it
	return 1;
}
