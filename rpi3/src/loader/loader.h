#include <stdint.h>

typedef struct {
    uint8_t* base;
    uint32_t size;
} memRegion;

int load_program(char*, memRegion*, uint32_t*);
