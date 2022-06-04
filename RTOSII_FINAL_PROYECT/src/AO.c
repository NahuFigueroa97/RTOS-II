/*=====[AO.c]======================================================
 * Copyright 2020 Authors:
 * Felipe Alberto Calcavecchia  <facalcavec@gmail.com>
 * Fabiola de las Casas EscardÃƒÂ³ <fabioladelascasas@gmail.com>
 * Alejandro Moreno 			<ale.moreno991@gmail.com>
 *
 * All rights reserved.
 * License: license text or at least name and link
         (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 1.0.0
 * Creation Date: 2020/12/02
 */

/*=====[Inclusion of own header]=============================================*/
#include "AO.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/


/*=====[Prototypes (declarations) of private functions]====================*/


/*=====[Prototypes (declarations) of public functions]======================*/

bool_t activeObjectCreate( activeObject_t* ao, callbackAO_t callback, TaskFunction_t taskForAO )
{
    BaseType_t retValue = pdFALSE;

    // Asignamos la tarea al objeto activo.
    ao->taskName = taskForAO;

    // Creamos la cola asociada a este objeto activo.
    ao->activeObjectQueue = xQueueCreate( nAO , sizeof( activeObjectEvent_t ) );

    if( ao->activeObjectQueue != NULL )
    {
        // Asignamos el callback al objeto activo.
        ao->callbackFunc = callback;

        // Creamos la tarea asociada al objeto activo. A la tarea se le pasará el objeto activo como parámetro.
        retValue = xTaskCreate( ao->taskName, ( const char * )"Task For AO", configMINIMAL_STACK_SIZE*1, ao, tskIDLE_PRIORITY+2, NULL );
    }

    // Chequeamos si la tarea se creó correctamente o no.
    if( retValue == pdPASS )
    {
        // Cargamos en la variable de estado del objeto activo el valor "true" para indicar que se ha creado.
        ao->exist = TRUE;
        //printf( "devuelve un TRUE\r\n");
        return( TRUE );
    }
    else
    {
    	configASSERT( retValue == pdPASS ); 	// Gestión de errores
    }
}

void activeObjectTask( void* pvParameters )
{
    BaseType_t ret;

    // Una variable local para almacenar el dato desde la cola.
    activeObjectEvent_t  auxValue;

    // Obtenemos el puntero al objeto activo.
    activeObject_t* actObj = ( activeObject_t* ) pvParameters;

    // Cuando hay un evento, lo procesamos.
    while( TRUE )
    {
        // Verifico si hay elementos para procesar en la cola.
        if( uxQueueMessagesWaiting( actObj->activeObjectQueue )!=0 )
        {
        	ret = xQueueReceive( actObj->activeObjectQueue, &auxValue, portMAX_DELAY );

            // Si la lectura fue exitosa, proceso el dato.
            if( ret )
            {
                // Llamamos al callback correspondiente en base al comando que se le pasó.
                ( actObj->callbackFunc )( &auxValue );
            }
        }

        // Caso contrario, la cola está vacía, lo que significa que debo eliminar la tarea.
        else
        {
            actObj->exist = FALSE;

            vQueueDelete( actObj->activeObjectQueue );

            vTaskDelete( NULL );

        }
    }
}


/*=====[Implementations of private functions]================================*/


