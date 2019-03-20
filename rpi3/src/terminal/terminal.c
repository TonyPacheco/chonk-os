#include "terminal.h"
#include "../signal.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../drivers/sdcard/SDCard.h"
#include "../drivers/stdio/emb-stdio.h" 
#include <stdarg.h>
#include "../hal/hal.h"
#include "../loader/loader.h"

#define NUM_CMDS 8
#define WIDTH 640
#define HEIGHT 480

typedef struct
{
    char *n[NUM_CMDS];
    int (*p[NUM_CMDS])(void);
} cmd_map;

#define BUFFER_MAX 500

char buffer[BUFFER_MAX];
int bpos;
cmd_map cmds;
char* work_dir;

void trm_init(){
    bpos = 0;
    work_dir = "\\";

    cmds.n[0] = "help";
    cmds.n[1] = "sysinfo";
    cmds.n[2] = "cd";
    cmds.n[3] = "exit";
    cmds.n[4] = "here";
    cmds.n[5] = "ls";
    cmds.n[6] = "cat";
    cmds.n[7] = "run";
    cmds.n[8] = "dump";

    cmds.p[0] = help;
    cmds.p[1] = sysinfo;
    cmds.p[2] = cd;
    cmds.p[3] = exit_sh;
    cmds.p[4] = here;
    cmds.p[5] = ls;
    cmds.p[6] = cat;
    cmds.p[7] = run;
    cmds.p[8] = dump;

    trm_main();
}

void trm_main(){
    char c;
    int s = SIG_GOOD;
    printf("%s\n\n>", "Welcome to CHONK OS");
    while (s != SIG_KILL)
    {
        c = hal_io_serial_getc(SerialA);
        hal_io_serial_putc(SerialA, c);
        printf("%c", c);
        s = trm_capture(c);
    }
}

int trm_capture(char ch){
    switch(ch){
    case '\r':
    case '\n':
    {
        int cmd_return_val = SIG_STRT;
        int cmd_index = trm_parse_buffer();
        if(cmd_index == SIG_FAIL)
            printf(">");
        else {
            cmd_return_val = cmds.p[cmd_index]();
            printf(">");
        }
            
        buffer_clear();
        return cmd_return_val;
    } break;

    case '\t':{           //replace tabs in the buffer with spaces 
        buffer_add_c(' ');
    } break;

    case '\b':{
        buffer_backs();
    } break;

    default:{             //simply add ch to the end of the buffer
        buffer_add_c(ch);
    }
    }
	return SIG_GOOD;
}

int trm_parse_buffer() { //return index of command to call, or SIG_FAIL if not found
    char cm[bpos];
    int i = 0;
    for (i = 0; buffer[i] != '\0' && buffer[i] != ' '; i++){
        cm[i] = buffer[i];
    }
    for (int i = 0; i < NUM_CMDS; ++i)
        if (!strcmp(cm, cmds.n[i]))
            return i;
    return SIG_FAIL;
}

void buffer_add_c(char c){
    buffer[bpos++] = c;
    buffer[bpos] = '\0';
}

void buffer_backs(){
    if(bpos == 0) 
        return;
    buffer[--bpos] = '\0';
}

void buffer_clear(){
    bpos = 0;
    buffer[0] = '\0';
}

int ls()
{
    printf("Directory (%s): \n", work_dir);
    HANDLE fh;
    FIND_DATA find;
    char *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    char *path[strlen(work_dir) + 4];
    strcpy(path, work_dir);
    strcat(path, "*.*");

    fh = sdFindFirstFile(path, &find); // Find first file
    do
    {
        if (find.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
            printf("%s <DIR>\n", find.cFileName);
        else
            printf("%c%c%c%c%c%c%c%c.%c%c%c Size: %9lu bytes, %2d/%s/%4d, LFN: %s\n",
                   find.cAlternateFileName[0], find.cAlternateFileName[1],
                   find.cAlternateFileName[2], find.cAlternateFileName[3],
                   find.cAlternateFileName[4], find.cAlternateFileName[5],
                   find.cAlternateFileName[6], find.cAlternateFileName[7],
                   find.cAlternateFileName[8], find.cAlternateFileName[9],
                   find.cAlternateFileName[10],
                   (unsigned long)find.nFileSizeLow,
                   find.CreateDT.tm_mday, month[find.CreateDT.tm_mon],
                   find.CreateDT.tm_year + 1900,
                   find.cFileName);           // Display each entry
    } while (sdFindNextFile(fh, &find) != 0); // Loop finding next file
    sdFindClose(fh);                          // Close the serach handle
    printf("\n");                             // New line at end looks nicer
    return SIG_GOOD;
}

int sysinfo()
{
    uint32_t Buffer[5];
    printf("OS: \t\t%s", "ChonkOS v1.1 Developer Edition 2019\n"); 
    printf("PROCESSOR: %1s", ""); displaySmartStart(&printf);
    printf("RESOLUTION: %3dx%d \n", WIDTH, HEIGHT); 
    printf("CPU FREQ: %12u Hz\n", Buffer[4]);
    printf("SHELL: %11s\n", "SealSh"); 
    printf("FONT: \t\t%s\n", "Monospace 8x16"); 
    return SIG_GOOD;
}

int cd()
{
    //Can only cd to absolute path, no relative paths based on current work_dir
    
    if(buffer[2] != ' ' || buffer[3] == '\0'){
        printf("CD - MISSING ARG\n");
        return SIG_FAIL;
    }

    char new_path[bpos-3];
    int i;
    for(i = 3; buffer[i] != ' ' && buffer[i] != '\0'; ++i){
        new_path[i-3] = buffer[i];
    }

    new_path[i-3] = '\0';
    strcpy(work_dir, new_path);

    return SIG_GOOD;
}

int here(){
    printf("%s\n", work_dir);
    return SIG_GOOD;
}

int exit_sh(){
    printf("<EXITING SealShell");
    return SIG_KILL;
}

int help(){
    for(int i = 1; i < NUM_CMDS; ++i){
        printf("%s\n", cmds.n[i]);
    }
    return SIG_GOOD;
}

int cat(){
    if (buffer[3] != ' ' || buffer[4] == '\0'){
        printf("CAT - MISSING ARG\n");
        return SIG_FAIL;
    }

    char cat_file[bpos - 4];
    int i;
    for (i = 4; buffer[i] != ' ' && buffer[i] != '\0'; ++i){
        cat_file[i - 4] = buffer[i];
    }

    cat_file[i - 4] = '\0';
    char* save_dir = strcpy(save_dir, work_dir);
    char* path_to_file = strcat(work_dir, cat_file);
    int sig = SIG_STRT;

    HANDLE fHandle = sdCreateFile(path_to_file, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (fHandle != 0) {
    	uint32_t bytesRead;
    	if ((sdReadFile(fHandle, &buffer[0], 500, &bytesRead, 0) == true))  {
    			buffer[bytesRead-1] = '\0';  ///insert null char
    			printf("File Contents: %s", &buffer[0]);
                sig = SIG_GOOD;
    	}
    	else{
    		printf("CAT FAILED" );
            sig = SIG_FAIL;
    	}
    }
    sdCloseHandle(fHandle);
    work_dir = strcpy(work_dir, save_dir);
    return sig;
}

int run(){
    if (buffer[3] != ' ' || buffer[4] == '\0')
    {
        printf("RUN - MISSING ARG\n");
        return SIG_FAIL;
    }

    char run_file[bpos - 4];
    int i;
    for (i = 4; buffer[i] != ' ' && buffer[i] != '\0'; ++i)
    {
        run_file[i - 4] = buffer[i];
    }

    run_file[i - 4] = '\0';
    char* path_to_file = strcat(work_dir, run_file);

    memRegion app_allocation;
    uint32_t stack_size;

    int status = load_program(path_to_file, &app_allocation, &stack_size);

    return SIG_GOOD;
}

int dump(){
    return SIG_GOOD;
}
