#include "stm32g0xx.h"
#include "driverCAN.h"

// Inizializzazione CAN tramite registri
void CAN_Init(void) {
    // Abilita il clock del modulo FDCAN e dei GPIO
    RCC->APBENR1 |= RCC_APBENR1_FDCANEN;  // Abilita il clock per il modulo FDCAN
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;    // Abilita il clock per GPIOA

    // Configura PA11 (RX) e PA12 (TX) come alternate function (AF9)
    GPIOA->MODER &= ~(GPIO_MODER_MODE11_Msk | GPIO_MODER_MODE12_Msk);
    GPIOA->MODER |= (2 << GPIO_MODER_MODE11_Pos) | (2 << GPIO_MODER_MODE12_Pos);
    GPIOA->AFR[1] |= (9 << GPIO_AFRH_AFSEL11_Pos) | (9 << GPIO_AFRH_AFSEL12_Pos);

    // Entra in modalità di inizializzazione (CCCR - Control Register)
    FDCAN1->CCCR |= FDCAN_CCCR_INIT;
    while (!(FDCAN1->CCCR & FDCAN_CCCR_INIT)) {}  // Attendi che entri in modalità INIT

    // Abilita la configurazione (CCCR - CCE bit)
    FDCAN1->CCCR |= FDCAN_CCCR_CCE;

    // Configura velocità a 500 kbps (NBTP - Nominal Bit Timing and Prescaler)
    // CAN clock = 48 MHz, Prescaler = 6, TimeSeg1 = 13, TimeSeg2 = 2, SyncJumpWidth = 1
    FDCAN1->NBTP = (6 - 1) << FDCAN_NBTP_NBRP_Pos |
                    (13 - 1) << FDCAN_NBTP_NTSEG1_Pos |
                    (2 - 1) << FDCAN_NBTP_NTSEG2_Pos |
                    (1 - 1) << FDCAN_NBTP_NSJW_Pos;

    // Accetta tutti i messaggi nella RX FIFO 0 (RXGFC - Global Filter Configuration)
    FDCAN1->RXGFC = (0x2 << FDCAN_RXGFC_ANFE_Pos) | (0x2 << FDCAN_RXGFC_ANFS_Pos);

    // Esci dalla modalità di inizializzazione
    FDCAN1->CCCR &= ~FDCAN_CCCR_INIT;
    while (FDCAN1->CCCR & FDCAN_CCCR_INIT) {}  // Attendi che esca dalla modalità INIT
}

// Invia un messaggio CAN
void CAN_SendMessage(uint32_t id, uint8_t *data, uint8_t length) {
    // Controlla se c'è un buffer di trasmissione libero (TXFQS - Transmit FIFO/Queue Status)
    uint32_t freeLevel = (FDCAN1->TXFQS & FDCAN_TXFQS_TFQF) == 0;
    if (!freeLevel) return;  // Nessun buffer libero

    // Imposta ID e lunghezza del messaggio
    FDCAN1->TXBAR = 0x1;  // Richiedi trasmissione su Buffer 0
    FDCAN1->TXBC |= (0x1 << 0);  // Seleziona Buffer 0
    FDCAN1->TXF0S = (id << 18) & FDCAN_TXF0S_EFF;  // ID standard
    FDCAN1->TXF0S |= (length & 0xF) << FDCAN_TXF0S_DLC_Pos;  // Lunghezza dati

    // Scrive i dati nei registri di trasmissione
    for (int i = 0; i < length; i++) {
        *((volatile uint8_t*)&FDCAN1->TXBRP + i) = data[i];
    }

    // Richiede la trasmissione (TXBAR - Transmit Buffer Add Request)
    FDCAN1->TXBAR = 0x1;
}

// Ricevi un messaggio CAN
int CAN_ReceiveMessage(uint32_t *id, uint8_t *data, uint8_t *length) {
    // Controlla se ci sono messaggi nella FIFO 0 (RXF0S - Receive FIFO 0 Status)
    if ((FDCAN1->RXF0S & FDCAN_RXF0S_F0FL_Msk) == 0) {
        return 0;  // Nessun messaggio disponibile
    }

    // Legge ID e lunghezza
    *id = (FDCAN1->RXF0S & FDCAN_RXF0S_EFF) >> 18;
    *length = (FDCAN1->RXF0S & FDCAN_RXF0S_DLC_Msk) >> FDCAN_RXF0S_DLC_Pos;

    // Legge i dati ricevuti
    for (int i = 0; i < *length; i++) {
        data[i] = *((volatile uint8_t*)&FDCAN1->RXF0S + i);
    }

    // Conferma la ricezione (RXF0A - Receive FIFO 0 Acknowledge)
    FDCAN1->RXF0A = (FDCAN1->RXF0A & FDCAN_RXF0A_F0AI_Msk) + 1;
    return 1;  // Messaggio ricevuto
}
