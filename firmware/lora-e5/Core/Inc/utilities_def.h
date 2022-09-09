#ifndef __UTILITIES_DEF_H__
#define __UTILITIES_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
  CFG_LPM_APPLI_Id,
  CFG_LPM_UART_TX_Id,
} CFG_LPM_Id_t;

typedef enum {
  CFG_SEQ_Prio_0,
  CFG_SEQ_Prio_NBR,
} CFG_SEQ_Prio_Id_t;

typedef enum {
  CFG_SEQ_Task_LmHandlerProcess,
  CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent,
  CFG_SEQ_Task_LoRaStoreContextEvent,
  CFG_SEQ_Task_LoRaStopJoinEvent,
  CFG_SEQ_Task_NBR
} CFG_SEQ_Task_Id_t;


#ifdef __cplusplus
}
#endif

#endif
