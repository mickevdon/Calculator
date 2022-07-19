// FlashProgram.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize the flash memory, write
// 32-bit data to flash, write an array of 32-bit data to flash,
// and erase a 1 KB block.
// Daniel Valvano
// May 3, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2016
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2016

 Copyright 2017 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include <stdint.h>
#include "FlashProgram.h"

#define FLASH_FMA_R             (*((volatile uint32_t *)0x400FD000))
#define FLASH_FMA_OFFSET_MAX    0x0003FFFF  // Address Offset max
#define FLASH_FMD_R             (*((volatile uint32_t *)0x400FD004))
#define FLASH_FMC_R             (*((volatile uint32_t *)0x400FD008))
#define FLASH_FMC_WRKEY         0xA4420000  // FLASH write key (KEY bit of FLASH_BOOTCFG_R set)
#define FLASH_FMC_WRKEY2        0x71D50000  // FLASH write key (KEY bit of FLASH_BOOTCFG_R cleared)
#define FLASH_FMC_MERASE        0x00000004  // Mass Erase Flash Memory
#define FLASH_FMC_ERASE         0x00000002  // Erase a Page of Flash Memory
#define FLASH_FMC_WRITE         0x00000001  // Write a Word into Flash Memory
#define FLASH_FMC2_R            (*((volatile uint32_t *)0x400FD020))
#define FLASH_FMC2_WRBUF        0x00000001  // Buffered Flash Memory Write
#define FLASH_FWBN_R            (*((volatile uint32_t *)0x400FD100))
#define FLASH_BOOTCFG_R         (*((volatile uint32_t *)0x400FE1D0))
#define FLASH_BOOTCFG_KEY       0x00000010  // KEY Select

long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

// Check if address offset is valid for write operation
// Writing addresses must be 4-byte aligned and within range
static int WriteAddrValid(uint32_t addr){
  // check if address offset works for writing
  // must be 4-byte aligned
  return (((addr % 4) == 0) && (addr <= FLASH_FMA_OFFSET_MAX));
}

// Check if address offset is valid for erase operation
// Erasing addresses must be 1 KB aligned and within range
static int EraseAddrValid(uint32_t addr){
  // check if address offset works for erasing
  // must be 1 KB aligned
  return (((addr % 1024) == 0) && (addr <= FLASH_FMA_OFFSET_MAX));
}

//------------Flash_Init------------
// This function was critical to the write and erase
// operations of the flash memory on the LM3S811
// microcontroller.  But newer processors work slightly
// differently, and for the TM4C123 the timing parameters
// for the flash and EEPROM memories are configured along
// with the PLL.  This function prototype is preserved to
// try to make it easier to reuse program code between the
// LM3S811, TM4C123, and TM4C1294.
// Input: systemClockFreqMHz  system clock frequency (units of MHz)
// Output: none
void Flash_Init(uint8_t systemClockFreqMHz){
  // do nothing; flash and EEPROM memory configured in PLL_Init()
  // if the processor is executing code out of flash memory,
  // presumably everything is configured correctly
}

//------------Flash_Write------------
// Write 32-bit data to flash at given address.
// Input: addr 4-byte aligned flash memory address to write
//        data 32-bit data
// Output: 'NOERROR' if successful, 'ERROR' if fail (defined in FlashProgram.h)
// Note: disables interrupts while writing
int Flash_Write(uint32_t addr, uint32_t data){
  uint32_t flashkey;
  if(WriteAddrValid(addr)){
    //DisableInterrupts();                            // may be optional step
                                                    // wait for hardware idle
    while(FLASH_FMC_R&(FLASH_FMC_WRITE|FLASH_FMC_ERASE|FLASH_FMC_MERASE)){
                 // to do later: return ERROR if this takes too long
                 // remember to re-enable interrupts
    };
    FLASH_FMD_R = data;
    FLASH_FMA_R = addr;
    if(FLASH_BOOTCFG_R&FLASH_BOOTCFG_KEY){          // by default, the key is 0xA442
      flashkey = FLASH_FMC_WRKEY;
    } else{                                         // otherwise, the key is 0x71D5
      flashkey = FLASH_FMC_WRKEY2;
    }
    FLASH_FMC_R = (flashkey|FLASH_FMC_WRITE);       // start writing
    while(FLASH_FMC_R&FLASH_FMC_WRITE){
                 // to do later: return ERROR if this takes too long
                 // remember to re-enable interrupts
    };           // wait for completion (~3 to 4 usec)
    return NOERROR;
  }
  return ERROR;
}

//------------Flash_Erase------------
// Erase 1 KB block of flash.
// Input: addr 1-KB aligned flash memory address to erase
// Output: 'NOERROR' if successful, 'ERROR' if fail (defined in FlashProgram.h)
// Note: disables interrupts while erasing
int Flash_Erase(uint32_t addr){
  uint32_t flashkey;
  if(EraseAddrValid(addr)){
    //DisableInterrupts();                            // may be optional step
                                                    // wait for hardware idle
    while(FLASH_FMC_R&(FLASH_FMC_WRITE|FLASH_FMC_ERASE|FLASH_FMC_MERASE)){
                 // to do later: return ERROR if this takes too long
                 // remember to re-enable interrupts
    };
    FLASH_FMA_R = addr;
    if(FLASH_BOOTCFG_R&FLASH_BOOTCFG_KEY){          // by default, the key is 0xA442
      flashkey = FLASH_FMC_WRKEY;
    } else{                                         // otherwise, the key is 0x71D5
      flashkey = FLASH_FMC_WRKEY2;
    }
    FLASH_FMC_R = (flashkey|FLASH_FMC_ERASE);       // start erasing 1 KB block
    while(FLASH_FMC_R&FLASH_FMC_ERASE){
                 // to do later: return ERROR if this takes too long
                 // remember to re-enable interrupts
    };           // wait for completion (~3 to 4 usec)
    //EnableInterrupts();
    return NOERROR;
  }
  return ERROR;
}
