#include "terminal.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../drivers/sdcard/SDCard.h"
#include "../drivers/stdio/emb-stdio.h" 
#include <stdarg.h>
#include "../hal/hal.h"

#define NUM_CMDS 5
#define WIDTH 640
#define HEIGHT 480
typedef struct
{
    char *n[NUM_CMDS];
    int (*p[NUM_CMDS])(void);
} cmd_map;

#define BUFFER_MAX 500

#define SIG_KILL 66
#define SIG_STRT 0
#define SIG_GOOD 1
#define SIG_FAIL -1

char buffer[BUFFER_MAX];
int bpos;
cmd_map cmds;
char* work_dir;

void trm_init(){
    bpos = 0;
    work_dir = "\\";

    cmds.n[0] = "ls";
    cmds.n[1] = "sysinfo";
    cmds.n[2] = "cd";
    cmds.n[3] = "exit";
    cmds.n[4] = "here";

    cmds.p[0] = ls;
    cmds.p[1] = sysinfo;
    cmds.p[2] = cd;
    cmds.p[3] = exit_sh;
    cmds.p[4] = here;

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
        if(cmd_index == -1)
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

int trm_parse_buffer() { //return index of command to call, or -1 if not found
    char cm[bpos];
    int i = 0;
    for (i = 0; buffer[i] != '\0' && buffer[i] != ' '; i++){
        cm[i] = buffer[i];
    }
    for (int i = 0; i < NUM_CMDS; ++i)
        if (!strcmp(cm, cmds.n[i]))
            return i;
    return -1;
}

char* buffer_out(){
    return buffer;
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
    if(buffer[2] != ' ' || buffer[3] == '\0'){
        printf("CD - MISSING ARG\n");
        return SIG_FAIL;
    }

    char new_path[bpos-3];
    int i;
    for(i = 3; buffer[i] != ' ' && buffer[i] != '\0'; ++i){
        new_path[i-3] = buffer[i];
    }

    if(buffer[i-1] != '\\')
        new_path[i-2] = '\\'; 

    new_path[i-3] = '\0';
    strcat(work_dir, new_path);
    return SIG_GOOD;
}

int here(){
    printf("%s\n", work_dir);
    return SIG_GOOD;
}

int exit_sh(){
    printf("EXITING SealShell");
    return SIG_KILL;
}
