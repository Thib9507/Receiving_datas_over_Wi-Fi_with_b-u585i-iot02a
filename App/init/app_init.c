/*
 * app_init.c
 *
 *  Created on: Oct 10, 2024
 *      Author: Thibault CIMIOTTI
 */

#include "app_init.h"

// 3 following functions are needed to communicate with the module
/**
  * @brief This function handles EXTI Line13 interrupt. (Notify_pin of spi) need it to change the state and to speak to/ listen from the module
  */
void EXTI14_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(MXCHIP_NOTIFY_Pin);
}

/**
  * @brief This function handles EXTI Line15 interrupt. (flow pin of spi) need it to change the state and to speak to/ listen from the module
  */
void EXTI15_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(MXCHIP_FLOW_Pin);
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    /* MXCHIP flow pin notification */
    case MXCHIP_FLOW_Pin:
    {
      mxchip_WIFI_ISR(MXCHIP_FLOW_Pin);
      break;
    }

    /* MXCHIP notify pin notification */
    case (MXCHIP_NOTIFY_Pin):
    {
      mxchip_WIFI_ISR(MXCHIP_NOTIFY_Pin);
      break;
    }

  default:
    {
      break;
    }
  }
}




// 2 following functions are needed for the initialization of the module
/**
  * @brief WiFi module GPIO Initialization Function
  * @param None
  * @retval None
  */
void Wifi_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MXCHIP_RESET_GPIO_Port, MXCHIP_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MXCHIP_NSS_GPIO_Port, MXCHIP_NSS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : MXCHIP_FLOW_Pin */
  GPIO_InitStruct.Pin = MXCHIP_FLOW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MXCHIP_FLOW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MXCHIP_RESET_Pin */
  GPIO_InitStruct.Pin = MXCHIP_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MXCHIP_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PD14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : MXCHIP_NSS_Pin */
  GPIO_InitStruct.Pin = MXCHIP_NSS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MXCHIP_NSS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MXCHIP_NOTIFY_Pin */
  GPIO_InitStruct.Pin = MXCHIP_NOTIFY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MXCHIP_NOTIFY_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI14_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI14_IRQn);

  HAL_NVIC_SetPriority(EXTI15_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_IRQn);
}


// function hw_start adapted from the initial project (consider the structure above)
int32_t hw_start(/*net_if_handle_t *pnetif */probe *probe_object)
{
  int32_t ret = /*NET_ERROR_DEVICE_ERROR*/-13;

  if (mxwifi_probe(&probe_object->probe_attribute) == 0) // function to indicate witch function should be use for the SPI
  {
    DEBUG_LOG("%s\n", "MX_WIFI IO [OK]");

    if (wifi_obj_get()->Runtime.interfaces == 0U)
    {
       // WiFi module hardware reboot.
      DEBUG_LOG("%s\n", "MX_WIFI REBOOT(HW) ...");
      ret = MX_WIFI_HardResetModule(wifi_obj_get());         // Not an error, it is mandatory for the init
    }
    else
    {
      ret = MX_WIFI_STATUS_OK;
    }

    if (MX_WIFI_STATUS_OK != ret)
    {
      ret = /*NET_ERROR_DEVICE_ERROR*/-13;
    }
    else
    {
      /* Initialize the WiFi module. */
      if (MX_WIFI_STATUS_OK != MX_WIFI_Init(wifi_obj_get())) // function to init the WiFi module communication and to verify it with 2 requests : ask version and ask MAC adress
      {
        ret = /*NET_ERROR_INTERFACE_FAILURE*/-17;
      }
      else
      {
          ret = /*NET_OK*/0;
      }
    }
  }
    return ret;
}



int8_t module_init(void){
    bsp_init();

    Wifi_IO_Init(); // initialization of the wifi module secondaries pins (not the spi pins)

    // initialization of the module request

	probe probe_object; // ~ pnetif to feed hw_start

	int32_t ret_hw_start = hw_start(&probe_object); // initialization of the SPI and the module

		if (ret_hw_start < 0){
			return INIT_KO;
		}
	return INIT_OK;
}
