/*=====[AO.h]======================================================
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

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef AO_H
#define AO_H

/*=====[Inclusions of public function dependencies]==========================*/
#include <stdint.h>

#include "FreeRTOSConfig.h"
#include "memory_manager.h"
#include "qmpool.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "sapi.h"
#include "queue.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "memory_manager.h"
#include "cola.h"
#include "frame_parser.h"
#include <string.h>
#include <stdbool.h>

#include "FreeRTOSConfig.h"
#include "task.h"

/*=====[Definition macros of public constants]===============================*/
#define nAO		10	// Cantidad de objectos activos

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/**
 * @brief Estructura para manejar los datos que se envian al objecto activo de mayusculizar o minusculizar
 */
typedef tMensaje activeObjectEvent_t;


typedef void (*callbackAO_t )(activeObjectEvent_t*);

/**
 * @brief estructura para manejar la creacion de los OA
 */
typedef struct {
	TaskFunction_t	taskName;
	QueueHandle_t	activeObjectQueue;		// cola para enviar mensaje a procesar de OA_app hacia Mayusc/Minusc
	callbackAO_t	callbackFunc;
	bool_t			exist;
} activeObject_t;


/*=====[Prototypes (declarations) of public functions]=======================*/
bool_t activeObjectCreate( activeObject_t* ao, callbackAO_t callback, TaskFunction_t taskForAO );

void activeObjectTask( void* pvParameters );

//void activeObjectEnqueue( activeObject_t* ao, activeObjectResponse_t* value );

/*=====[Prototypes (declarations) of public interrupt functions]=============*/


/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* _AO_H */
