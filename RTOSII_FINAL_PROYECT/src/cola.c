/*=====[cola.c]======================================================
 * Copyright 2020 Authors:
 * Felipe Alberto Calcavecchia  <facalcavec@gmail.com>
 * Fabiola de las Casas Escardó <fabioladelascasas@gmail.com>
 * Alejandro Moreno 			<ale.moreno991@gmail.com>
 *
 * All rights reserved.
 * License: license text or at least name and link
         (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 0.0.3
 * Creation Date: 2020/11/26
 */

/*=====[Inclusion of own header]=============================================*/
#include "cola.h"

/*=====[Inclusions of private function dependencies]=========================*/
#include <string.h>
#include <stdbool.h>
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "qmpool.h"

/*=====[Definition macros of private constants]==============================*/

#define MAX_QUEUE_LENGHT 20

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of Interrupt functions]====================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

/**
 * @brief Constructor de las colas
 *
 * @param self Manejador de las colas (TDA)
 *
 */
void Cola_Create( cola_t *self )
{
	self->queue = xQueueCreate( MAX_QUEUE_LENGHT ,sizeof( char * )  );
	configASSERT( self->queue != NULL ); /* gestion de errores */
}

/**
 * @brief Permite pispear un dato de la cola, sin desencolarlo.
 *
 * @param self Manejador de las colas (TDA)
 * @param data Es un puntero al bloque de memoria
 */
void Cola_Peek( cola_t *self, tMensaje *data )
{
	xQueuePeek( self->queue, data, portMAX_DELAY );
}

/**
 * @brief Permite desencolar un dato de la cola, sin liberar la memoria.
 *
 * @param self Manejador de las colas (TDA)
 * @param data Es un puntero al bloque de memoria
 */
void Cola_Pop( cola_t *self, tMensaje *data )
{
	xQueueReceive( self->queue, data, portMAX_DELAY );
}



/**
 * @brief Permite desencolar un dato de la cola, sin liberar la memoria.
 *
 * @param self Manejador de las colas (TDA)
 * @param data Es un puntero al bloque de memoria
 */
bool Cola_PopFromISR( cola_t *self, tMensaje *data )
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	BaseType_t rv = xQueueReceiveFromISR( self->queue, data,
											&xHigherPriorityTaskWoken );

	return (bool) rv;
}

/**
 * @brief Permite encolar un dato, que esta albergado en un bloque del pool
 * de memoria.
 *
 * @param self Manejador de las colas (TDA)
 * @param data Es un puntero al bloque de memoria
 */
void Cola_Post( cola_t *self, const tMensaje *data)
{
	xQueueSend( self->queue, data, portMAX_DELAY );
}

/**
 * @brief Permite encolar un dato, que esta albergado en un bloque del pool
 * de memoria, desde una ISR
 *
 * @param self Manejador de las colas (TDA)
 * @param data Es un puntero al bloque de memoria
 */
bool Cola_PostFromISR( cola_t *self, const tMensaje *data)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendFromISR( self->queue, data, &xHigherPriorityTaskWoken );
}


/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/
