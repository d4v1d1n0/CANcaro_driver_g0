#ifndef driverCAN_H
#define driverCAN_H

#include "stm32g0xx.h"
#include <stdint.h>

/**
 * @brief Inizializza il modulo CAN usando i registri.
 *
 * Configura i pin indicati per TX e RX, assegna l'alternate function, e imposta
 * la velocità CAN in kbps. Supporta solo ID standard e ricezione FIFO0.
 *
 * @param port     Puntatore al GPIO (es. GPIOA)
 * @param tx_pin   Numero del pin TX (es. 12)
 * @param rx_pin   Numero del pin RX (es. 11)
 * @param af       Alternate function da usare (tipicamente 9 per FDCAN)
 * @param kbps     Velocità desiderata in kilobit al secondo (es. 500)
 */
void CAN_Init(GPIO_TypeDef *port, uint8_t tx_pin, uint8_t rx_pin, uint8_t af, uint32_t kbps);

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

#endif
