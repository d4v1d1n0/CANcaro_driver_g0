# STM32G0 FDCAN Driver (Accesso diretto ai registri)

Questo progetto fornisce un **driver minimale e portabile** per il modulo **FDCAN** degli **STM32G0** realizzato **esclusivamente tramite accesso diretto ai registri**, senza l'utilizzo della HAL o altre librerie STM32Cube.

## Funzionalità Implementate

* Inizializzazione del modulo FDCAN1
* Configurazione parametrica dei pin TX e RX (porta, numero pin, alternate function)
* Configurazione della velocità CAN (es. 500 kbps)
* Invio di messaggi CAN standard (ID a 11 bit, fino a 8 byte)
* Ricezione di messaggi CAN dalla FIFO 0

## Struttura del Progetto

```
├── driverCAN.h          // Header con le dichiarazioni e i commenti Doxygen
├── driverCAN.c          // Implementazione del driver con registri puri
├── main.c               // Esempio di utilizzo del driver
├── README.md            // Questo file
```

## API Driver

### void CAN\_Init(GPIO\_TypeDef \*port, uint8\_t tx\_pin, uint8\_t rx\_pin, uint8\_t af, uint32\_t kbps)

Inizializza il modulo FDCAN.

* `port`: GPIOA / GPIOB / GPIOC
* `tx_pin`: Numero del pin TX (es. 12)
* `rx_pin`: Numero del pin RX (es. 11)
* `af`: Alternate function (tipicamente 9 per FDCAN1)
* `kbps`: Baud rate (es. 500)

### void CAN\_SendMessage(uint32\_t id, uint8\_t \*data, uint8\_t length)

Invia un messaggio CAN standard con ID a 11 bit e payload fino a 8 byte.

### int CAN\_ReceiveMessage(uint32\_t \*id, uint8\_t \*data, uint8\_t \*length)

Riceve un messaggio CAN dalla FIFO0. Restituisce 1 se il messaggio è disponibile, 0 altrimenti.

## Esempio

```c
#include "driverCAN.h"

int main(void) {
    CAN_Init(GPIOA, 12, 11, 9, 500);  // PA12 = TX, PA11 = RX, AF9, 500 kbps

    uint8_t data_out[8] = {0xDE, 0xAD, 0xBE, 0xEF};
    CAN_SendMessage(0x123, data_out, 4);

    while (1) {
        uint32_t id;
        uint8_t data_in[8];
        uint8_t len;

        if (CAN_ReceiveMessage(&id, data_in, &len)) {
            // Elabora il messaggio
        }
    }
}
```

## Considerazioni Tecniche

* Il driver accede direttamente ai registri del modulo FDCAN.
* Supporta solo ID **standard (11 bit)** e **Classic CAN** (non FD).
* Utilizza una singola entry in RX FIFO0 e TX buffer.
* Progettato per **STM32G0** con clock FDCAN a 48 MHz.

## 📜 Licenza

MIT License - vedi file `LICENSE`


