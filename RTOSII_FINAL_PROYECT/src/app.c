/*=====[app.c]======================================================
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
#include "app.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/
extern cola_t cola_2a3, cola_3a2;

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/


/*=====[Prototypes (declarations) of private functions]====================*/

/*=====[Prototypes (declarations) of public functions]======================*/

void Minusc(activeObjectEvent_t *AOdata)
{
	uint16_t size_msj;
	uint16_t i = OFFSET_PAYLOAD;	// Arranca en 2 porque el msj[0]=SOM y msj[1]=c

	size_msj = strlen(*AOdata);

	while ( i < size_msj - BYTES_OMITTED)
	{
		/* Convierto a minuscula todos los caractertes, excepto el espacio */
		if( (*AOdata)[i] >= ASCII_A && (*AOdata)[i] <= ASCII_Z )
		{
			(*AOdata)[i] = (*AOdata)[i] - ASCII_A + ASCII_a;
		}
		else if( ( (*AOdata)[i] >= ASCII_a && (*AOdata)[i] <= ASCII_z ) ||
				 ( (*AOdata)[i] == ASCII_espacio ) )
		{
			/* no hace nada */
		}
		else
		{
			strcpy((*AOdata), MSG_ERROR_CARACTER);
			return;
		}
		i++;
	}
}

void Mayusc(activeObjectEvent_t *AOdata)
{
	uint16_t size_msj;
	uint16_t i = OFFSET_PAYLOAD;	// Arranca en 2 porque el msj[0]=SOM y msj[1]=c

	size_msj = strlen((*AOdata));

	while ( i < size_msj - BYTES_OMITTED )
	{
		/* Convierto a mayuscula todos los caractertes, excepto el espacio */
		if( (*AOdata)[i] >= ASCII_a && (*AOdata)[i] <= ASCII_z )
		{
			(*AOdata)[i] = (*AOdata)[i] - ASCII_a + ASCII_A;
		}
		else if( ( (*AOdata)[i] >= ASCII_A && (*AOdata)[i] <= ASCII_Z ) ||
				 ( (*AOdata)[i] == ASCII_espacio ) )
		{
			/* no hace nada */
		}
		else
		{
			strcpy((*AOdata), MSG_ERROR_CARACTER);
			return;
		}
		i++;
	}
}

void App_Convert( void* taskParmt )
{
	tMensaje data;

    activeObject_t mayuscAO;
    mayuscAO.exist = FALSE;

    activeObject_t minuscAO;
    minuscAO.exist = FALSE;
    printf( "ingreso a App_Convert\r\n");
	while( TRUE )
	{
		/* Recibo desde la capa 2 */
		Cola_Pop( &cola_2a3, &data );
		printf( "elijo m o M\r\n");

		 switch(data[1])
		 {
			case 'm':
				if (minuscAO.exist == FALSE)
				{
					activeObjectCreate( &minuscAO, Minusc , activeObjectTask );
					printf( "entro if\r\n");
				}

				xQueueSend( minuscAO.activeObjectQueue, &data, portMAX_DELAY );
				break;

			case 'M':
				if (mayuscAO.exist == FALSE)
				{
					activeObjectCreate( &mayuscAO, Mayusc , activeObjectTask );
				}

				xQueueSend( mayuscAO.activeObjectQueue, &data, portMAX_DELAY );
				break;

			default:
				strcpy(data, MSG_ERROR_C);
				break;
		 }
		 /* Envio data a la capa 2 */
		Cola_Post( &cola_3a2, &data );
	}
}



