#include "../drivers/sdcard/SDCard.h"
#include "../drivers/stdio/emb-stdio.h" // Needed for printf
#include "../boot/rpi-smartstart.h"        // For sysinfo
#include <string.h>                     // Needed for memcpy
#include <stdarg.h>                     // Needed for variadic arguments

#define NUM_CMDS 3
typedef struct {
    char* n[NUM_CMDS];
    int (*p[NUM_CMDS])(void);
} cmds;

int ls()
{
    printf("Directory (/) <ROOT> : \n");
    HANDLE fh;
    FIND_DATA find;
    char *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    fh = sdFindFirstFile("\\*.*", &find); // Find first file
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
    return 2;
}

int sysinfo()
{
    printf("OS: "); displaySmartStart(&printf);  // OS: 
    // Resolution: 
    // Shell: 
    // Font: 

    return 3;
}

int cd()
{
    return 4;
}
