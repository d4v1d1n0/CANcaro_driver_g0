#ifndef DRIVERCAN_H
#define DRIVERCAN_H

#include <stdint.h>

#define CAN_QUEUE_SIZE 32

typedef struct {
    uint32_t id;
    uint8_t data[8];
    uint8_t len;
} CAN_Messaggio;

void CAN_incoda(CAN_Messaggio msg);
CAN_Messaggio CAN_toglicoda(void);
void CAN_init(void);
void CAN_InviaMess();
void CAN_RiceviMess();
void CAN_