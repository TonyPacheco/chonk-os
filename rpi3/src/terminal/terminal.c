#include "terminal.h"
#include "known_cmds.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

char* buffer;
int bpos;
cmds commands;

void trm_init(){
    bpos = 0;
    commands.n[0] = "ls";
    commands.n[1] = "sysinfo";
    commands.n[2] = "cd";
    commands.p[0] = ls;
    commands.p[1] = sysinfo;
    commands.p[2] = cd;
}

int trm_capture(char ch){
    switch(ch){
    case '\r':
    case '\n':
    {
        printf("\n");
        int cmd_return_val = -1;
        int cmd_index = trm_parse_buffer();
        if(cmd_index != -1)
            cmd_return_val = trm_exec_run(cmd_index);

        buffer_clear();
        return cmd_return_val;
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

int trm_parse_buffer() { //return index of command to call, or -1 if not found
    for(int i = 0; i < NUM_CMDS; ++i)
        if(!strcmp(buffer, commands.n[i])){
            return i;
        }
    return -1;
}

char* buffer_out(){
    return buffer;
}

void buffer_add_c(char c){
    buffer[bpos++] = c;
    buffer[bpos] = '\0';
}

void buffer_clear(){
    bpos = 0;
    buffer[0] = '\0';
}

int trm_exec_run (int i){
	//call the appropriate command or program 
	//and return the response code of it
    commands.p[i]();
	return 0;
}
