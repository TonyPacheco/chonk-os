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
#include "hal/hal.h"
#include "terminal/terminal.h"

char buffer[500];

void DisplayDirectory(const char*);

int main (void) {
	PiConsole_Init(0, 0, 0, &printf);								// Auto resolution console, show resolution to screen

	displaySmartStart(&printf);
	printf("\n");															// Display smart start details
	ARM_setmaxspeed(&printf);										// ARM CPU to max speed and confirm to screen

	/* Display the SD CARD directory */
	sdInitCard (&printf, &printf, true);
	printf("\n");

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

	// }

	//hal_io_video_init();
	//hal_io_video_puts( "HELLO THERE ", 3, VIDEO_COLOR_WHITE );

	//Typewriter
	//hal_io_serial_init();
	//hal_io_serial_puts( SerialA, "Typewriter:" );

	char c;
	trm_init();

	while(1){
		c = hal_io_serial_getc( SerialA );
	  trm_capture(c);
		hal_io_serial_putc( SerialA, c );
		printf( "%c", c );
	}



	/* display bitmap on screen */
	//DisplayBitmap(743, 624, "./MINIOS.BMP");   //<<<<-- Doesn't seem to work

	return(0);
}
