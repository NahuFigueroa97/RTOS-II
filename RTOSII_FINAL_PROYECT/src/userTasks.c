/*=============================================================================
 * Copyright (c) 2022, Nahuel Figueroa <nahuu810@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2022/06/07
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "userTasks.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

// Task implementation
void myTask(void *taskParmPtr)
{
   // ----- Task setup -----------------------------------
   // printf( "Blinky with freeRTOS y sAPI.\r\n" );

   gpioWrite(LED, ON);

   // Send the task to the locked state for 1 s (delay)
   vTaskDelay(1000 / portTICK_RATE_MS);

   gpioWrite(LED, OFF);

   // Periodic task every 500 ms
   portTickType xPeriodicity = 500 / portTICK_RATE_MS;
   portTickType xLastWakeTime = xTaskGetTickCount();

   // ----- Task repeat for ever -------------------------
   while (TRUE)
   {
      gpioToggle(LED);

      // Send the task to the locked state during xPeriodicity
      vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
   }
}

/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/
