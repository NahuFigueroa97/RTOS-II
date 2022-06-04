/*=====[frame_parser.c]======================================================
 * Copyright 2020 Authors:
 * Felipe Alberto Calcavecchia  <facalcavec@gmail.com>
 * Fabiola de las Casas Escardó <fabioladelascasas@gmail.com>
 * Alejandro Moreno 			<ale.moreno991@gmail.com>
 *
 * All rights reserved.
 * License: license text or at least name and link
 *  (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 0.0.3
 * Creation Date: 2020/11/26
 */

/*=====[Inclusion of own header]=============================================*/
#include "frame_parser.h"

#include <string.h>
#include <stdbool.h>

#include "FreeRTOSConfig.h"
#include "task.h"
#include "access2medium.h"


/*=====[Definition macros of private constants]==============================*/
#define FRAME_MAX_SIZE  201

#define SOM '(' 	/** @brief Start of Message delimiter */
#define EOM ')'		/** @brief End of Message delimiter */

#define ASCII_0 '0'
#define ASCII_9 '9'

#define ASCII_A 'A'

#define END_OF_STRING '\0'

#define TASK_PARSER_ENCODE "TAREA (encode frames)"
#define TASK_PARSER_DECODE "TAREA (decode frames)"

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

extern cola_t cola_2a3, cola_3a2;

extern access2medium_t uart_usb_handle;

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of Interrupt functions]====================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/**
 * @brief Se encarga de recibir el inicio de un paquete
 *
 * @param self Manejador del perisferico ( TDA )
 * @param c Byte recibido
 */
static void _StartOfMessage( char c, uint32_t *bytes_received, char *message );

/**
 * @brief Se encarga de recibir un byte en el medio paquete
 *
 * @param self Manejador del perisferico ( TDA )
 * @param c Byte recibido
 */
static void _MiddleOfMessage( char c, uint32_t *bytes_received, char *message );

/**
 * @brief Se encarga de recibir el final de un paquete
 *
 * @param self Manejador del perisferico ( TDA )
 * @param c Byte recibido
 * return true Paquete recibido con éxito
 * return false Paquete descartado
 */
static bool _EndOfMessage( char c, uint32_t *bytes_received, char *message );

/*=====[Implementations of public functions]=================================*/

void FrameParser_Create( frame_parser_t *self )
{
	BaseType_t res;

    /* Inicializar los elementos de sincronización */
    self->new_frame_signal 	= xSemaphoreCreateBinary();
    configASSERT( self->new_frame_signal != NULL ); /* gestion de errores */

	/* Tarea que se encarga de parsear las tramas */
    res = xTaskCreate(
    					FrameParser_RunEncoding,      		// Funcion de la tarea a ejecutar
						( const char * )TASK_PARSER_ENCODE, // Nombre de la tarea como String amigable para el usuario
						configMINIMAL_STACK_SIZE*1,     	// Cantidad de stack de la tarea
						self,                           	// Parametros de tarea
						tskIDLE_PRIORITY+1,           		// Prioridad de la tarea
						0                             		// Puntero a la tarea creada en el sistema
			 );

	configASSERT( res == pdPASS ); 	// Gestión de errores

	/* Tarea que se encarga de parsear las tramas */
    res = xTaskCreate(
    					FrameParser_RunDecoding,      		// Funcion de la tarea a ejecutar
						( const char * )TASK_PARSER_DECODE, // Nombre de la tarea como String amigable para el usuario
						configMINIMAL_STACK_SIZE*1,     	// Cantidad de stack de la tarea
						self,                           	// Parametros de tarea
						tskIDLE_PRIORITY+1,           		// Prioridad de la tarea
						0                             		// Puntero a la tarea creada en el sistema
			 );

	configASSERT( res == pdPASS ); 	// Gestión de errores
}

void FrameParser_RunDecoding( void *self )
{
	frame_parser_t *parser = (frame_parser_t *) self;
	tMensaje data;
	int size;
	uint8_t crc1,crc2;

    while( TRUE )
    {
    	/* Recibo desde la capa 1 */
    	Cola_Pop( &uart_usb_handle.cola_1a2, &data );
    	printf( "Recibe de 1 a 2 - Mensaje de ingreso UART: %s \r\n", data );

    	//printf( "Cola POP %s \r\n", data );

		/* determina el tamaño del string descartando los delimitadores */
		for( int i = 1; data[i] != EOM; i++ )
		{
			size = i;
		}

		//printf( "Tama�o del mensaje: %d \r\n", size );

		/* incrusta un '\0' al final del string	 */

		data[size + 2] = END_OF_STRING;


		/* se prepara para calcular el CRC8 */
		crc1 = 0;

		/* convertidor ASCII a hexadecimal */
		crc1 = ( data[size-1] <= ASCII_9 ) ? ( data[size-1] - ASCII_0) : ( data[size-1] - ASCII_A + 10);
		//printf( "CRC1-a: %d \r\n", crc1 );
		crc1 = crc1 << 4;
		//printf( "CRC1-b: %d \r\n", crc1 );
		crc1 += ( data[size] <= ASCII_9 ) ? ( data[size] - ASCII_0) : ( data[size] - ASCII_A + 10);
		printf( "CRC1-c: %d \r\n", crc1 );
		crc2=crc8_calc(SEED, data + 1, size - 2);
		printf( "CRC2: %d \r\n", crc2 );

		/* compara el CRC que viene con el string y el calculado */
		if( crc1 == crc8_calc(SEED, data + 1, size - 2))
		{
			printf( "CRC8 OK\r\n");
			printf( "Dato ok: %s \r\n", data );
			/* envia el paquete a la capa de aplicacion 3 */
			Cola_Post( &cola_2a3, &data );
		}
      	else
      	{
      		printf( "CRC8 ERROR\r\n");
      		printf( "Dato error: %s \r\n", data );
      		/* descarta el string por error en el CRC */
      		data[0] = END_OF_STRING;
      		//Cola_Post( &uart_usb_handle.cola_2a1, &data );
		}
    }
}


void FrameParser_RunEncoding( void *self )
{
	frame_parser_t *parser = (frame_parser_t *) self;
	char *data;
	uint32_t size;
	uint8_t crc2;

    while( TRUE )
    {
    	/* Recibo desde la capa 3 */
    	Cola_Pop( &cola_3a2, &data );
    	printf( "Recibio de 3 a 2 \r\n");
		// determina el tamaño del string descartando los delimitadores
		for( int i = 1; data[i] != EOM; i++ )
		{
			size = i;
		}

		crc2 = crc8_calc(SEED, data + 1, size - 2);

		uint8_t aux = crc2 & 0x0F;

		/* convertidor hexadecimal a ASCII */
		data[size] = ( aux < 10 ) ? ( aux + ASCII_0 ) : ( aux - 10 + ASCII_A);
		aux = ( crc2 >> 4 ) & 0x0F;
		data[size - 1] = ( aux < 10 ) ? ( aux + ASCII_0 ) : ( aux - 10 + ASCII_A);

    	/* Envío el dato procesado a la capa 1 */
		printf( "Envia cola2a1 \r\n");
		Cola_Post( &uart_usb_handle.cola_2a1, &data );
    }
}

bool FrameParser_CatchMsg( char c, uint32_t *bytes_received, tMensaje message )
{
	bool ret = false;

	/* Descarto el paquete si recibí más bytes del máximo estipulado */
	if( (FRAME_MAX_SIZE - 1) == *bytes_received )
	{
		/* reinicio el paquete */
		*bytes_received = 0;
	}

	/* Decido que hacer dependiento el byte que llegó */
	switch (c)
	{
		case SOM:
			_StartOfMessage( c, bytes_received, message );
			break;

		case EOM:
			if ( true == _EndOfMessage( c, bytes_received, message) )
			{
				ret = true;
			}
			break;

		default:
			_MiddleOfMessage( c, bytes_received, message );
			break;
	}

	return ret;
}


/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/

void _StartOfMessage( char c, uint32_t *bytes_received, char *message )
{
    if( 0 == *bytes_received )
    {
        /* 1er byte del frame*/
    }
    else
    {
        /* fuerzo el arranque del frame (descarto lo anterior)*/
    	*bytes_received = 0;
    }

    message[*bytes_received] = c;

    /* incremento la cantidad de bytes recibidos */
    *bytes_received += 1;
}

void _MiddleOfMessage( char c, uint32_t *bytes_received, char *message )
{
    /* solo cierro el fin de frame si al menos se recibio un start.*/
    if( *bytes_received >= 1 )
    {
		/* guardo el dato */
		message[*bytes_received] = c;

		/* incremento la cantidad de bytes recibidos */
		*bytes_received += 1;
	}
	else
	{
		/* si no es un caracter alfa-numerico descarta el string */
		*bytes_received = 0;
	}
}

bool _EndOfMessage( char c, uint32_t *bytes_received, char *message )
{
	bool rv;

	/* solo cierro el fin de frame si al menos se recibio un start.*/
    if( *bytes_received > 0 )
    {
    	/* se termino el paquete - guardo el dato */
    	message[*bytes_received] = c;

        /* incremento la cantidad de bytes recibidos*/
    	*bytes_received += 1;

        rv = true;
    }
    else
    {
        /* no hago nada */
    	rv = false;
    }

    return rv;
}
