#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_fdcan.h"
#include <string.h>

#define CAN_QUEUE_SIZE 32

typedef struct {
    uint32_t id;
    uint8_t data[8];
    uint8_t len;
} CAN_Messaggio;

CAN_Messaggio canCoda[CAN_QUEUE_SIZE];
uint8_t head = 0, tail = 0;

