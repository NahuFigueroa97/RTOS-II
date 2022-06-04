/*=====[memory_manager.h]======================================================
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

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

/*=====[Inclusions of public function dependencies]==========================*/
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOSConfig.h"
#include "qmpool.h"
#include "FreeRTOS.h"
#include "sapi.h"


/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/
#define BLOCK_SIZE  		201 * sizeof( char )			// block size
#define POOL_TOTAL_BLOCKS 	10								// number of blocks
#define POOL_SIZE 			POOL_TOTAL_BLOCKS * BLOCK_SIZE 	// pool total size

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

typedef struct {
	/**
	 * @brief Espacio de almacenamiento de memoria
	 */
	QMPool mem_pool;


} memory_manager_t;

typedef char* tMensaje;

/*=====[Prototypes (declarations) of public functions]=======================*/

void Memory_Init( memory_manager_t *self, tMensaje *mem_pool_ptr );

bool Memory_Alloc( memory_manager_t *self, tMensaje *block );

void Memory_Dealloc( memory_manager_t *self, tMensaje block );

/*=====[Prototypes (declarations) of public interrupt functions]=============*/


/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* MEMORY_MANAGER_H */