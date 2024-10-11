/*
 * app_init.h
 *
 *  Created on: Oct 10, 2024
 *      Author: Thibault CIMIOTTI
 */

#ifndef INIT_APP_INIT_H_
#define INIT_APP_INIT_H_


#ifdef MX_WIFI_API_DEBUG
#define DEBUG_LOG(M, ...)  printf((M), ##__VA_ARGS__) /*;*/
#else
#define DEBUG_LOG(M, ...)  /*;*/
#endif /* MX_WIFI_API_DEBUG */


#include "bsp.h"
#include "mx_wifi.h"
#include "mx_wifi_io.h"
#include "mx_address.h"

#include <stdio.h>
#include <string.h>

// define the wifi module secondaries pins (not the spi pins)
#define MXCHIP_SPI hspi2
#define MXCHIP_FLOW_Pin         GPIO_PIN_15
#define MXCHIP_FLOW_GPIO_Port   GPIOG
#define MXCHIP_FLOW_EXTI_IRQn   EXTI15_IRQn
#define MXCHIP_RESET_Pin        GPIO_PIN_15
#define MXCHIP_RESET_GPIO_Port  GPIOF
#define MXCHIP_NSS_Pin          GPIO_PIN_12
#define MXCHIP_NSS_GPIO_Port    GPIOB
#define MXCHIP_NOTIFY_Pin       GPIO_PIN_14
#define MXCHIP_NOTIFY_GPIO_Port GPIOD
#define MXCHIP_NOTIFY_EXTI_IRQn EXTI14_IRQn


#define NET_IPPROTO_TCP         6


//empty structure to feed mxwifi_probe function in hw_start
typedef struct
{
	void *probe_attribute;
}probe;

typedef enum { // Structure to indicate the initialization result
    INIT_OK = 0,
	INIT_KO = -1,
} Init_status;


// 3 following functions are needed to communicate with the module
/**
  * @brief This function handles EXTI Line13 interrupt. (Notify_pin of spi) need it to change the state and to speak to/ listen from the module
  */
void EXTI14_IRQHandler(void);

/**
  * @brief This function handles EXTI Line15 interrupt. (flow pin of spi) need it to change the state and to speak to/ listen from the module
  */
void EXTI15_IRQHandler(void);

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin);




// 2 following functions are needed for the initialization of the module
/**
  * @brief WiFi module GPIO Initialization Function
  * @param None
  * @retval None
  */
void Wifi_IO_Init(void);


// function hw_start adapted from the initial project (consider the structure above)
int32_t hw_start(/*net_if_handle_t *pnetif */probe *probe_object);


int8_t module_init(void);


#endif /* INIT_APP_INIT_H_ */
