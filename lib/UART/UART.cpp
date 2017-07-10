/*
 * UART.cpp
 *
 * Created: 25/05/2017 16:45:37
 * Author: joaob
 */ 

#include "UART.h"

void UART0_Init(uint32_t baud, uint8_t double_speed) {
	UBRR0 = (uint16_t)(F_CPU / ((double_speed ? 8 : 16) * baud) - 1);
	
	if(double_speed) {
		UCSR0A = (1 << U2X0);
	}
	
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
uint8_t UART0_Read_Byte() {
	while(!(UCSR0A & (1 << RXC0)));
	
	return UDR0;
}
void UART0_Read(uint8_t* dst, uint8_t count) {
	for(uint8_t i = 0; i < count; i++) {
		*(dst + i) = UART0_Read_Byte();
	}
}
void UART0_Write_Byte(uint8_t byte) {
	while(!(UCSR0A & (1 << UDRE0)));
	
	UDR0 = byte;
}
void UART0_Write(uint8_t* src, uint8_t count) {
	for(uint8_t i = 0; i < count; i++) {
		UART0_Write_Byte(*(src + i));
	}
}


void UART1_Init(uint32_t baud, uint8_t double_speed) {
	UBRR1 = (uint16_t)(F_CPU / ((double_speed ? 8 : 16) * baud) - 1);
	
	if(double_speed) {
		UCSR1A = (1 << U2X1);
	}
	
	UCSR1B = (1 << TXEN1) | (1 << RXEN1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}
uint8_t UART1_Read_Byte() {
	while(!(UCSR1A & (1 << RXC1)));
	
	return UDR1;
}
void UART1_Read(uint8_t* dst, uint8_t count) {
	for(uint8_t i = 0; i < count; i++) {
		*(dst + i) = UART1_Read_Byte();
	}
}
void UART1_Write_Byte(uint8_t byte) {
	while(!(UCSR1A & (1 << UDRE1)));
	
	UDR1 = byte;
}
void UART1_Write(uint8_t* src, uint8_t count) {
	for(uint8_t i = 0; i < count; i++) {
		UART1_Write_Byte(*(src + i));
	}
}