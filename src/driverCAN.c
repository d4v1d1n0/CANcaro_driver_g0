#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_fdcan.h"
#include <string.h>
#include "driverCAN.h"

#define CAN_QUEUE_SIZE 32

typedef struct {
    uint32_t id;
    uint8_t data[8];
    uint8_t len;
} CAN_Messaggio;

CAN_Messaggio canCoda[CAN_QUEUE_SIZE];
uint8_t head = 0;
uint8_t tail = 0;

void CAN_incoda(CAN_Messaggio msg) {//prende un msg di tipo CAn_Messaggio e lo inserisce in una coda circolare (canCoda)
    canCoda[head]=msg;              //copia il msg nella posiione attuale, quindi in head
    head=(head+1)%CAN_QUEUE_SIZE;   //incrementa head, e nel caso raggiunga il limite della coda, lo riporta a 0 (%=divisione con resto 0)
    if(head==tail){                 //controllo overflow (per preparare la sovrascrittura)
        tail(tail+1)%CAN_QUEUE_SIZE;//incrementa tail, eliminando il messaggio più vecchio (e fa spazio a quello nuovo)
    }
}