#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_fdcan.h"
#include <string.h>
#include "driverCAN.h"

#define CAN_QUEUE_SIZE 32
#define ERROR_CAN_ID 0x7FF 

typedef struct {
    uint32_t id;
    uint8_t data[8];
    uint8_t len;
} CAN_Messaggio;

CAN_Messaggio canCoda[CAN_QUEUE_SIZE];
uint8_t head = 0;
uint8_t tail = 0;

void CAN_Incoda(CAN_Messaggio msg) {//prende un msg di tipo CAn_Messaggio e lo inserisce in una coda circolare (canCoda)
    canCoda[head]=msg;              //copia il msg nella posiione attuale, quindi in head
    head=(head+1)%CAN_QUEUE_SIZE;   //incrementa head, e nel caso raggiunga il limite della coda, lo riporta a 0 (%=divisione con resto 0)
    if(head==tail){                 //controllo overflow (per preparare la sovrascrittura)
        tail(tail+1)%CAN_QUEUE_SIZE;//incrementa tail, eliminando il messaggio più vecchio (e fa spazio a quello nuovo)
    }
}

CAN_Messaggio CAN_toglicoda(void){
    CAN_Messaggio msg=canCoda[tail];    //copia il messaggio in tail in msg
    tail=(tail+1)%CAN_QUEUE_SIZE;       //incrementa tail, e nel caso raggiunga il limite della coda, lo riporta a 0 (%=divisione con resto 0)
    return msg;                         //ritorna il messaggio
}

void CAN_InviaMess(uint32_t id, uint8_t *data, uint8_t length) {
    CAN_Messaggio msg;
    msg.id = id;
    memcpy(msg.data, data, length);
    msg.len = length;
    CAN_Incoda(msg);
}

void CAN_init(void){
    __HAL_RCC_FDCAN_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    hfdcan1.Instance = FDCAN1;
    hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
    hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
    hfdcan1.Init.AutoRetransmission = ENABLE;
    hfdcan1.Init.TransmitPause = DISABLE;
    hfdcan1.Init.ProtocolException = DISABLE;
    hfdcan1.Init.NominalPrescaler = 6;
    hfdcan1.Init.NominalSyncJumpWidth = 1;
    hfdcan1.Init.NominalTimeSeg1 = 13;
    hfdcan1.Init.NominalTimeSeg2 = 2;

    if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK) {
        Errori();
    }

    FDCAN_FilterTypeDef sFilterConfig;
    sFilterConfig.IdType = FDCAN_STANDARD_ID;
    sFilterConfig.FilterIndex = 0;
    sFilterConfig.FilterType = FDCAN_FILTER_MASK;
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig.FilterID1 = 0x123;
    sFilterConfig.FilterID2 = 0x7FF;

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) {
        Erroi();
    }

    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_TX_COMPLETE, 0);

    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
        Errori();
    }

    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
}

void CAN_RiceviMess(void) {
    FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];

    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
        CAN_Message msg;
        msg.id = RxHeader.Identifier;
        msg.length = RxHeader.DataLength >> 16;
        memcpy(msg.data, RxData, msg.length);
        CAN_Incoda(msg);
    }
}

void CAN_CheckFIfio(void){
    uint32_t fifoLevel = HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0);
    if (fifoLevel > 0) {
        for (uint32_t i = 0; i < fifoLevel; i++) {
            CAN_RiceviMess();
        }
    }
}

// Callback degli interrupt
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) {
        CAN_CheckFifoStatus();
    }
}

void HAL_FDCAN_TxEventFifoCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TxEventFifoITs) {
    if (TxEventFifoITs & FDCAN_IT_TX_COMPLETE) {
        uint8_t newMessage[] = {0x00, 0x00, 0x00, 0x00};
        CAN_SendMessage(0x124, newMessage, sizeof(newMessage));
    }
}

// Interrupt handler
void FDCAN1_IT0_IRQHandler(void) {
    HAL_FDCAN_IRQHandler(&hfdcan1);
}

void Errori(void){
    uint8_t errorMsg[8]={'e','r','r','o','r',0x00,0x00,0x00};
    CAN_InviaMess(ERROR_CAN_ID, errorMsg, size(errorMsg))
    while (1){};
}