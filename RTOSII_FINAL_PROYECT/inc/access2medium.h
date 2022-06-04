/*=====[access2medium.h]======================================================
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

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef ACCESS2MEDIUM_H
#define ACCESS2MEDIUM_H

/*=====[Inclusions of public function dependencies]==========================*/
#include <stdint.h>

#include "cola.h"
#include "FreeRTOSConfig.h"
#include "memory_manager.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "sapi.h"
#include "timers.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

typedef bool (*callback_rx)( char c, uint32_t *bytes_received, tMensaje message);

typedef struct {
	uartMap_t uart;		/** @brief Periferico con el que se estÃ¡ trabajando */
	uint32_t baudrate;	/** @brief Baudrate de la transmisiÃ³n */

	tMensaje frame_rx;
	callback_rx CatchMsg;
	uint32_t bytes_received;

	tMensaje frame_tx;
	bool sending;
	uint32_t bytes_sent;
	bool timeOut_tx;

	/**
	 * @brief Utilizado para establecer zonas criticas asociadas a los paquetes
	 * recibidos
	 */
	SemaphoreHandle_t mutex;

	/**
	 * @brief Utilizado para sincronizar la transmisiÃ³n de paquetes entre la
	 * interrupciÃ³n y la tarea
	 */
	SemaphoreHandle_t sem_tx;

	TimerHandle_t onTxTimeOut; //Timer para el timeout de la transmision
    TimerHandle_t onRxTimeOut; //Timer para el timeout de la recepcion

	cola_t cola_1a2;
    cola_t cola_2a1;
	
} access2medium_t;


/*=====[Prototypes (declarations) of public functions]=======================*/

/**
 * @brief Constructor de la capa de acceso al medio.
 *
 * @param self Manejador del perisferico ( TDA )
 * @param [in] uart Configura el periferico UART que se desea utilizar
 * 		- USB_UART
 * 		- UART_GPIO
 * 		- UART_485
 * 		- UART_ENET
 * 		- UART_232
 * @param [in] baudrate Configura el baudrate de la comunicaciÃ³n
 *
 */
void Access2Medium_Create( access2medium_t *self,
						   uartMap_t uart,
						   uint32_t baudrate,
						   callback_rx Fn );

/**
 * @brief Tarea (_thread_) que maneja el acceso al medio para transmisiÃ³n.
 *
 * @details Se encarga de consolidar los datos recibidos y se los comunica a
 * capas superiores. AdemÃ¡s, administra las solicitudes de transmisiÃ³n de las
 * capas superiores.
 *
 * @param [in] self Manejador del perisferico ( TDA )
 */
void Access2Medium_Transmition( void *self );

/**
 * @brief Tarea (_thread_) que maneja el acceso al medio para recepciÃ³n
 *
 * @details Se encarga de consolidar los datos recibidos y se los comunica a
 * capas superiores. AdemÃ¡s, administra las solicitudes de transmisiÃ³n de las
 * capas superiores.
 *
 * @param [in] self Manejador del perisferico (Es el propio TDA)
 */
void Access2Medium_Reception( void *self );

/**
 * @brief Se encarga de manejar y coordinar el envÃ­o de paquetes
 *
 * @param self Manejador del perisferico ( TDA )
 * @param [in] data Arreglo de caracteres que se desea transmitir
 * @param [in] size Cantidad de bytes que se desean transmitir
 */
void Access2Medium_SendFrame( access2medium_t *self );

/**
 * @brief Se encarga de coordinar la recepciÃ³n de un paquete
 *
 * @param self Manejador del perisferico ( TDA )
 * @param [out] data Arreglo de caracteres que fue recibido
 * @param [out] size Cantidad de bytes que se recibieron
 */
void Access2Medium_ReceiveFrame( access2medium_t *self,
								 char **data,
								 uint32_t *size );

/**
 * @brief Se encarga de liberar el buffer de recepciÃ³n y gestionar los recursos
 * del perifÃ©rico para que reanude la recepciÃ³n de nuevos paquetes
 *
 * @param self Manejador del perisferico ( TDA )
 */
void Access2Medium_DiscardFrame( access2medium_t *self );

/**
 * @brief calcula el CRC8s
 *
 * @param val semilla
 * @param *buf puntero al vector a analizar
 * @param cnt tamaÃ±o del vector
 */
uint8_t crc8_calc(uint8_t val, void *buf, int cnt);

/*=====[Prototypes (declarations) of public interrupt functions]=============*/


/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* _ACCESS2MEDIUM_H */
