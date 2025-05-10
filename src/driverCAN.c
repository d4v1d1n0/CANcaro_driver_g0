#include "driverCAN.h"

#define FDCAN_CLOCK 48000000U // Frequenza del clock FDCAN in Hz
#define FDCAN RAM_BASE 0x4000AC00U // Base address della RAM del messaggio FDCAN

// Struttura per la configurazione del filtro standard
typedef struct {
    uint32_t SFID1 : 11;
    uint32_t SFID2 : 11;
    uint32_t SFEC  : 3;
    uint32_t SFT   : 3;
    uint32_t       : 4;
} FDCAN_StdFilterTypeDef;

// Puntatore alla RAM del messaggio FDCAN
volatile uint32_t *FDCAN_RAM = (uint32_t *)FDCAN_RAM_BASE;

void CAN_Init(GPIO_TypeDef *port, uint8_t tx_pin, uint8_t rx_pin, uint8_t af, uint32_t kbps) {
    // 1. Abilita il clock per GPIO e FDCAN
    if (port == GPIOA) RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    else if (port == GPIOB) RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    else if (port == GPIOC) RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
    // Aggiungi altri casi se necessario

    RCC->APBENR1 |= RCC_APBENR1_FDCANEN;

    // 2. Configura i pin TX e RX in alternate function
    port->MODER &= ~(0x3 << (tx_pin * 2));
    port->MODER |= (0x2 << (tx_pin * 2)); // Alternate function
    port->AFR[tx_pin >> 3] &= ~(0xF << ((tx_pin & 0x7) * 4));
    port->AFR[tx_pin >> 3] |= (af << ((tx_pin & 0x7) * 4));

    port->MODER &= ~(0x3 << (rx_pin * 2));
    port->MODER |= (0x2 << (rx_pin * 2)); // Alternate function
    port->AFR[rx_pin >> 3] &= ~(0xF << ((rx_pin & 0x7) * 4));
    port->AFR[rx_pin >> 3] |= (af << ((rx_pin & 0x7) * 4));

    // 3. Entra in modalità di inizializzazione
    FDCAN1->CCCR |= FDCAN_CCCR_INIT;
    while (!(FDCAN1->CCCR & FDCAN_CCCR_INIT));

    // 4. Abilita la configurazione
    FDCAN1->CCCR |= FDCAN_CCCR_CCE;

    // 5. Configura la velocità nominale
    uint32_t nominal_prescaler = FDCAN_CLOCK / (kbps * 16);
    FDCAN1->NBTP = ((15 & 0xFF) << 16) | ((4 & 0x7F) << 8) | ((1 & 0x7F) << 0) | ((nominal_prescaler - 1) << 24);

    // 6. Configura la RAM del messaggio
    FDCAN1->RXGFC = 0x00000000; // Disabilita tutti i filtri
    FDCAN1->SIDFC = ((uint32_t)FDCAN_RAM - FDCAN_RAM_BASE) & 0xFFFF;
    FDCAN1->RXF0C = (((uint32_t)FDCAN_RAM - FDCAN_RAM_BASE + 0x100) & 0xFFFF) | (1 << 16); // 1 elemento
    FDCAN1->TXBC = (((uint32_t)FDCAN_RAM - FDCAN_RAM_BASE + 0x180) & 0xFFFF) | (1 << 16); // 1 elemento

    // 7. Esci dalla modalità di inizializzazione
    FDCAN1->CCCR &= ~FDCAN_CCCR_INIT;
    while (FDCAN1->CCCR & FDCAN_CCCR_INIT);
}

void CAN_SendMessage(uint32_t id, uint8_t *data, uint8_t length) {
    // Attendi che il buffer di trasmissione sia disponibile
    while (!(FDCAN1->TXFQS & FDCAN_TXFQS_TFQF));

    // Configura l'elemento di trasmissione
    uint32_t *tx_buffer = (uint32_t *)(FDCAN_RAM_BASE + 0x180);
    tx_buffer[0] = (id << 18);
    tx_buffer[1] = (length << 16);
    for (uint8_t i = 0; i < length; i++) {
        ((uint8_t *)&tx_buffer[2])[i] = data[i];
    }

    // Richiedi la trasmissione
    FDCAN1->TXBAR = 0x1;
}

int CAN_ReceiveMessage(uint32_t *id, uint8_t *data, uint8_t *length) {
    if (!(FDCAN1->RXF0S & FDCAN_RXF0S_F0FL)) {
        return 0; // Nessun messaggio disponibile
    }

    // Leggi l'elemento di ricezione
    uint32_t *rx_buffer = (uint32_t *)(FDCAN_RAM_BASE + 0x100);
    *id = (rx_buffer[0] >> 18) & 0x7FF;
    *length = (rx_buffer[1] >> 16) & 0xF;
    for (uint8_t i = 0; i < *length; i++) {
        data[i] = ((uint8_t *)&rx_buffer[2])[i];
    }

    // Aggiorna l'indice di lettura
    FDCAN1->RXF0A = 0x0;

    return 1;
}
