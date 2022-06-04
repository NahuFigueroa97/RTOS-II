/*=====[cola.h]======================================================
 * Copyright 2020 Authors:
 * Felipe Alberto Calcavecchia  <facalcavec@gmail.com>
 * Fabiola de las Casas Escardó <fabioladelascasas@gmail.com>
 * Alejandro Moreno 			<ale.moreno991@gmail.com>
 *
 * All rights reserved.
 * License: license text or at least name and link
         (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 0.0.1
 * Creation Date: 2020/11/05
 */

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef COLA_H
#define COLA_H

/*=====[Inclusions of public function dependencies]==========================*/
#include <stdint.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "sapi.h"
#include "qmpool.h"
#include "queue.h"
#include "memory_manager.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

typedef struct {
	xQueueHandle queue;
} cola_t;

/*=====[Prototypes (declarations) of public functions]=======================*/

void Cola_Create( cola_t *self );

void Cola_Peek( cola_t *self, tMensaje *data );

void Cola_Pop( cola_t *self, tMensaje *data );

bool Cola_PopFromISR( cola_t *self, tMensaje *data );

void Cola_Post( cola_t *self, const tMensaje *data);

bool Cola_PostFromISR( cola_t *self, const tMensaje *data );


/*=====[Prototypes (declarations) of public interrupt functions]=============*/


/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* _COLA_H */
