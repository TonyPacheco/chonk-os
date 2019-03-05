#include <stdint.h>  // Needed for uint32_t, uint16_t etc
#include <stdbool.h> // Needed for bool
void trm_init(void);
uint8_t trm_capture(uint8_t);
bool trm_parse_buffer(void);
uint8_t trm_exec_run(void);
char* buffer_out(void);
void buffer_clear(void);
void buffer_add_c(uint8_t);
