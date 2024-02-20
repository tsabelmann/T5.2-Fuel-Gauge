/**
 * DELAY Generated Driver API Header File
 * 
 * @file delay.h
 * 
 * @defgroup delay DELAY BLOCKING DRIVER
 * 
 * @brief This file contains the API to generate delays in the millisecond and microsecond ranges.
 *
 * @version DELAY Driver Version 1.1.0
*/
/*
� [2024] Microchip Technology Inc. and its subsidiaries.

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

#ifndef _DELAY_H
#define _DELAY_H

#include <stdint.h>

/**
*  @ingroup delay
*  @brief Call this function to delay execution of the program for a certain number of milliseconds
*  @param milliseconds - number of milliseconds to delay
*/
void DELAY_milliseconds(uint16_t milliseconds);

/**
*  @ingroup delay
*  @brief Call this function to delay execution of the program for a certain number of microseconds
*  @param microseconds - number of microseconds to delay
*/
void DELAY_microseconds(uint16_t microseconds);

#endif	// _DELAY_H