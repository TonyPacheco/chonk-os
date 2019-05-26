/**
*     	main.c
*
*				Kernel's Entry Point
*
*/

#include <stdbool.h>			// Neede for bool
#include <stdint.h>				// Needed for uint32_t, uint16_t etc
#include <string.h>				// Needed for memcpy
#include <stdarg.h>				// Needed for variadic arguments
#include "drivers/stdio/emb-stdio.h"			// Needed for printf
#include "boot/rpi-smartstart.h"		// Needed for smart start API
#include "drivers/sdcard/SDCard.h"
#include "terminal/terminal.h"

int main (void) {
	PiConsole_Init(0, 0, 0, 0);								// Auto resolution console, show resolution to screen

	// displaySmartStart(&printf);
	printf("\n");															// Display smart start details
	ARM_setmaxspeed(0);									    	// ARM CPU to max speed and confirm to screen

	/* Display the SD CARD directory */
	sdInitCard (0, &printf, true);

	/* Start up the main terminal loop! */
	trm_init();

	return (0);

	//printf("Opening Alice.txt \n");

	//HANDLE fHandle = sdCreateFile("Alice.txt", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	// if (fHandle != 0) {
	// 	uint32_t bytesRead;

	// 	if ((sdReadFile(fHandle, &buffer[0], 500, &bytesRead, 0) == true))  {
	// 			buffer[bytesRead-1] = '\0';  ///insert null char
	// 			printf("File Contents: %s", &buffer[0]);
	// 	}
	// 	else{
	// 		printf("Failed to read" );
	// 	}

	// 	// Close the file
	// 	sdCloseHandle(fHandle);

	/* display bitmap on screen */
	//DisplayBitmap(743, 624, "./MINIOS.BMP");   //<<<<-- Doesn't seem to work
}
