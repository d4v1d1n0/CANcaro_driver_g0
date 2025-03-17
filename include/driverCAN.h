#ifndef DRIVERCAN_H
#define DRIVERCAN_H

#include <stdint.h>

#define CAN_QUEUE_SIZE 32

/**
 * @brief Struttura che rappresenta un messaggio CAN
 */
typedef struct {
    uint32_t id;        /** identificatore del messaggio CAN */
    uint8_t data[8];    /** Payload del messaggio CAN */
    uint8_t len;        /** lunghezza in byte del payload */
} CAN_Messaggio;

/**
 * @brief Inizializza la periferica FDCAN.
 *
 * Configura il bus CAN a 500 kbps, imposta i filtri, abilita gli interrupt,
 * e configura i pin GPIO PA11 (RX) e PA12 (TX) per FDCAN1
 *
 * Questa funzione deve essere chiamata prima di inviare o ricevere messaggi
 */
void CAN_init(void);
/**
 * @brief Inserisce un messaggio nella coda circolare CAN.
 *
 * Questa funzione prende un messaggio di tipo `CAN_Messaggio` e lo inserisce
 * nella coda circolare `canCoda`. Se la coda è piena, il messaggio più vecchio
 * viene sovrascritto
 *
 * @param msg Il messaggio CAN da inserire nella coda
 *
 * @note La coda ha una dimensione massima definita da `CAN_QUEUE_SIZE`. Se la coda
 *       è piena, il puntatore `tail` viene incrementato per eliminare il messaggio
 *       più vecchio e fare spazio a quello nuovo
 */
void CAN_Incoda(CAN_Messaggio msg);
/**
 * @brief Estrae un messaggio dalla coda di ricezione
 *
 * Recupera il messaggio più vecchio ricevuto dal buffer circolare
 *
 * @return CAN_Messaggio Il messaggio estratto
 */
CAN_Messaggio CAN_toglicoda(void);
/**
 * @brief Invia un messaggio CAN.
 *
 * Aggiunge un messaggio CAN alla coda di trasmissione
 *
 * @param id     Identificatore del messaggio (ID standard, 11 bit)
 * @param data   Puntatore al payload dei dati
 * @param length Lunghezza del payload in byte (max 8 byte)
 */
void CAN_InviaMess(uint32_t id, uint8_t *data, uint8_t length);
/**
 * @brief Riceve un messaggio CAN.
 *
 * Questa funzione riceve un messaggio CAN dalla periferica FDCAN.
 */
void CAN_RiceviMess(void);
/**
 * @brief Controlla lo stato FIFO e processa i messaggi in arrivo.
 *
 * Legge tutti i messaggi disponibili dal FIFO RX e li inserisce in coda per l'elaborazione.
 */
void CAN_CheckFIfio(void);
/**
 * @brief Elabora i messaggi CAN ricevuti
 *
 * Questa è una funzione implementata dall'utente per gestire i messaggi in arrivo
 *
 * @param id     Identificatore del messaggio ricevuto
 * @param data   Puntatore al payload del messaggio
 * @param length Lunghezza del payload in byte
 */
void ElaboraMsgRicevuto(uint32_t id, uint8_t *data, uint8_t length);
/**
 * @brief Gestisce gli errori di comunicazione CAN
 * 
 * Questa funzione è chiamata in caso di errore di comunicazione CAN
 */
void Errori(void);

#endif