/*
 * AIODisplayBL.cpp
 *
 * Created: 30-May-17 11:56:08 AM
 * Author : João
 */ 

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdint.h>
#include "lib/UART/UART.h"

#define BOOT_SIZE 8192
#define BOOT_START (FLASHEND - BOOT_SIZE + 1)

#define UART_CMD_PING 0x01
#define UART_CMD_GET_PAGE_SIZE 0x02
#define UART_CMD_PROGRAM_PAGE 0x03
#define UART_CMD_EOF 0x04

#define UART_STATUS_OK 0x01
#define UART_STATUS_ERROR_BOOTLOADER_OVERWRITE 0xE0
#define UART_STATUS_ERROR_UNKNOWN 0xFF


void Watchdog_Reset() {
	wdt_enable(WDTO_15MS);
	while(1);
}
void App_Start() {
	SPL = (RAMEND & 0xFF);
	SPH = (RAMEND >> 8);
	
	asm("jmp 0x00000");
}
void Flash_Program_Page(uint32_t page, uint8_t *buf, uint16_t buf_size) {
	uint16_t i;
	
	buf_size = (buf_size > SPM_PAGESIZE) ? SPM_PAGESIZE : buf_size;
	
	eeprom_busy_wait();
	
	boot_page_erase(page);
	boot_spm_busy_wait();
	
	for(i = 0; i < buf_size; i += 2) {
		boot_page_fill(page + i, *(buf + i) | (*(buf + i + 1) << 8));
	}
	
	boot_page_write(page);
	boot_spm_busy_wait();
	
	boot_rww_enable();
}
void Check_UART() {
	uint8_t RX_Buffer[SPM_PAGESIZE];
	uint32_t RX_Buffer_Pos = 0;
	uint32_t Current_Page = 0;
	
	wdt_enable(WDTO_500MS);
	wdt_reset();
	
	UART0_Init(1000000, 1);
	
	while(1) {
		uint8_t ch = UART0_Read_Byte();
		
		switch(ch) {
			case UART_CMD_PING: {
				UART0_Write_Byte(UART_STATUS_OK);
				
				wdt_disable();
			}
			break;
			case UART_CMD_GET_PAGE_SIZE: {
				UART0_Write_Byte(UART_STATUS_OK);
				UART0_Write_Byte(SPM_PAGESIZE >> 8);
				UART0_Write_Byte(SPM_PAGESIZE & 0xFF);
			}
			break;
			case UART_CMD_PROGRAM_PAGE: {
				uint16_t Data_Size;
				
				Data_Size = (UART0_Read_Byte() << 8) | UART0_Read_Byte();
				Data_Size = (Data_Size > SPM_PAGESIZE) ? SPM_PAGESIZE : Data_Size;
				
				while(RX_Buffer_Pos < Data_Size) {
					RX_Buffer[RX_Buffer_Pos] = UART0_Read_Byte();
					
					RX_Buffer_Pos++;
				}
				
				if(Current_Page < BOOT_START) {
					Flash_Program_Page(Current_Page, RX_Buffer, Data_Size);
					
					Current_Page += Data_Size;
					RX_Buffer_Pos = 0;
					
					UART0_Write_Byte(UART_STATUS_OK);
				} else {
					UART0_Write_Byte(UART_STATUS_ERROR_BOOTLOADER_OVERWRITE);
					
					Watchdog_Reset();
				}
			}
			break;
			case UART_CMD_EOF: {
				UART0_Write_Byte(UART_STATUS_OK);
				
				Watchdog_Reset();
			}
			break;
		}
	}
}

int main() {
	uint8_t _MCUSR;
	
	_MCUSR = MCUSR;
	MCUSR = 0;
	
	wdt_disable();
	
	/*if(_MCUSR & (1 << WDRF)) {
		eeprom_busy_wait();
		
		if(eeprom_read_byte(0x00) == 0x3C) {
			Check_SPI_Flash();
			
			eeprom_write_byte(0x00, 0x00);
			
			wdt_enable(WDTO_15MS);
			while(1);
		}
	}*/
	
	
	if(_MCUSR & ((1 << EXTRF) | (1 << PORF))) {
		Check_UART();
	}
	
	App_Start();
}