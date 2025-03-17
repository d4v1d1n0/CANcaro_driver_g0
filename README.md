## Struttura del Progetto

### File Principale
- **fdcan_driver.c**: Contiene l'implementazione delle funzioni principali per la gestione del CAN.

### Librerie Utilizzate
- `stm32g0xx_hal.h`
- `stm32g0xx_hal_fdcan.h`
- `string.h` per la gestione dei buffer.

---

## Funzionalità Implementate

### 1. Inizializzazione del Modulo CAN
La funzione `CAN_Init`:
- Abilita il clock del modulo FDCAN.
- Configura i parametri del bit timing per una velocità di 500 kbps. La configurazione è calcolata in base alla formula del `bit_timing`, ovvero:
```math
Bitrate=\dfrac{CAN\ Clock}{Prescaler\times\left(1+TimeSeg1+TimeSeg2\right)}
```
I dati per questi calcoli sono inseriti all'interno del programma con i seguenti valori:
`hfdcan1.Init.NominalPrescaler = 6;`
`hfdcan1.Init.NominalTimeSeg1 = 13;`
`hfdcan1.Init.NominalTimeSeg2 = 2;`
`hfdcan1.Init.NominalSyncJumpWidth = 1;`
- Imposta un filtro per accettare solo i messaggi con ID 0x123.
- Attiva gli interrupt per nuovi messaggi ricevuti e completamento della trasmissione.
- Abilita il modulo CAN e configura le priorità degli interrupt NVIC.

### 2. Invio di Messaggi CAN
La funzione `CAN_SendMessage` permette di:
- Inviare messaggi con ID standard a 11 bit.
- Supportare messaggi con una lunghezza massima di 8 byte.
- Utilizzare la coda di trasmissione interna.

### 3. Ricezione di Messaggi CAN
La funzione `CAN_ReceiveMessage`:
- Legge i messaggi ricevuti dalla FIFO e li salva in una coda circolare implementata manualmente.
- La coda ha una dimensione di 32 messaggi, con gestione degli overflow tramite sovrascrittura del messaggio più vecchio.

### 4. Gestione degli Interrupt
- `HAL_FDCAN_RxFifo0Callback`: Viene richiamato quando un nuovo messaggio è disponibile nella FIFO. Controlla lo stato della FIFO e svuota tutti i messaggi presenti nella coda circolare.
- `HAL_FDCAN_TxEventFifoCallback`: Richiamato al completamento di un invio.
- `FDCAN1_IT0_IRQHandler`: Gestisce gli interrupt del modulo FDCAN.

### 5. Coda Circolare
Implementata per evitare la perdita di messaggi quando ne arrivano più di quanti possano essere elaborati istantaneamente.
- Dimensione: 32 messaggi.
- Dimensione di ogni messaggio: 16 byte.
- Memoria totale utilizzata: 512 byte.

### 6. Scelte di Design
- **Bitrate**: Configurato a 500 kbps.
- **Filtri**: Accetta solo messaggi con ID 0x123 per ridurre il carico.
- **Gestione della RAM**: La coda circolare da 32 messaggi occupa solo 512 byte, perfetta per le risorse limitate dello STM32G0.
- **Sicurezza**: Utilizzo di interrupt per la gestione asincrona dei messaggi.

---
