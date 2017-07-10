/*
 * UART.h
 *
 * Created: 25/05/2017 16:45:37
 * Author : joaob
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdint.h>


extern void UART0_Init(uint32_t baud, uint8_t double_speed);
extern uint8_t UART0_Read_Byte();
extern void UART0_Read(uint8_t* dst, uint8_t count);
extern void UART0_Write_Byte(uint8_t byte);
extern void UART0_Write(uint8_t* src, uint8_t count);


extern void UART1_Init(uint32_t baud, uint8_t double_speed);
extern uint8_t UART1_Read_Byte();
extern void UART1_Read(uint8_t* dst, uint8_t count);
extern void UART1_Write_Byte(uint8_t byte);
extern void UART1_Write(uint8_t* src, uint8_t count);

#endif /* UART_H_ */