
#include "driverCAN.h"

int main(void) {
    // Inizializza il CAN su GPIOA pin 12 (TX) e 11 (RX), alternate function 9, 500 kbps
    CAN_Init(GPIOA, 12, 11, 9, 500);

    // Invia un messaggio CAN di test
    uint8_t txData[8] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE};
    CAN_SendMessage(0x123, txData, 8);

    // Ciclo principale: ricezione
    while (1) {
        uint32_t rxId;
        uint8_t rxData[8];
        uint8_t rxLen;

        if (CAN_ReceiveMessage(&rxId, rxData, &rxLen)) {
            // Qui puoi elaborare il messaggio ricevuto
            // Per esempio: confrontare ID, eseguire comandi, ecc.
        }
    }
}
