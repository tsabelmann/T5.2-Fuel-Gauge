/**
  * SPI2 Generated Driver File
  *
  * @file spi2.c
  *
  * @ingroup spi2
  *
  * @brief Contains the API Implementations for SPI2 module driver.
  *
  * @version SPI2 Driver Version 2.0.1
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include "../spi2.h"
#include <xc.h>

/**
 * @ingroup spi2
 * @struct SPI_INTERFACE SPI2
 * @brief Defines an object for SPI_DRIVER_FUNCTIONS.
 */ 
const struct SPI_INTERFACE SPI2 = {
    .Initialize = SPI2_Initialize,
    .Close = SPI2_Close,
    .Open = SPI2_Open,
    .BufferExchange = SPI2_BufferExchange,
    .BufferRead = SPI2_BufferRead,
    .BufferWrite = SPI2_BufferWrite,	
    .ByteExchange = SPI2_ByteExchange,
    .ByteRead = SPI2_ByteRead,	
    .ByteWrite = SPI2_ByteWrite,
};

typedef struct 
{ 
    uint8_t con0; 
    uint8_t con1; 
    uint8_t con2; 
    uint8_t baud; 
    uint8_t operation;
} spi2_configuration_t;


//con0 == SPIxCON0, con1 == SPIxCON1, con2 == SPIxCON2, baud == SPIxBAUD, operation == Host/Client
static const spi2_configuration_t spi2_configuration[] = 
{   
    { 0x2, 0x40, 0x0, 0x3F, 0 }
};

void SPI2_Initialize(void)
{
    //SDOP active high; SDIP active high; SSP active high; FST disabled; CKP Idle:Low, Active:High; CKE Active to idle; SMP Middle; 
    SPI2CON1 = 0x40;
    //RXR data is not stored in the FIFO; TXR not required for a transfer; SSET disabled; 
    SPI2CON2 = 0x0;
    //CLKSEL FOSC; 
    SPI2CLK = 0x0;
    //BAUD 63; 
    SPI2BAUD = 0x3F;
    TRISBbits.TRISB3 = 0;
    //BMODE last byte; MST bus host; LSBF MSb first; EN disabled; 
    SPI2CON0 = 0x2;
}

bool SPI2_Open(uint8_t spiConfigIndex)
{   
    if(!SPI2CON0bits.EN)
    {
        SPI2CON0 = spi2_configuration[spiConfigIndex].con0;
        SPI2CON1 = spi2_configuration[spiConfigIndex].con1;
        SPI2CON2 = spi2_configuration[spiConfigIndex].con2 | (_SPI2CON2_SPI2RXR_MASK | _SPI2CON2_SPI2TXR_MASK);
        SPI2CLK  = 0x00;
        SPI2BAUD = spi2_configuration[spiConfigIndex].baud;        
        TRISBbits.TRISB3 = spi2_configuration[spiConfigIndex].operation;
        SPI2CON0bits.EN = 1;
        return true;
    }
    return false;
}

void SPI2_Close(void)
{
    SPI2CON0bits.EN = 0;
}

uint8_t SPI2_ByteExchange(uint8_t data)
{
    SPI2TCNTL = 1;
    SPI2TXB = data;
    while(!PIR5bits.SPI2RXIF);
    return SPI2RXB;
}

void SPI2_BufferExchange(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        SPI2TCNTL = 1;
        SPI2TXB = *data;
        while(!PIR5bits.SPI2RXIF);
        *data++ = SPI2RXB;
    }
}

// Half Duplex SPI Functions
void SPI2_BufferWrite(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        SPI2_ByteExchange(*data++);
    }
}

void SPI2_BufferRead(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        *data++ = SPI2_ByteExchange(0);
    }
}

void SPI2_ByteWrite(uint8_t byte)
{
    SPI2TXB = byte;
}

uint8_t SPI2_ByteRead(void)
{
    return SPI2RXB;
}

uint8_t SPI2_ExchangeByte(uint8_t data)
{
    return SPI2_ByteExchange(data);
}

void SPI2_ExchangeBlock(void *block, size_t blockSize)
{
    SPI2_BufferExchange(block, blockSize);
}

void SPI2_WriteBlock(void *block, size_t blockSize)
{
    SPI2_BufferWrite(block, blockSize);
}

void SPI2_ReadBlock(void *block, size_t blockSize)
{
    SPI2_BufferRead(block, blockSize);
}

void SPI2_WriteByte(uint8_t byte)
{
    SPI2_ByteWrite(byte);
}

uint8_t SPI2_ReadByte(void)
{
    return SPI2_ByteRead();
}
