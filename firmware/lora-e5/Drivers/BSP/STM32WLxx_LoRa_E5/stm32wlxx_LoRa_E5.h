#ifndef STM32WLXX_LORA_E5_H
#define STM32WLXX_LORA_E5_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32wlxx_LoRa_E5_conf.h"
#include "stm32wlxx_LoRa_E5_errno.h"

#if (USE_BSP_COM_FEATURE > 0)
#if (USE_COM_LOG > 0)
#ifndef __GNUC__
#include "stdio.h"
#endif
#endif
#endif

typedef enum { LED1 = 0, LED_PWR = LED1 } Led_TypeDef;

#if (USE_BSP_COM_FEATURE > 0)
typedef enum { COM1 = 0U, COMn } COM_TypeDef;

typedef enum {
  COM_STOPBITS_1 = UART_STOPBITS_1,
  COM_STOPBITS_2 = UART_STOPBITS_2,
} COM_StopBitsTypeDef;

typedef enum {
  COM_PARITY_NONE = UART_PARITY_NONE,
  COM_PARITY_EVEN = UART_PARITY_EVEN,
  COM_PARITY_ODD = UART_PARITY_ODD,
} COM_ParityTypeDef;

typedef enum {
  COM_HWCONTROL_NONE = UART_HWCONTROL_NONE,
  COM_HWCONTROL_RTS = UART_HWCONTROL_RTS,
  COM_HWCONTROL_CTS = UART_HWCONTROL_CTS,
  COM_HWCONTROL_RTS_CTS = UART_HWCONTROL_RTS_CTS,
} COM_HwFlowCtlTypeDef;

typedef enum {
  COM_WORDLENGTH_7B = UART_WORDLENGTH_7B,
  COM_WORDLENGTH_8B = UART_WORDLENGTH_8B,
  COM_WORDLENGTH_9B = UART_WORDLENGTH_9B,
} COM_WordLengthTypeDef;

typedef struct {
  uint32_t BaudRate;
  COM_WordLengthTypeDef WordLength;
  COM_StopBitsTypeDef StopBits;
  COM_ParityTypeDef Parity;
  COM_HwFlowCtlTypeDef HwFlowCtl;
} COM_InitTypeDef;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
typedef struct {
  pUART_CallbackTypeDef pMspInitCb;
  pUART_CallbackTypeDef pMspDeInitCb;
} BSP_COM_Cb_t;
#endif
#endif

typedef enum {
  ABSENT = 0,
  PRESENT = 1,
} Presence_TypeDef;

#define __STM32WLXX_LoRa_E5_BSP_VERSION_MAIN (0x01U)
#define __STM32WLXX_LoRa_E5_BSP_VERSION_SUB1 (0x01U)
#define __STM32WLXX_LoRa_E5_BSP_VERSION_SUB2 (0x00U)
#define __STM32WLXX_LoRa_E5_BSP_VERSION_RC (0x00U)
#define __STM32WLXX_LoRa_E5_BSP_VERSION                                        \
  ((__STM32WLXX_LoRa_E5_BSP_VERSION_MAIN << 24) |                              \
   (__STM32WLXX_LoRa_E5_BSP_VERSION_SUB1 << 16) |                              \
   (__STM32WLXX_LoRa_E5_BSP_VERSION_SUB2 << 8) |                               \
   (__STM32WLXX_LoRa_E5_BSP_VERSION_RC))

#if !defined(USE_STM32WLXX_LoRa_E5)
#define USE_STM32WLXX_LoRa_E5
#endif

#define LEDn 1
#define LED1_PIN GPIO_PIN_6
#define LED1_GPIO_PORT GPIOB
#define LED1_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE() __HAL_RCC_GPIOB_CLK_DISABLE()
#define LEDx_GPIO_CLK_ENABLE(__INDEX__) __HAL_RCC_GPIOB_CLK_ENABLE()
#define LEDx_GPIO_CLK_DISABLE(__INDEX__) __HAL_RCC_GPIOB_CLK_DISABLE()

#if (USE_BSP_COM_FEATURE > 0)
/** @defgroup STM32WLXX_LoRa_E5_LOW_LEVEL_COM LOW LEVEL COM Port Constants
 * @{
 */
#define COM1_UART LPUART1
#define COM1_CLK_ENABLE() __HAL_RCC_LPUART1_CLK_ENABLE()
#define COM1_CLK_DISABLE() __HAL_RCC_LPUART1_CLK_DISABLE()

#define COM1_TX_PIN GPIO_PIN_2
#define COM1_TX_GPIO_PORT GPIOA
#define COM1_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM1_TX_GPIO_CLK_DISABLE() __HAL_RCC_GPIOA_CLK_DISABLE()
#define COM1_TX_AF GPIO_AF8_LPUART1

#define COM1_RX_PIN GPIO_PIN_3
#define COM1_RX_GPIO_PORT GPIOA
#define COM1_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM1_RX_GPIO_CLK_DISABLE() __HAL_RCC_GPIOA_CLK_DISABLE()
#define COM1_RX_AF GPIO_AF8_LPUART1
#define COM_POLL_TIMEOUT 1000

#define MX_UART_InitTypeDef COM_InitTypeDef
#endif

#if (USE_BSP_COM_FEATURE > 0)
extern UART_HandleTypeDef hcom_uart[];
#endif

uint32_t BSP_GetVersion(void);
int32_t BSP_LED_Init(Led_TypeDef Led);
int32_t BSP_LED_DeInit(Led_TypeDef Led);
int32_t BSP_LED_On(Led_TypeDef Led);
int32_t BSP_LED_Off(Led_TypeDef Led);
int32_t BSP_LED_Toggle(Led_TypeDef Led);
int32_t BSP_LED_GetState(Led_TypeDef Led);

#if (USE_BSP_COM_FEATURE > 0)
int32_t BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init);
int32_t BSP_COM_DeInit(COM_TypeDef COM);
#if (USE_COM_LOG > 0)
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM);
#endif
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM);
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *Callback);
#endif
HAL_StatusTypeDef MX_LPUART1_Init(UART_HandleTypeDef *huart,
                                  MX_UART_InitTypeDef *COM_Init);
#endif

#ifdef __cplusplus
}
#endif

#endif
