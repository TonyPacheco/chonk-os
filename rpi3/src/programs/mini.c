#include "mini.h"
#include "../signal.h"
#include "../drivers/sdcard/SDCard.h"
#include "../drivers/stdio/emb-stdio.h"
#include <string.h>
#include <stdio.h>

#define MINI_BUFFER_MAX 900
#define EDIT_MODE 1
#define CTRL_MODE 0


uint8_t mode;
char file[50];
char w_dir[50];
char buffer[MINI_BUFFER_MAX];
int bpos = 0;

int start_mini_new(char* dir){
    mini_init(dir, "temp");
    return mini_main();
}

int start_mini_file(char* dir, char* filename){
    mini_init(dir, filename);
    char path_to_file[50];
    sprintf(path_to_file, "%s%s", dir, filename);
    if(mini_load(path_to_file) == SIG_FAIL){
        mini_init(dir, "temp");
        ctrl_output("Load failed!");
    }
    return mini_main();
}

int mini_init(char* dir, char* filename){
    ClearScreen(0, 0);
    for(int i = 0; i < MINI_BUFFER_MAX; ++i)
        buffer[i] = '\0';
    printf("MINI - Text Editor\n ");
    sprintf(w_dir, "%s", dir);
    sprintf(file, "%s", filename);
    mode = EDIT_MODE;
    return SIG_GOOD;
}

int mini_load(char* path_to_file){
    ctrl_output(path_to_file);
    HANDLE handle = sdCreateFile(path_to_file, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (handle != 0) {
        int file_size = sdGetFileSize(handle, 0);
        uint32_t bytesRead;
        char *buffer_w;
        if ((sdReadFile(handle, &buffer_w[0], file_size, &bytesRead, 0) == true)) {
            for (size_t i = 0; i < MINI_BUFFER_MAX && i < file_size; ++i) {
                mini_capture(buffer_w[i]);
            }
        }
    } else {
        return SIG_FAIL;
    }
    sdCloseHandle(handle);
    return SIG_GOOD;
}

int mini_main(){
    char c;
    int s = SIG_STRT;

    while (s != SIG_HALT){
        c = hal_io_serial_getc(0);
        hal_io_serial_putc(0, c);
        s = mini_capture(c);
    }
    return SIG_GOOD;
}

int mini_capture(char ch) {
    if (ch == '\e') {
        switch_mode();
        return SIG_GOOD;
    }
    
    if(mode == EDIT_MODE){
        printf("%c", ch);
        if (ch == '\b') {
            mini_back_sp();
        } else if (ch == '\n' || ch == '\r') {
            printf(" ");
            mini_add_c(ch);
        } else {
            mini_add_c(ch);
        }
    } else { //Control mode
        if(ch == 's') {
            return mini_save();
        } else if(ch == 'n') {
            return mini_init(w_dir, "temp");
        } else if (ch == 'o') {
            return mini_open();
        } else if (ch == 'x') {
            return mini_exit();
        } else if (ch == 'i') {
            MoveCursor(GetCursorX(), GetCursorY()-1);
        } else if (ch == 'j') {
            MoveCursor(GetCursorX()-1, GetCursorY());
        } else if (ch == 'k') {
            MoveCursor(GetCursorX(), GetCursorY()-1);
        } else if (ch == 'l') {
            MoveCursor(GetCursorX()+1, GetCursorY());
        }
    }
    return SIG_GOOD;
}

void mini_add_c(char c) {
    buffer[bpos++] = c;
    buffer[bpos] = '\0';
}

void mini_back_sp() {
    if (bpos == 0)
        return;
    buffer[--bpos] = '\0';
}

int mini_exit(){
    ClearScreen(0, 0);
    return SIG_HALT;
}

void switch_mode(){
    mode ^= 1;
    ctrl_output(mode == EDIT_MODE ? "#EDIT" : "#CTRL");
}

int mini_open(){
    char input[35];
    ctrl_input("Filename:", &input);
    char path_to_file[50];
    sprintf(path_to_file, "%s%s", w_dir, input);
    mini_init(w_dir, input);
    if(mini_load(path_to_file) == SIG_FAIL){
        mini_init(w_dir, "temp");
        ctrl_output("Load failed!");
        return SIG_FAIL;
    }
    return SIG_GOOD;
}

int mini_save() {
    if(!strcmp(file, "temp")){
        char input[35];
        ctrl_input("Filename:", &input);
        sprintf(file, "%s", input);
    }
    char path_to_file[50];
    sprintf(path_to_file, "%s%s", w_dir, file);
    HANDLE handle = sdCreateFile(path_to_file, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    sdCloseHandle(handle);
    ctrl_output("SAVED!");
    return SIG_GOOD;
}

void ctrl_output(char* msg) {
    int x = GetCursorX();
    int y = GetCursorY();
    MoveCursor(30, 0);
    printf("                                       ");
    MoveCursor(30, 0);
    printf(msg);
    MoveCursor(x, y);
}

void ctrl_input(char* prompt, char** field){
    char c = ' ';
    int i = 0;
    char buf[35];
    ctrl_output(prompt);
    while (c != '\r' && c != '\n') {
        c = hal_io_serial_getc(0);
        hal_io_serial_putc(0, c);
        buf[i++] = c;
        buf[i] = '\0';
        char* out;
        sprintf(out, "%s%s", prompt, buf);
        ctrl_output(out);
    }
    sprintf(*field, "%s", buf);
}

