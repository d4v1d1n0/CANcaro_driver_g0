#include "driverCAN.h"

int main(void) {
    CAN_Init();

    // Invia un messaggio di test
    uint8_t txData[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    CAN_SendMessage(0x123, txData, 4);

    // Ciclo principale
    while (1) {
        uint32_t rxId;
        uint8_t rxData[8];
        uint8_t rxLength;

        // Se è disponibile un messaggio CAN, lo riceve
        if (CAN_ReceiveMessage(&rxId, rxData, &rxLength)) {
            // Qui puoi elaborare il messaggio ricevuto
            // Per esempio: accendere un LED, cambiare stato, ecc.
        }
    }
}
