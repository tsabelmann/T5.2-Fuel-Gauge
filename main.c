/*
 * File:   main.c
 * Author: tim.sabelmann
 *
 * Created on 24. März 2023, 13:17
 */

#define MAJOR_VERSION_NUMBER    1U
#define MINOR_VERSION_NUMBER    0U
#define PATCH_VERSION_NUMBER    0U

#include <xc.h>

#include "mcc_generated_files/can/can1.h"
#include "mcc_generated_files/can/can_interface.h"
#include "mcc_generated_files/spi/spi2.h"
#include "mcc_generated_files/system/pins.h"
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/tmr0.h"

/* Transmit CAN-frame */
static uint8_t TxData[8];
static struct CAN_MSG_OBJ TxFrm = {
    .msgId = 0x291,
    .field = {
        .formatType = CAN_2_0_FORMAT,
        .brs = CAN_NON_BRS_MODE,
        .frameType = CAN_FRAME_DATA,
        .idType = CAN_FRAME_STD,
        .dlc = DLC_8
    },
    .data = TxData
};

/* Receiving CAN-frame */
static uint8_t RxData[8];
static struct CAN_MSG_OBJ RxFrm = {
    .field = {
        .formatType = CAN_2_0_FORMAT,
        .brs = CAN_NON_BRS_MODE,
        .frameType = CAN_FRAME_DATA,
        .idType = CAN_FRAME_STD,
        .dlc = DLC_8
    },
    .data = RxData
};

/* Potentiometer values */
static char P0 = 0xFF;
static char P1 = 0xFF;
static uint16_t SoC = 5100U;
static uint32_t Tmr = 0U;
static uint32_t TiOutTmr = 0U;

/* Timer 0 */
static uint32_t __INTERNAL_TIMER__ = 0U;
static void UpdateTmr(void) {
    __INTERNAL_TIMER__ += 1000u;
}

static void IO_RTC_StartTime(uint32_t * const ptr) {
    if (ptr != NULL) {
        *ptr = __INTERNAL_TIMER__;
    }
}

static uint32_t IO_RTC_GetTimeUS(uint32_t tmr) {
    return (__INTERNAL_TIMER__ - tmr);
}

uint16_t linear_map(uint16_t a, uint16_t b, uint16_t x, uint16_t y, uint16_t value)
{
    if (value < a) {
        return x;
    } else if (value > b) {
        return y;
  } else {
        uint32_t A = a;
        uint32_t B = b;
        uint32_t X = x;
        uint32_t Y = y;
        uint32_t V = value;
    
        uint32_t ret = X + (((V - A) * (Y - X))) / (B - A);
        return ((uint16_t)ret);
    }
}

bool is_little_endian(void) {
    uint16_t value = 0x0001;
    uint8_t * ptr = (uint8_t*)&value;
    if (ptr[0] == 0x01)
        return true;
    else
        return false;
}

bool is_big_endian(void) {
    uint16_t value = 0x0001;
    uint8_t * ptr = (uint8_t*)&value;
    if (ptr[0] == 0x01)
        return false;
    else
        return true;
}

void setup_Poti()
{
    SPI2.Open(SPI2_DEFAULT);
    /* TCON */
    __delay_us(50);
    IO_RA1_SetLow();
    __delay_us(50);
    SPI2.ByteExchange(0x40);
    SPI2.ByteExchange(0xEE);
    __delay_us(500);
    IO_RA1_SetHigh();
    __delay_us(50);
    SPI2.Close();
    __delay_us(50);
    /* Wiper 0 */
    SPI2.Open(SPI2_DEFAULT);
    __delay_us(50);
    IO_RA1_SetLow();
    __delay_us(50);
    SPI2.ByteExchange(0x00);
    SPI2.ByteExchange(0xFB);
    __delay_us(500);
    IO_RA1_SetHigh();
    __delay_us(50);
    SPI2.Close();
    __delay_us(50);
    /* Wiper 1 */
    SPI2.Open(SPI2_DEFAULT);
    __delay_ms(50);
    IO_RA1_SetLow();
    __delay_us(50);
    SPI2.ByteExchange(0x10);
    SPI2.ByteExchange(0xFB);
    __delay_us(500);
    IO_RA1_SetHigh();
    __delay_us(50);
    SPI2.Close();
}

void transmit_P(uint8_t p0, uint8_t p1) 
{    
    /* Wiper 0 */
    SPI2.Open(SPI2_DEFAULT);
    __delay_us(50);
    IO_RA1_SetLow();
    __delay_us(50);
    SPI2.ByteExchange(0x00);
    SPI2.ByteExchange(p0);
    __delay_us(500);
    IO_RA1_SetHigh();
    __delay_us(50);
    SPI2.Close();
    __delay_us(50);
    /* Wiper 1 */
    SPI2.Open(SPI2_DEFAULT);
    __delay_us(50);
    IO_RA1_SetLow();
    __delay_us(50);
    SPI2.ByteExchange(0x10);
    SPI2.ByteExchange(p1);
    __delay_us(500);
    IO_RA1_SetHigh();
    __delay_us(50);
    SPI2.Close();
}

void calculate_P(char * const p0, char * const p1, uint16_t percent)
{    
//    uint8_t GeberVollAdjust = 0u;
//    uint8_t GeberLeerAdjust = 0u;
    
    if (percent < 625) {
        /* 0/16 */
        *p0 = 0xE9;
        *p1 = 0xFF;
    } else if (
        (625 <= percent)
        && (percent < 1250)
    ) {
        /* 1/16 */
        *p0 = 0xF2;
        *p1 = 0xFE;
    } else if (
        (1250 <= percent)
        && (percent < 1875)
    ) {
        /* 2/16 */
        *p0 = 0xF5;
       * p1 = 0xFE;
    } else if (
        (1875 <= percent)
        && (percent < 2500)
    ) {
        /* 3/16 */
        *p0 = 0xF6;
        *p1 = 0xFD;
    } else if (
        (2500 <= percent)
        && (percent < 3750)
    ) {
        /* 4/16*/
        *p0 = 0xF7;
        *p1 = 0xFC;
    } else if (
        (3750 <= percent)
        && (percent < 5000)
    ) {
        /* 3/8 */
        *p0 = 0xFA;
        *p1 = 0xFC;
    } else if (
        (5000 <= percent)
        && (percent < 6250)
    ) {
        /* 4/8 */
        *p0 = 0xFB;
        *p1 = 0xFB;
    } else if (
        (6250 <= percent)
        && (percent < 7500)
    ) {
        /* 5/8 */
        *p0 = 0xFC;
        *p1 = 0xF8;
    } else if (
        (7500 <= percent)
        && (percent < 8750)
    ) {
        /* 6/8 */
        *p0 = 0xFE;
        *p1 = 0xF7;
    } else if (
        (8750 <= percent)
        && (percent < 9500)
    ) {
        /* 7/8 */
        *p0 = 0xFF;
        *p1 = 0xF5;
    } else if (9500 <= percent) {
        /* 8/8 */
        *p0 = 0xFF;
        *p1 = 0xE9;
    }
}

void main(void) {    
    SYSTEM_Initialize();     
    CAN1_OperationModeSet(CAN_NORMAL_2_0_MODE);    
    Timer0_OverflowCallbackRegister(&UpdateTmr);
    Timer0_Start();
    INTERRUPT_GlobalInterruptEnable();    
    
//    IO_RTC_StartTime(&Tmr);
    setup_Poti();
    while (1) {        
        IO_RTC_StartTime(&Tmr);
        if(CAN1.ReceivedMessageCountGet() > 0) {
            if (CAN1.Receive(&RxFrm)) {
                if (RxFrm.msgId == 0x391) {
                    uint16_t SoC_raw = RxFrm.data[0];
                    SoC = linear_map(
                        0U, 
                        255U, 
                        0U, 
                        10000U, 
                        SoC_raw);
                    
                    IO_RTC_StartTime(&TiOutTmr);
                }
                
//                TxFrm.data[0] = i;
                TxFrm.data[1] = SoC & 0xFF;
                TxFrm.data[2] = (SoC >> 8) & 0xFF;
                CAN1.Transmit(CAN1_TXQ, &TxFrm);
            }
        }
        
//        if (IO_RTC_GetTimeUS(Tmr) >= 1000000) {
//            CAN1.Transmit(CAN1_TXQ, &TxFrm);
//            IO_RTC_StartTime(&Tmr);
//        }
        
        /* reset SoC on timeout - after 60s */
        if (IO_RTC_GetTimeUS(TiOutTmr) >= 60000000) {
            SoC = 0U;
        }
        
        calculate_P(&P0, &P1, SoC);
        transmit_P(P0, P1);
        
        while (IO_RTC_GetTimeUS(Tmr) < 1000000) {
            /* do nothing here */
        }   
    }
    
    return;
}
