#include "terminal.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../drivers/sdcard/SDCard.h"
#include "../drivers/stdio/emb-stdio.h" 
#include <stdarg.h>                     

#define NUM_CMDS 3
#define WIDTH 640
#define HEIGHT 480
typedef struct
{
    char *n[NUM_CMDS];
    int (*p[NUM_CMDS])(void);
} cmds;

char* buffer;
int bpos;
cmds commands;
char* work_dir;

void trm_init(){
    bpos = 0;
    work_dir = "\\";
    commands.n[0] = "ls";
    commands.n[1] = "sysinfo";
    commands.n[2] = "cd";
    commands.p[0] = ls;
    commands.p[1] = sysinfo;
    commands.p[2] = cd;
    printf("%s>", work_dir);
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
            cmd_return_val = commands.p[cmd_index]();

        buffer_clear();
        printf("%s>", work_dir);
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
    char cm[bpos];
    int i = 0;
    for (i = 0; buffer[i] != '\0' && buffer[i] != ' '; i++){
        cm[i] = buffer[i];
    }
    for (int i = 0; i < NUM_CMDS; ++i)
        if (!strcmp(cm, commands.n[i]))
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
    return 0;
}

int sysinfo()
{
    uint32_t Buffer[5];
    printf("OS: "); displaySmartStart(&printf);
    printf("Resolution: %dx%d \n", WIDTH, HEIGHT); 
    printf("CPU frequency: %u Hz\n", Buffer[4]);
    printf("Shell: \n"); 
    printf("Font: \n"); 
    return 0;
}

int cd()
{
    if(buffer[2] != ' ' || buffer[3] == '\0'){
        printf("CD - MISSING ARG\n");
        return -1;
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
    return 0;
}
