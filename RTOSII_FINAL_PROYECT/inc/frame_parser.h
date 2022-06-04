/*=====[frame_parser.h]======================================================
 * Copyright 2020 Authors:
 * Felipe Alberto Calcavecchia  <facalcavec@gmail.com>
 * Fabiola de las Casas Escardó <fabioladelascasas@gmail.com>
 * Alejandro Moreno 			<ale.moreno991@gmail.com>
 *
 * All rights reserved.
 * License: license text or at least name and link
         (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 0.0.2
 * Creation Date: 2020/11/19
 */

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef FRAME_PARSER_H
#define FRAME_PARSER_H

/*=====[Inclusions of public function dependencies]==========================*/
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "sapi.h"
#include "cola.h"
#include "timers.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

#define SEED 0

#define MSG_ERROR_CRC "(ERROR36B)"

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

typedef struct {

	/**
	 * @brief Utilizado para sincronizar la recepción de paquetes
	 */
	SemaphoreHandle_t new_frame_signal;

} frame_parser_t;

/*=====[Prototypes (declarations) of public functions]=======================*/

void FrameParser_Create( frame_parser_t *self );

bool FrameParser_CatchMsg( char c, uint32_t *bytes_received, tMensaje message );

void FrameParser_RunDecoding( void *self );

void FrameParser_RunEncoding( void *self );

uint8_t crc8_calc(uint8_t val, void *buf, int cnt);

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* FRAME_PARSER_H */
