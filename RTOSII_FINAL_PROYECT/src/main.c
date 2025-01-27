/*=============================================================================
 * Copyright (c) 2022, Nahuel Figueroa <nahuu810@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2022/06/07
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "main.h"
#include "sapi.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "userTasks.h"
#include "wrapper.h"
#include "appConfig.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

int main(void)
{
    boardInit();

    static config_t config = {
        .uart = UART_USB,
        .baud = 115200,
        .index = 0};
    initWrapper(&config);

    // static config_t config2 = {
    //     .uart = UART_GPIO,
    //     .baud = 115200,
    //     .index = 1};
    // initWrapper(&config2);

    BaseType_t res;

    // Create a task in freeRTOS with dynamic memory
    res = xTaskCreate(
        myTask,                       // Function that implements the task.
        (const char *)"myTask",       // Text name for the task.
        configMINIMAL_STACK_SIZE * 1, // Stack size in words, not bytes.
        0,                            // Parameter passed into the task.
        tskIDLE_PRIORITY + 1,         // Priority at which the task is created.
        0                             // Pointer to the task created in the system
    );
    configASSERT(res == pdPASS);

    vTaskStartScheduler(); // Initialize scheduler

    while (true)
        ; // If reach heare it means that the scheduler could not start

    // YOU NEVER REACH HERE, because this program runs directly or on a
    // microcontroller and is not called by any Operating System, as in the
    // case of a PC program.
    return 0;
}
