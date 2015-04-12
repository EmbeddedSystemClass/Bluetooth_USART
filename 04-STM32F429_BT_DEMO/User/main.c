/**
 * @Title: Bluetooth Activity Demo Project
 *
 *	@Summary: Demo Project based on STM32F429 Disovery Board
 * 
 * 				-Demo includes Leds, TouchScreen, Servos, Distance
 * 				-Sensor, Bluetooth, and later GPS
 * 
 * @By ERIC YU
 * @
 *
 *
 */
 
 
/* Include core modules */
#include "stm32f4xx.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_ili9341.h"
#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_hcsr04.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_servo.h"
#include <stdio.h>


#define DISTANCE_SENSOR 		1
#define SERVO_CONTROL			2
#define LED_CONTROL				3
#define IRQ_ON						1
#define IRQ_OFF					0

int globalKey = 0;
int globalCntLED = 0;
int globalCntSERVO = 0;
int globalIRQ;
char globalCharCount;
char buffer[100];
TM_SERVO_t Servo1;

void globalKeyFunctions(int keyIdx);
void TM_USART1_ReceiveHandler(uint8_t c);
void undrawText();

void InitializeModules(){

		SystemInit();
		
		TM_ILI9341_Init();
		TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
		TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
		
		TM_DISCO_LedInit();
		
		TM_DELAY_Init();
		
		/* Initialize USART1 at 9600 baud, TX: PB6, RX: PB7 */
		TM_USART_Init(USART1, TM_USART_PinsPack_2, 9600);
		
		TM_SERVO_Init(&Servo1, TIM2, TM_PWM_Channel_1, TM_PWM_PinsPack_2);
}

void distanceSensorFunction(){
		float distance;
		char distanceStr[10];
		if(!TM_HCSR04_Init()) {
				/* Sensor is not ON */
				while(globalKey == DISTANCE_SENSOR){
					TM_DISCO_LedToggle(LED_RED);
					Delayms(100);
				}
		}
		TM_ILI9341_Puts(65, 130, "Distance Sensing", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
		while(globalKey == DISTANCE_SENSOR){
			distance = TM_HCSR04_Read();
			sprintf(distanceStr, "%f", distance);
			TM_ILI9341_Puts(65, 180, distanceStr, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
		}
}

void servoControl(){
		while(globalCntSERVO == 1){
				TM_SERVO_SetDegrees(&Servo1, 0);
				Delayms(2000);
				TM_SERVO_SetDegrees(&Servo1, 90);
				Delayms(2000);
				TM_SERVO_SetDegrees(&Servo1, 180);
				Delayms(2000);
		}
}

void ledControl(){
		
		while(globalIRQ == IRQ_ON){
			if(globalCntLED == 1){
				TM_DISCO_LedOn(LED_ALL);
				TM_USART_Puts(USART1, "ON");
				//TM_ILI9341_Puts(10, 100, "LN", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
			}else if(globalCntLED == 0){
				TM_DISCO_LedOff(LED_ALL);
				TM_USART_Puts(USART1, "FF");
				//TM_ILI9341_Puts(10, 100, "LF", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
			}
			globalIRQ = IRQ_OFF;
		}
		Delayms(500);
}


int main(void) {
		
		char usartChar;
		char *string;
		int count;
		
		
		/* Initialize system */
		InitializeModules();
	
		TM_ILI9341_Puts(65, 100, "Welcome!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
		TM_ILI9341_Puts(65, 150, "Select an Option", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);

		while (1) {
				
				//Functions to Run based on globalKey
				globalKeyFunctions(globalKey);
				Delayms(100);
				
		}
}

void globalKeyFunctions(int keyIdx){
		
		switch(keyIdx){
			case DISTANCE_SENSOR:
				distanceSensorFunction();
				break;
			case SERVO_CONTROL: 
				servoControl();
				break;
			case LED_CONTROL:
				ledControl();
				break;
			case 0:
			default:
				break;
		}
		
}
void undrawText(int undrawIdx){
	switch(undrawIdx){
		case DISTANCE_SENSOR:
			TM_ILI9341_Puts(65, 100, "Welcome!", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			TM_ILI9341_Puts(65, 150, "Select an Option", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			TM_ILI9341_Puts(65, 130, "Servo Control", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			TM_ILI9341_Puts(65, 130, "Led Control", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);

			break;
		case SERVO_CONTROL:
			TM_ILI9341_Puts(65, 100, "Welcome!", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			TM_ILI9341_Puts(65, 150, "Select an Option", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			TM_ILI9341_Puts(65, 130, "Distance Sensing", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			TM_ILI9341_Puts(65, 130, "Led Control", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);

			break;
		case LED_CONTROL:
			TM_ILI9341_Puts(65, 100, "Welcome!", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			TM_ILI9341_Puts(65, 130, "Distance Sensing", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			TM_ILI9341_Puts(65, 130, "Servo Control", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			TM_ILI9341_Puts(65, 150, "Select an Option", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			break;
		case 0:
		default:{
			TM_ILI9341_Puts(65, 130, "Servo Control", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			TM_ILI9341_Puts(65, 130, "Distance Sensing", &TM_Font_11x18, ILI9341_COLOR_MAGENTA, ILI9341_COLOR_MAGENTA);
			TM_ILI9341_Puts(65, 150, "Select an Option", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
			break;
		}
	}
}

void TM_USART1_ReceiveHandler(uint8_t c){

		if(c == 'a' ){
			//DISTANCE SENSOR
			globalKey = 1;
			globalIRQ = IRQ_ON;
			undrawText(globalKey);
			TM_USART_ClearBuffer(USART1);
		}
		if(c == 'b' ){
			//SERVO CONTROL
			globalKey = 2;
			if(globalCntSERVO == 0){
				globalCntSERVO = 1;
			}else{
				globalCntSERVO = 0;
			}
			undrawText(globalKey);
			TM_ILI9341_Puts(65, 130, "Servo Control", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
			TM_USART_ClearBuffer(USART1);
		}
		if(c == 'c' ){
			//LED Control
			globalKey = 3;
			globalIRQ = IRQ_ON;
			undrawText(globalKey);
			TM_ILI9341_Puts(65, 130, "Led Control", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
			if(globalCntLED == 0){
				globalCntLED = 1;
			}else{
				globalCntLED = 0;
			}
			TM_USART_ClearBuffer(USART1);
		}
		if(c == 'd' ){
			globalKey = 4;
			TM_USART_ClearBuffer(USART1);
		}
		if(c == 'e'){
			globalKey = 0;
			undrawText(globalKey);
			TM_USART_ClearBuffer(USART1);
		}
	
}



