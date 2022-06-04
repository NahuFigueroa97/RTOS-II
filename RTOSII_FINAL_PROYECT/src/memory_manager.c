/*=====[memory_manager.c]======================================================
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
#include "memory_manager.h"

#include <string.h>
#include <stdbool.h>
#include "task.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of Interrupt functions]====================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

/**
 * @brief InicializaciÃ³n del pool de memoria
 *
 * @param mem_pool_ptr puntero al pool de memoria a reservar
 * @param mem_pool pool de memoria creado
 */
void Memory_Init( memory_manager_t *self, tMensaje *mem_pool_ptr)
{
    /*	Reservo memoria para el memory pool */
    *mem_pool_ptr = ( tMensaje ) pvPortMalloc( POOL_SIZE );
    configASSERT( *mem_pool_ptr != NULL ); /* gestion de errores */

	/* Creo el pool de memoria */
	QMPool_init( &self->mem_pool, ( tMensaje ) *mem_pool_ptr, POOL_SIZE, BLOCK_SIZE );
}

bool Memory_Alloc( memory_manager_t *self, tMensaje *block )
{
	bool ret = FALSE;

	if ( NULL != ( *block = (tMensaje) QMPool_get( &self->mem_pool, 5U ) ) )
	{
		ret = TRUE;
	}
	else
	{
		/* comportamiento si no hay más memoria disponible */
		ret = FALSE;
	}

	return ret;
}

void Memory_Dealloc( memory_manager_t *self, tMensaje block )
{
	/* Libero el bloque de memoria */
	/*
	 * No hace falta proteger con mutex porque la función ya implementa
	 * la critical section
	 */
	QMPool_put( &self->mem_pool, block );
}

/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/
