#include <stdint.h>  // Needed for uint32_t, uint16_t etc
void trm_init(void);
int trm_capture(char);
int trm_parse_buffer(void);
int trm_exec_run(int);
char* buffer_out(void);
void buffer_clear(void);
void buffer_add_c(char);
