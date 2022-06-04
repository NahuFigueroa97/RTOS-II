/*=====[access2medium.c]======================================================
 * Copyright 2020 Authors:
 * Felipe Alberto Calcavecchia  <facalcavec@gmail.com>
 * Fabiola de las Casas EscardÃ³ <fabioladelascasas@gmail.com>
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
#include "access2medium.h"

#include <string.h>

#include "FreeRTOSConfig.h"
#include "task.h"

/*=====[Definition macros of private constants]==============================*/

#define TASK_NAME_TX "TAREA (Acceso al Medio: TX)"
#define PROTOCOL_TIMEOUT pdMS_TO_TICKS(60)

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/
extern memory_manager_t mem_handle;

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of Interrupt functions]====================*/

/**
 * @brief Rutina de interrupción que atiende la recepción del periférico
 *
 * @param self Manejador del perisferico ( TDA )
 */
void ISR_RX( void *self );

/**
 * @brief Rutina de interrupción que atiende la transmisión del periférico
 *
 * @param self Manejador del perisferico ( TDA )
 */
void ISR_TX( void *self );

/**
 * @brief Callback de timeout de recepción
 * @param timer asociado
 */
void onRxTimeOutCallback( TimerHandle_t xTimer );

/**
 * @brief Callback de timeout de transmision
 * @param timer asociado
 */
void onTxTimeOutCallback( TimerHandle_t xTimer );

/*=====[Prototypes (declarations) of private functions]======================*/


/**
 * @brief Configura el periférico
 *
 * @param self Manejador del perisferico ( TDA )
 * @param [in] uart Configura el periferico UART que se desea utilizar
 * @param [in] baudrate Configura el baudrate de la comunicación
 */
static void _ConfigPeriph( access2medium_t *self,
						   uartMap_t uart,
						   uint32_t baudrate );

/**
 * @brief Configura los elementos de sincronización interna
 *
 * @details Se utilizan semaforos y mutex para sincronizar las rutinas de
 * interrupcion, que atienden la transmisión y recepción del periférico, con
 * las tareas (_threads_) que implementan la aplicación.
 *
 * @param self Manejador del perisferico ( TDA )
 */
static void _ConfigSincronization( access2medium_t *self );


/*=====[Implementations of public functions]=================================*/

void Access2Medium_Create( access2medium_t *self,
						   uartMap_t uart,
						   uint32_t baudrate,
						   callback_rx Fn )
{
	memset( self, 0, sizeof( access2medium_t ) );

/*-------- CONFIGURO LA PARTE LOGICA -------------------------------*/
	_ConfigSincronization( self );
	self->CatchMsg = Fn;

/*----------- CONFIGURO EL PERIFÉRICO -------------------------------*/
    _ConfigPeriph( self, uart, baudrate );


/*----------- CREO LAS TAREAS QUE MANEJAN EL ACCESO AL MEDIO------------ */
	BaseType_t res;

	/* Tarea que maneja la transmisión */
    res = xTaskCreate(
						Access2Medium_Transmition,      // Funcion de la tarea a ejecutar
						( const char * )TASK_NAME_TX, 	// Nombre de la tarea como String amigable para el usuario
						configMINIMAL_STACK_SIZE*1,     // Cantidad de stack de la tarea
						self,                           // Parametros de tarea
						tskIDLE_PRIORITY+1,           	// Prioridad de la tarea
						0                             	// Puntero a la tarea creada en el sistema
			 );
	configASSERT( res == pdPASS ); 	// Gestion de errores

	/*----------- CREO LAS COLAS ------------ */
	Cola_Create( &self->cola_1a2 );
	Cola_Create( &self->cola_2a1 );

    /*----------- CREO LOS TIMERS ASOCIADO A LA RECEPCION Y AL ENVIO ------------ */

    self->onTxTimeOut = xTimerCreate( "TX Time Out", 	// Nombre del timer como String amigable para el usuario
    								  PROTOCOL_TIMEOUT, // Tiempo para la ejecucion del callback
									  pdTRUE, 			// Si es pdTrue, el callback se llama periódicamente.
									  ( void* ) self, 	// Puntero a variable de usuario que comparte el callback y la tarea que arranca el timer
									  onTxTimeOutCallback // Funcion de callback
						);

    self->onRxTimeOut = xTimerCreate( "RX Time Out", 	// Nombre del timer como String amigable para el usuario
    								  PROTOCOL_TIMEOUT, // Tiempo para la ejecucion del callback
									  pdFALSE, 			// Si es pdTrue, el callback se llama periódicamente.
									  ( void* ) self, 	// Puntero a variable de usuario que comparte el callback y la tarea que arranca el timer
									  onRxTimeOutCallback // Funcion de callback
						);
}

void Access2Medium_Transmition( void* self )
{
	access2medium_t *acc2med = (access2medium_t *) self;
	tMensaje message;

	/* arranco el timer de transmision */
	xTimerStart( acc2med->onTxTimeOut, 0 );

	acc2med->sending = FALSE;
	acc2med->timeOut_tx = TRUE;
    
	while( TRUE )
    {
		/* Uso la cola desde la que recibo datos de la capa 2
		 * como elemento de sincronización */
    	Cola_Peek( &acc2med->cola_2a1, &message );

    	Access2Medium_SendFrame( acc2med );

        while ( !( acc2med->timeOut_tx ))
        {
        	/* no hace nada, solo espera que se consuma el tiempo del timer */
        }
    }
}


void Access2Medium_SendFrame( access2medium_t *self )
{
    /* Seteo un callback al evento de transmisión y habilito su interrupcion */
	uartCallbackSet( self->uart, UART_TRANSMITER_FREE, ISR_TX, self );

    /* Dispara la interrupcion de transmisión*/
    uartSetPendingInterrupt( self->uart );

    /* Señal de sincronización: se está enviando un paquete*/
    xSemaphoreTake( self->sem_tx, portMAX_DELAY );
}

/*=====[Implementations of interrupt functions]==============================*/
void ISR_TX( void *param )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    access2medium_t *acc2med = (access2medium_t *) param;

    /* En caso de estar no estar enviando un mensaje, verifico si hay
     * alguno esperando a ser mandado en la cola */
    if ( FALSE == acc2med->sending )
    {
    	acc2med->sending = Cola_PopFromISR( &acc2med->cola_2a1, &acc2med->frame_tx );
    	acc2med->bytes_sent = 0;
    }

    /* Acción a ejecutar si estoy enviando un mensaje */
    if ( TRUE == acc2med->sending )
    {

    	/* si es el primer byte a enviar reseteo el timer de transmision */
    	if( acc2med->bytes_sent == 0 )
		{
			xTimerStartFromISR( acc2med->onTxTimeOut, &xHigherPriorityTaskWoken );
		}

		/* Envío el dato si aún no me llegó un '\0' */
		if( acc2med->frame_tx[acc2med->bytes_sent] != '\0' )
		{
			uartTxWrite( acc2med->uart, acc2med->frame_tx[acc2med->bytes_sent] );
			acc2med->bytes_sent += 1;
		}
		else
		{
			Memory_Dealloc( &mem_handle, acc2med->frame_tx );

			/* Seteo un callback al evento de recepcion y habilito su interrupcion */
		    uartCallbackSet( acc2med->uart, UART_RECEIVE, ISR_RX, acc2med );

			/* Terminé de enviar el dato */
			acc2med->sending = FALSE;
			acc2med->timeOut_tx = FALSE;

			/* deshabilito la isr de transmision */
			uartCallbackClr( acc2med->uart, UART_TRANSMITER_FREE );

			/* Termina la sección crítica de procesamiento de un byte recibido */
			xSemaphoreGiveFromISR( acc2med->sem_tx, &xHigherPriorityTaskWoken );
		}
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


void ISR_RX( void *param )
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    access2medium_t *acc2med = (access2medium_t *) param;

    /* leemos el caracter recibido */
    char c = uartRxRead( acc2med->uart );

    /* Comienza la sección crítica de procesamiento de un byte recibido */
    BaseType_t signaled = xSemaphoreTakeFromISR( acc2med->mutex,
    											 &xHigherPriorityTaskWoken );

    if( signaled )
    {
    	/* Llamo a la callback function que me indica qué hacer con el byte */
    	if ( TRUE == acc2med->CatchMsg( c,
    									&acc2med->bytes_received,
										acc2med->frame_rx ) )
    	{
    		Cola_PostFromISR( &acc2med->cola_1a2, &acc2med->frame_rx );

			/* Pedir memoria dinamica */
    		if ( FALSE == Memory_Alloc( &mem_handle, &acc2med->frame_rx ) )
    		{
    			/* deshabilito la isr de recepción */
    			uartCallbackClr( acc2med->uart, UART_RECEIVE );
    		}
    	}
		else
		{
			// Reseteo el timer si recibi un byte que no es el EOM.
			xTimerStartFromISR( acc2med->onRxTimeOut, &xHigherPriorityTaskWoken );
		}
        /* Termina la sección crítica de procesamiento de un byte recibido */
        xSemaphoreGiveFromISR( acc2med->mutex, &xHigherPriorityTaskWoken );
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*=====[Implementations of private functions]================================*/

void _ConfigSincronization( access2medium_t *self )
{
	/* Indico que aun no recibi nada */
    self->bytes_received = 0;

    /* pedir memoria dinamica para self->frame */
    Memory_Alloc( &mem_handle, &self->frame_rx );

    self->mutex 			= xSemaphoreCreateMutex();
    configASSERT( self->mutex != NULL ); /* gestion de errores */

    self->sem_tx 			= xSemaphoreCreateBinary();
	configASSERT( self->sem_tx != NULL ); /* gestion de errores */
}

void _ConfigPeriph( access2medium_t *self, uartMap_t uart, uint32_t baudrate )
{
    self->uart = uart;
    self->baudrate = baudrate;

    /* Inicializar la UART_USB junto con las interrupciones de Tx y Rx */
    uartConfig( self->uart, self->baudrate );

    /* Seteo un callback al evento de recepcion y habilito su interrupcion */
    uartCallbackSet( self->uart, UART_RECEIVE, ISR_RX, self );

    /* Habilito todas las interrupciones de UART_USB */
    uartInterrupt( self->uart, TRUE );
}

/*=====[Implementations of timer functions]================================*/

void onRxTimeOutCallback( TimerHandle_t xTimer)
{
	access2medium_t *self = ( access2medium_t *) pvTimerGetTimerID( xTimer );
	
	/* con time out descarto el paquete y comienzo de nuevo */
	self->bytes_received = 0;
}

void onTxTimeOutCallback( TimerHandle_t xTimer)
{
	access2medium_t *self = ( access2medium_t *) pvTimerGetTimerID( xTimer );

	/* con time out habilito para nueva transmision de paquetes */
	self->timeOut_tx = TRUE;
}
