#include "lora_app.h"
#include "CayenneLpp.h"
#include "LmHandler.h"
#include "adc_if.h"
#include "flash_if.h"
#include "lora_app_version.h"
#include "lora_info.h"
#include "lorawan_version.h"
#include "platform.h"
#include "stm32_lpm.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "subghz_phy_version.h"
#include "sys_app.h"
#include "sys_sensors.h"
#include "utilities_def.h"


typedef enum TxEventType_e { TX_ON_TIMER, TX_ON_EVENT } TxEventType_t;

#define LED_PERIOD_TIME 500
#define JOIN_TIME 2000
#define LORAWAN_NVM_BASE_ADDRESS ((uint32_t)0x0803F000UL)

static void SendTxData(void);
static void OnTxTimerEvent(void *context);
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams);
static void OnTxData(LmHandlerTxParams_t *params);
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);
static void OnBeaconStatusChange(LmHandlerBeaconParams_t *params);
static void OnClassChange(DeviceClass_t deviceClass);
static void StoreContext(void);
static void StopJoin(void);
static void OnStopJoinTimerEvent(void *context);
static void OnNvmDataChange(LmHandlerNvmContextStates_t state);
static void OnStoreContextRequest(void *nvm, uint32_t nvm_size);
static void OnRestoreContextRequest(void *nvm, uint32_t nvm_size);
static void OnMacProcessNotify(void);
static void OnTxPeriodicityChanged(uint32_t periodicity);
static void OnTxFrameCtrlChanged(LmHandlerMsgTypes_t isTxConfirmed);
static void OnPingSlotPeriodicityChanged(uint8_t pingSlotPeriodicity);
static void OnSystemReset(void);

static ActivationType_t ActivationType = LORAWAN_DEFAULT_ACTIVATION_TYPE;
static bool ForceRejoin = LORAWAN_FORCE_REJOIN_AT_BOOT;
static uint8_t AppDataBuffer[20];
static LmHandlerCallbacks_t LmHandlerCallbacks = {
    .GetUniqueId = GetUniqueId,
    .GetDevAddr = GetDevAddr,
    .OnRestoreContextRequest = OnRestoreContextRequest,
    .OnStoreContextRequest = OnStoreContextRequest,
    .OnMacProcess = OnMacProcessNotify,
    .OnNvmDataChange = OnNvmDataChange,
    .OnJoinRequest = OnJoinRequest,
    .OnTxData = OnTxData,
    .OnRxData = OnRxData,
    .OnBeaconStatusChange = OnBeaconStatusChange,
    .OnClassChange = OnClassChange,
    .OnTxPeriodicityChanged = OnTxPeriodicityChanged,
    .OnTxFrameCtrlChanged = OnTxFrameCtrlChanged,
    .OnPingSlotPeriodicityChanged = OnPingSlotPeriodicityChanged,
    .OnSystemReset = OnSystemReset,
};
static LmHandlerParams_t LmHandlerParams = {
    .ActiveRegion = ACTIVE_REGION,
    .DefaultClass = LORAWAN_DEFAULT_CLASS,
    .AdrEnable = LORAWAN_ADR_STATE,
    .IsTxConfirmed = LORAWAN_DEFAULT_CONFIRMED_MSG_STATE,
    .TxDatarate = LORAWAN_DEFAULT_DATA_RATE,
    .PingSlotPeriodicity = LORAWAN_DEFAULT_PING_SLOT_PERIODICITY,
    .RxBCTimeout = LORAWAN_DEFAULT_CLASS_B_C_RESP_TIMEOUT};
static TxEventType_t EventType = TX_ON_TIMER;
static UTIL_TIMER_Object_t TxTimer;
static UTIL_TIMER_Time_t TxPeriodicity = APP_TX_DUTYCYCLE;
static UTIL_TIMER_Object_t StopJoinTimer;
static volatile uint8_t IsTxFramePending = 0;

void LoRaWAN_Init(void) {
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  uint32_t feature_version = 0UL;
  APP_LOG(TS_OFF, VLEVEL_M, "APPLICATION_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(APP_VERSION_MAIN), (uint8_t)(APP_VERSION_SUB1),
          (uint8_t)(APP_VERSION_SUB2));
  APP_LOG(TS_OFF, VLEVEL_M, "MW_LORAWAN_VERSION:  V%X.%X.%X\r\n",
          (uint8_t)(LORAWAN_VERSION_MAIN), (uint8_t)(LORAWAN_VERSION_SUB1),
          (uint8_t)(LORAWAN_VERSION_SUB2));
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:    V%X.%X.%X\r\n",
          (uint8_t)(SUBGHZ_PHY_VERSION_MAIN),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB1),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB2));
  LmHandlerGetVersion(LORAMAC_HANDLER_L2_VERSION, &feature_version);
  APP_LOG(TS_OFF, VLEVEL_M, "L2_SPEC_VERSION:     V%X.%X.%X\r\n",
          (uint8_t)(feature_version >> 24), (uint8_t)(feature_version >> 16),
          (uint8_t)(feature_version >> 8));
  LmHandlerGetVersion(LORAMAC_HANDLER_REGION_VERSION, &feature_version);
  APP_LOG(TS_OFF, VLEVEL_M, "RP_SPEC_VERSION:     V%X-%X.%X.%X\r\n",
          (uint8_t)(feature_version >> 24), (uint8_t)(feature_version >> 16),
          (uint8_t)(feature_version >> 8), (uint8_t)(feature_version));
  APP_LOG(TS_ON, VLEVEL_M, "Initializing Tx and Join timers..\r\n");
  UTIL_TIMER_Create(&StopJoinTimer, JOIN_TIME, UTIL_TIMER_ONESHOT,
                    OnStopJoinTimerEvent, NULL);
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LmHandlerProcess), UTIL_SEQ_RFU,
                   LmHandlerProcess);
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent),
                   UTIL_SEQ_RFU, SendTxData);
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaStoreContextEvent), UTIL_SEQ_RFU,
                   StoreContext);
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaStopJoinEvent), UTIL_SEQ_RFU,
                   StopJoin);

  LoraInfo_Init();
  LmHandlerInit(&LmHandlerCallbacks, APP_VERSION);
  LmHandlerConfigure(&LmHandlerParams);
  LmHandlerJoin(ActivationType, ForceRejoin);

  if (EventType == TX_ON_TIMER) {
    UTIL_TIMER_Create(&TxTimer, TxPeriodicity, UTIL_TIMER_ONESHOT,
                      OnTxTimerEvent, NULL);
    UTIL_TIMER_Start(&TxTimer);
  } else {
    // todo: event-based uplinks
  }
}

static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params) {
}

static void SendTxData(void) {
  if (LmHandlerIsBusy() == true) {
    return;
  }

  uint8_t isPending = 0;
  CRITICAL_SECTION_BEGIN();
  isPending = IsTxFramePending;
  IsTxFramePending = 0;
  CRITICAL_SECTION_END();

  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  if (TxPeriodicity != 60000){
	  OnTxPeriodicityChanged(60000);
  }

  if (isPending == 1) {
    LmHandlerErrorStatus_t status = LORAMAC_HANDLER_ERROR;
    uint16_t dist_mm = getSensorReading(2, 150, 7, 150, 7);				/* todo: improve timing and performance */
    uint16_t batt_level_adc = SYS_GetBatteryLevel();
    AppDataBuffer[0] = (uint8_t)dist_mm;
    APP_LOG(TS_OFF, VLEVEL_L, "Distance (Low byte): %x\n", AppDataBuffer[0]);
    AppDataBuffer[1] = (uint8_t)(dist_mm >> 8);
    APP_LOG(TS_OFF, VLEVEL_L, "Distance (High byte): %x\n", AppDataBuffer[1]);
    AppDataBuffer[2] = (uint8_t)batt_level_adc;
    APP_LOG(TS_OFF, VLEVEL_L, "Battery Level (Low byte): %x\n",
            AppDataBuffer[2]);
    AppDataBuffer[3] = (uint8_t)(batt_level_adc >> 8);
    APP_LOG(TS_OFF, VLEVEL_L, "Battery Level (High byte): %x\n",
            AppDataBuffer[3]);
    LmHandlerAppData_t appData = {
        .Buffer = AppDataBuffer, .BufferSize = 4, .Port = 1};
    status = LmHandlerSend(&appData, LORAMAC_HANDLER_UNCONFIRMED_MSG, true);
    APP_LOG(TS_ON, VLEVEL_L, "Uplink send %s\r\n", status);
  }
}

static void OnTxTimerEvent(void *context) {
  UTIL_TIMER_Stop(&TxTimer);
  IsTxFramePending = 1;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent),
                   CFG_SEQ_Prio_0);
  UTIL_TIMER_Start(&TxTimer);
}

static void OnTxData(LmHandlerTxParams_t *params) {}

static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{
}

static void OnBeaconStatusChange(LmHandlerBeaconParams_t *params) {}

static void OnClassChange(DeviceClass_t deviceClass) {}

static void OnMacProcessNotify(void) {
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LmHandlerProcess), CFG_SEQ_Prio_0);
}

static void OnTxPeriodicityChanged(uint32_t periodicity) {
  TxPeriodicity = periodicity;
  if (TxPeriodicity == 0) {
  }
  UTIL_TIMER_Stop(&TxTimer);
  UTIL_TIMER_SetPeriod(&TxTimer, TxPeriodicity);
  UTIL_TIMER_Start(&TxTimer);
}

static void OnTxFrameCtrlChanged(LmHandlerMsgTypes_t isTxConfirmed) {
  LmHandlerParams.IsTxConfirmed = isTxConfirmed;
}

static void OnPingSlotPeriodicityChanged(uint8_t pingSlotPeriodicity) {
  LmHandlerParams.PingSlotPeriodicity = pingSlotPeriodicity;
}

static void OnSystemReset(void) {
  if ((LORAMAC_HANDLER_SUCCESS == LmHandlerHalt()) &&
      (LmHandlerJoinStatus() == LORAMAC_HANDLER_SET)) {
    NVIC_SystemReset();
  }
}

static void StopJoin(void) {
  UTIL_TIMER_Stop(&TxTimer);
  if (LORAMAC_HANDLER_SUCCESS != LmHandlerStop()) {
    APP_LOG(TS_OFF, VLEVEL_M, "LmHandler Stop on going ...\r\n");
  } else {
    APP_LOG(TS_OFF, VLEVEL_M, "LmHandler Stopped\r\n");
    if (LORAWAN_DEFAULT_ACTIVATION_TYPE == ACTIVATION_TYPE_ABP) {
      ActivationType = ACTIVATION_TYPE_OTAA;
      APP_LOG(TS_OFF, VLEVEL_M, "LmHandler switch to OTAA mode\r\n");
    } else {
      ActivationType = ACTIVATION_TYPE_ABP;
      APP_LOG(TS_OFF, VLEVEL_M, "LmHandler switch to ABP mode\r\n");
    }
    LmHandlerConfigure(&LmHandlerParams);
    LmHandlerJoin(ActivationType, true);
    UTIL_TIMER_Start(&TxTimer);
  }
  UTIL_TIMER_Start(&StopJoinTimer);
}

static void OnStopJoinTimerEvent(void *context) {
  if (ActivationType == LORAWAN_DEFAULT_ACTIVATION_TYPE) {
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaStopJoinEvent), CFG_SEQ_Prio_0);
  }
}

static void StoreContext(void) {
  LmHandlerErrorStatus_t status = LORAMAC_HANDLER_ERROR;
  status = LmHandlerNvmDataStore();
  if (status == LORAMAC_HANDLER_NVM_DATA_UP_TO_DATE) {
    APP_LOG(TS_OFF, VLEVEL_M, "NVM DATA UP TO DATE\r\n");
  } else if (status == LORAMAC_HANDLER_ERROR) {
    APP_LOG(TS_OFF, VLEVEL_M, "NVM DATA STORE FAILED\r\n");
  }
}

static void OnNvmDataChange(LmHandlerNvmContextStates_t state) {
  if (state == LORAMAC_HANDLER_NVM_STORE) {
    APP_LOG(TS_OFF, VLEVEL_M, "NVM DATA STORED\r\n");
  } else {
    APP_LOG(TS_OFF, VLEVEL_M, "NVM DATA RESTORED\r\n");
  }
}

static void OnStoreContextRequest(void *nvm, uint32_t nvm_size) {
  if (HAL_FLASH_Unlock() == HAL_OK) {
    if (FLASH_IF_EraseByPages(PAGE(LORAWAN_NVM_BASE_ADDRESS), 1, 0U) ==
        FLASH_OK) {
      FLASH_IF_Write(LORAWAN_NVM_BASE_ADDRESS, (uint8_t *)nvm, nvm_size, NULL);
    }
    HAL_FLASH_Lock();
  }
}

static void OnRestoreContextRequest(void *nvm, uint32_t nvm_size) {
  UTIL_MEM_cpy_8(nvm, (void *)LORAWAN_NVM_BASE_ADDRESS, nvm_size);
}
