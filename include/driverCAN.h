#ifndef FDCAN_DRIVER_REGISTERS_H
#define FDCAN_DRIVER_REGISTERS_H

#include "stm32g0xx.h"
#include <stdint.h>

/**
 * @brief Inizializza il modulo FDCAN usando l'accesso diretto ai registri.
 *
 * Configura il clock, i pin GPIOA (PA11/PA12), imposta il baud rate a 500 kbps,
 * abilita i filtri per ricevere tutti i messaggi standard e avvia il modulo FDCAN1.
 */
void CAN_Init(void);

/**
 * @brief Invia un messaggio CAN con ID standard.
 *
 * @param id     ID del messaggio (11 bit standard).
 * @param data   Puntatore ai dati (max 8 byte).
 * @param length Lunghezza del payload in byte (0-8).
 */
void CAN_SendMessage(uint32_t id, uint8_t *data, uint8_t length);

/**
 * @brief Riceve un messaggio CAN dalla FIFO0, se disponibile.
 *
 * @param id     Puntatore per ricevere l'ID del messaggio.
 * @param data   Buffer per i dati ricevuti (minimo 8 byte).
 * @param length Puntatore per ricevere la lunghezza del messaggio.
 * @return int   1 se un messaggio è stato ricevuto, 0 altrimenti.
 */
int CAN_ReceiveMessage(uint32_t *id, uint8_t *data, uint8_t *length);

#endif // FDCAN_DRIVER_REGISTERS_H