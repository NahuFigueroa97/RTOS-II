/*=====[app.h]======================================================
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

#ifndef APP_H
#define APP_H

/*=====[Inclusions of public function dependencies]==========================*/
#include <stdint.h>

#include "FreeRTOSConfig.h"
#include "memory_manager.h"
#include "qmpool.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "sapi.h"
#include "queue.h"
#include "AO.h"

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
#define MINUSC	'm'
#define MAYUSC	'M'

#define ASCII_A 'A'
#define ASCII_Z 'Z'
#define ASCII_a 'a'
#define ASCII_z 'z'
#define ASCII_espacio ' '

#define OFFSET_PAYLOAD 2
#define BYTES_OMITTED  3

#define TASK_APP 		"TAREA (aplicacion)"
#define MSG_ERROR_CARACTER	"(ERROR111)"
#define MSG_ERROR_C		"(ERROR222)"

/*=====[Public function-like macros]=========================================*/


/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public functions]=======================*/

void Minusc(activeObjectEvent_t *AOdata);

void Mayusc(activeObjectEvent_t *AOdata);

void App_Convert( void* );


/*=====[Prototypes (declarations) of public interrupt functions]=============*/


/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* _APP_H */
