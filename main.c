/* ###################################################################
**     Filename    : main.c
**     Project     : EE128_Project
**     Processor   : MK64FN1M0VLL12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2025-03-08, 22:52, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Pins1.h"
#include "CsIO1.h"
#include "IO1.h"
#include "SM1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "MK64F12.h"
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */

int current_motor = 0;
int array_index = 0;
int array_size = 100;
int game[100] = {
    0, 3, 2, 1, 0, 2, 1, 3, 1, 0, 3, 2, 1, 3, 0, 1, 3, 1, 2, 0,
    1, 2, 3, 0, 2, 0, 1, 3, 2, 1, 0, 2, 0, 3, 2, 1, 2, 1, 3, 0,
    3, 0, 1, 2, 1, 3, 2, 0, 1, 2, 3, 1, 2, 0, 1, 3, 0, 2, 0, 1,
    3, 0, 2, 3, 0, 1, 3, 2, 0, 3, 1, 0, 3, 2, 0, 1, 3, 0, 3, 2,
    0, 2, 1, 3, 2, 3, 0, 1, 3, 1, 2, 0, 1, 2, 0, 3, 0, 3, 1, 2
};
int score = 0;

void software_delay(unsigned long delay)
{
    while (delay > 0) delay--;
}

void motor_up(int index) {
	int phases[8] = {0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100, 0b1000, 0b1001};
	int i = 0;
	for (int count = 0; count < 1024; count++) {
		software_delay(0xBB8);
		if (index == 0) {
			GPIOD_PDOR = ((GPIOD_PDOR & 0x0F) | (phases[i] << 4));
		}
		else if (index == 1) {
			GPIOC_PDOR = ((GPIOC_PDOR & 0xFF0) | (phases[i]));
		}
		else if (index == 2) {
			GPIOC_PDOR = ((GPIOC_PDOR & 0x0FF) | (phases[i] << 8));
		}
		else if (index == 3) {
			GPIOB_PDOR = ((GPIOB_PDOR & 0x3F3) | ((phases[i] & 0xC) << 8) | ((phases[i] & 0x3) << 2));
		}
		i++;
		if (i > 7) {
			i = 0;
		}
	}
}

void motor_down(int index) {
	int phases[8] = {0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100, 0b1000, 0b1001};
	int i = 7;
	for (int count = 0; count < 1024; count++) {
		software_delay(0xBB8);
		if (index == 0) {
			GPIOD_PDOR = ((GPIOD_PDOR & 0x0F) | (phases[i] << 4));
		}
		else if (index == 1) {
			GPIOC_PDOR = ((GPIOC_PDOR & 0xFF0) | (phases[i]));
		}
		else if (index == 2) {
			GPIOC_PDOR = ((GPIOC_PDOR & 0x0FF) | (phases[i] << 8));
		}
		else if (index == 3) {
			GPIOB_PDOR = ((GPIOB_PDOR & 0x3F3) | ((phases[i] & 0xC) << 8) | ((phases[i] & 0x3) << 2));
		}
		i--;
		if (i < 0) {
			i = 7;
		}
	}
}

unsigned char write[512];

int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  uint32_t delay;
  int len;
  LDD_TDeviceData *SM1_DeviceData;
  SM1_DeviceData = SM1_Init(NULL);

  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
  SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;

  PORTA_PCR1 = 0x100; // Configure Pin 1 for GPIO
  PORTA_PCR2 = 0x100; // Configure Pin 2 for GPIO

  PORTB_PCR2 = 0x100; // Configure Pin 2 for GPIO
  PORTB_PCR3 = 0x100; // Configure Pin 3 for GPIO
  PORTB_PCR10 = 0x100; // Configure Pin 10 for GPIO
  PORTB_PCR11 = 0x100; // Configure Pin 11 for GPIO

  PORTC_PCR0 = 0x100; // Configure Pin 0 for GPIO
  PORTC_PCR1 = 0x100; // Configure Pin 1 for GPIO
  PORTC_PCR2 = 0x100; // Configure Pin 2 for GPIO
  PORTC_PCR3 = 0x100; // Configure Pin 3 for GPIO
  PORTC_PCR4 = 0x100; // Configure Pin 4 for GPIO
  PORTC_PCR8 = 0x100; // Configure Pin 8 for GPIO
  PORTC_PCR9 = 0x100; // Configure Pin 9 for GPIO
  PORTC_PCR10 = 0x100; // Configure Pin 10 for GPIO
  PORTC_PCR11 = 0x100; // Configure Pin 11 for GPIO
  PORTC_PCR16 = 0x100; // Configure Pin 16 for GPIO
  PORTC_PCR17 = 0x100; // Configure Pin 17 for GPIO
  PORTC_PCR18 = 0x100; // Configure Pin 18 for GPIO

  PORTD_PCR4 = 0x100; // Configure Pin 4 for GPIO
  PORTD_PCR5 = 0x100; // Configure Pin 5 for GPIO
  PORTD_PCR6 = 0x100; // Configure Pin 6 for GPIO
  PORTD_PCR7 = 0x100; // Configure Pin 7 for GPIO

  // Set Port A Pins 1-2 as Input
  GPIOA_PDDR &= ~(1 << 1);
  GPIOA_PDDR &= ~(1 << 2);

  // Set Port B Pins 2-3, 10-11 as Output
  GPIOB_PDDR |= 0xC0C;

  // Set Port C Pins 0-3, 8-11 as Output
  GPIOC_PDDR |= 0xF1F;

  // Set Port C Pins 16-18 as Input
  GPIOC_PDDR &= ~(1 << 16);
  GPIOC_PDDR &= ~(1 << 17);
  GPIOC_PDDR &= ~(1 << 18);

  // Set Port D Pins 4-7 as Output
  GPIOD_PDDR |= 0xF0;

  // Start the game and put the first motor up.
  motor_up(game[array_index]);

  // Set current motor to first element in the array.
  current_motor = game[array_index];

  for (;;) {
      // UART and SPI
      printf("Score \t: %4d\n", score);
      len = sprintf(write, "Score \t: %4d\n", score);
      GPIOC_PDOR |= 0x10;
      SM1_SendBlock(SM1_DeviceData, &write, len);
      for(delay = 0; delay < 300000; delay++); //delay
      GPIOC_PDOR &= 0xEF;

      // Read buttons
      int button_0 = (GPIOC_PDIR & (1 << 17));
      int button_1 = (GPIOA_PDIR & (1 << 1));
      int button_2 = (GPIOA_PDIR & (1 << 2));
      int button_3 = (GPIOC_PDIR & (1 << 16));
      int button_end = (GPIOC_PDIR & (1 << 18));

      if ((current_motor == 0) && (button_0 != 0)) {
         score++;
         motor_down(0);
         array_index++;
         if (array_index >= array_size) {
            array_index = 0;
         }
         if (game[array_index] == 1) {
            motor_up(1);
            current_motor = 1;
         }
         else if (game[array_index] == 2) {
            motor_up(2);
            current_motor = 2;
         }
         else if (game[array_index] == 3) {
            motor_up(3);
            current_motor = 3;
         }
      }

      else if ((current_motor == 1) && (button_1 != 0)) {
         score++;
         motor_down(1);
         array_index++;
         if (array_index >= array_size) {
            array_index = 0;
         }
         if (game[array_index] == 0) {
            motor_up(0);
            current_motor = 0;
         }
         else if (game[array_index] == 2) {
            motor_up(2);
            current_motor = 2;
         }
         else if (game[array_index] == 3) {
            motor_up(3);
            current_motor = 3;
         }
      }

      else if ((current_motor == 2) && (button_2 != 0)) {
         score++;
         motor_down(2);
         array_index++;
         if (array_index >= array_size) {
            array_index = 0;
         }
         if (game[array_index] == 0) {
            motor_up(0);
            current_motor = 0;
         }
         else if (game[array_index] == 1) {
            motor_up(1);
            current_motor = 1;
         }
         else if (game[array_index] == 3) {
            motor_up(3);
            current_motor = 3;
         }
      }

      else if ((current_motor == 3) && (button_3 != 0)) {
         score++;
         motor_down(3);
         array_index++;
         if (array_index >= array_size) {
            array_index = 0;
         }
         if (game[array_index] == 0) {
            motor_up(0);
            current_motor = 0;
         }
         else if (game[array_index] == 1) {
            motor_up(1);
            current_motor = 1;
         }
         else if (game[array_index] == 2) {
            motor_up(2);
            current_motor = 2;
         }
      }

      else if (button_end != 0) {
         motor_down(current_motor);
         score = 0;
         array_index = 0;
         break;
      }

   }
  /* For example: for(;;) { } */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
