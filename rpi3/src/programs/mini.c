#include "mini.h"
#include "../signal.h"
#include "../drivers/sdcard/SDCard.h"
#include "../drivers/stdio/emb-stdio.h"
#include <string.h>
#include <stdio.h>

#define MINI_BUFFER_MAX 900
#define EDIT_MODE 1
#define CTRL_MODE 0


uint8_t mode = EDIT_MODE;
char* file = "temp";
char* w_dir;
char buffer[MINI_BUFFER_MAX];
int bpos = 0;

int start_mini_new(char* dir){
    mini_clear();
    return mini_main();
}

int start_mini_file(char* dir, char* filename){
    mini_clear();
    sprintf(w_dir, "%s", dir);
    char path_to_file[50];
    sprintf(path_to_file, "%s%s", dir, filename);
    ctrl_output(path_to_file);
    HANDLE handle = sdCreateFile(path_to_file, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (handle != 0)
    {
        int file_size = sdGetFileSize(handle, 0);
        uint32_t bytesRead;
        char *buffer_w;
        if ((sdReadFile(handle, &buffer_w[0], file_size, &bytesRead, 0) == true)){
            for (size_t i = 0; i < MINI_BUFFER_MAX; ++i) {
                mini_capture(buffer_w[i]);
            }
        }
    } else {
        ctrl_output("Load failed!");
    }
    sdCloseHandle(handle);
    return mini_main();
}

void mini_clear(){
    ClearScreen(0, 0);
    printf("MINI - Text Editor\n ");
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
            mini_save();
            return SIG_SAVE;
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
    char* note = mode == EDIT_MODE ? "_EDIT" : "_CTRL";
    ctrl_output(note);
}

int mini_save() {
    if(!strcmp(file, "temp")){
        char input[35];
        ctrl_input("Filename:", &input);
        sprintf(file, "%s%s", w_dir, input);
    }
    ctrl_output("SAVED!");
    HANDLE handle = sdCreateFile(file, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    sdCloseHandle(handle);
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
        char* out;
        sprintf(out, "%s%s", prompt, buf);
        ctrl_output(out);
    }
    sprintf(*field, "%s", buf);
}

