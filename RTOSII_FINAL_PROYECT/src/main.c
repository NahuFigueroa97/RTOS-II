/*=====[main.c]===========================================================
 * Copyright 2020 Authors:
 * Felipe Alberto Calcavecchia  <facalcavec@gmail.com>
 * Fabiola de las Casas EscardÃ³ <fabioladelascasas@gmail.com>
 * Alejandro Moreno 			<ale.moreno991@gmail.com>
 *
 * All rights reserved.
 * License: license text or at least name and link
 *       (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 1.0.0
 * Creation Date: 2020/12/02
 */

/*=====[Inclusion of own header]=============================================*/


/*=====[Inclusions of private function dependencies]=========================*/
#include "access2medium.h"
#include "app.h"
#include "cola.h"
#include "frame_parser.h"
#include "FreeRTOSConfig.h"
#include "qmpool.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "AO.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/* Manejador del perifÃ©rico UART_USB */
access2medium_t uart_usb_handle;

/* Colas de comunicaciÃ³n entre capas */
cola_t cola_2a3;
cola_t cola_3a2;//Definimos la cola

/* Manejador de memoria dinamica */
memory_manager_t mem_handle;

tMensaje mem_pool_ptr; //puntero al segmento de memoria que albergara el pool

frame_parser_t frame_parser_handle;

activeObject_t aoApp;


/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

int main( void )
{
	/* Inicializar la placa */
    boardConfig();

    Memory_Init ( &mem_handle, &mem_pool_ptr );

	Cola_Create( &cola_2a3 );//Crea COLAS
	Cola_Create( &cola_3a2 );//Crea COLAS

    Access2Medium_Create( &uart_usb_handle,
    					  UART_USB,
						  115200,
						  FrameParser_CatchMsg );

    FrameParser_Create( &frame_parser_handle );//CREA TAREAS


    printf( "Grupo XXX - Farfan - Rizo\r\n");
   // printf( "temperatura_sensor = %d --> temperatura_grados = %d C\r\n", 1, 1 );


    activeObjectCreate( &aoApp, NULL , App_Convert );
	
    vTaskStartScheduler();

    return 0;
}

/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/
