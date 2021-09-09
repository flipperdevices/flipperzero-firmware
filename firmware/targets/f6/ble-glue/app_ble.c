#include "main.h"

#include "app_entry.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "tl.h"
#include "app_ble.h"

#include "cmsis_os.h"
#include "shci.h"
#include "otp.h"
#include "dev_info_service.h"
#include "battery_service.h"
#include "serial_service.h"

#include <furi-hal.h>

#define BLE_TAG "BLE"

typedef struct _tSecurityParams {
  uint8_t ioCapability;
  uint8_t mitm_mode;
  uint8_t bonding_mode;
  uint8_t Use_Fixed_Pin;
  uint8_t encryptionKeySizeMin;
  uint8_t encryptionKeySizeMax;
  uint32_t Fixed_Pin;
  uint8_t initiateSecurity;
} tSecurityParams;

typedef struct _tBLEProfileGlobalContext {
  tSecurityParams bleSecurityParam;
  uint16_t gapServiceHandle;
  uint16_t devNameCharHandle;
  uint16_t appearanceCharHandle;
  uint16_t connectionHandle;
  uint8_t advtServUUIDlen;
  uint8_t advtServUUID[100];
} BleGlobalContext_t;

typedef struct {
  BleGlobalContext_t BleApplicationContext_legacy;
  APP_BLE_ConnStatus_t Device_Connection_Status;
  uint8_t Advertising_mgr_timer_Id;
} BleApplicationContext_t;


#define FAST_ADV_TIMEOUT               (30*1000*1000/CFG_TS_TICK_VAL) /**< 30s */
#define INITIAL_ADV_TIMEOUT            (60*1000*1000/CFG_TS_TICK_VAL) /**< 60s */

#define BD_ADDR_SIZE_LOCAL    6

#define LED_ON_TIMEOUT                 (0.005*1000*1000/CFG_TS_TICK_VAL) /**< 5ms */

PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static TL_CmdPacket_t BleCmdBuffer;

static const uint8_t M_bd_addr[BD_ADDR_SIZE_LOCAL] =
    {
        (uint8_t)((CFG_ADV_BD_ADDRESS & 0x0000000000FF)),
        (uint8_t)((CFG_ADV_BD_ADDRESS & 0x00000000FF00) >> 8),
        (uint8_t)((CFG_ADV_BD_ADDRESS & 0x000000FF0000) >> 16),
        (uint8_t)((CFG_ADV_BD_ADDRESS & 0x0000FF000000) >> 24),
        (uint8_t)((CFG_ADV_BD_ADDRESS & 0x00FF00000000) >> 32),
        (uint8_t)((CFG_ADV_BD_ADDRESS & 0xFF0000000000) >> 40)
    };

static uint8_t bd_addr_udn[BD_ADDR_SIZE_LOCAL];

static const uint8_t BLE_CFG_IR_VALUE[16] = CFG_BLE_IRK;
static const uint8_t BLE_CFG_ER_VALUE[16] = CFG_BLE_ERK;

PLACE_IN_SECTION("TAG_OTA_END") const uint32_t MagicKeywordValue = 0x94448A29 ;
PLACE_IN_SECTION("TAG_OTA_START") const uint32_t MagicKeywordAddress = (uint32_t)&MagicKeywordValue;

PLACE_IN_SECTION("BLE_APP_CONTEXT") static BleApplicationContext_t BleApplicationContext;
PLACE_IN_SECTION("BLE_APP_CONTEXT") static uint16_t AdvIntervalMin, AdvIntervalMax;

uint8_t  manuf_data[14] = {
    sizeof(manuf_data)-1, AD_TYPE_MANUFACTURER_SPECIFIC_DATA,
    0x01/*SKD version */,
    0x00 /* Generic*/,
    0x00 /* GROUP A Feature  */,
    0x00 /* GROUP A Feature */,
    0x00 /* GROUP B Feature */,
    0x00 /* GROUP B Feature */,
    0x00, /* BLE MAC start -MSB */
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, /* BLE MAC stop */

};

osMutexId_t MtxHciId;
osSemaphoreId_t SemHciId;
osThreadId_t AdvUpdateProcessId;
osThreadId_t HciUserEvtProcessId;

const osThreadAttr_t AdvUpdateProcess_attr = {
    .name = CFG_ADV_UPDATE_PROCESS_NAME,
    .attr_bits = CFG_ADV_UPDATE_PROCESS_ATTR_BITS,
    .cb_mem = CFG_ADV_UPDATE_PROCESS_CB_MEM,
    .cb_size = CFG_ADV_UPDATE_PROCESS_CB_SIZE,
    .stack_mem = CFG_ADV_UPDATE_PROCESS_STACK_MEM,
    .priority = CFG_ADV_UPDATE_PROCESS_PRIORITY,
    .stack_size = CFG_ADV_UPDATE_PROCESS_STACK_SIZE
};

const osThreadAttr_t HciUserEvtProcess_attr = {
    .name = CFG_HCI_USER_EVT_PROCESS_NAME,
    .attr_bits = CFG_HCI_USER_EVT_PROCESS_ATTR_BITS,
    .cb_mem = CFG_HCI_USER_EVT_PROCESS_CB_MEM,
    .cb_size = CFG_HCI_USER_EVT_PROCESS_CB_SIZE,
    .stack_mem = CFG_HCI_USER_EVT_PROCESS_STACK_MEM,
    .priority = CFG_HCI_USER_EVT_PROCESS_PRIORITY,
    .stack_size = CFG_HCI_USER_EVT_PROCESS_STACK_SIZE
};

/* Private function prototypes -----------------------------------------------*/
static void HciUserEvtProcess(void *argument);
static void BLE_UserEvtRx( void * pPayload );
static void BLE_StatusNot( HCI_TL_CmdStatus_t status );
static void Ble_Tl_Init( void );
static void Ble_Hci_Gap_Gatt_Init();
static const uint8_t* BleGetBdAddress( void );
static void Adv_Request( APP_BLE_ConnStatus_t New_Status );
static void Adv_Mgr( void );
static void AdvUpdateProcess(void *argument);
static void Adv_Update( void );


bool APP_BLE_Init() {
  SHCI_C2_Ble_Init_Cmd_Packet_t ble_init_cmd_packet = {
    {{0,0,0}},                  /**< Header unused */
    {0,                         /** pBleBufferAddress not used */
    0,                          /** BleBufferSize not used */
    CFG_BLE_NUM_GATT_ATTRIBUTES,
    CFG_BLE_NUM_GATT_SERVICES,
    CFG_BLE_ATT_VALUE_ARRAY_SIZE,
    CFG_BLE_NUM_LINK,
    CFG_BLE_DATA_LENGTH_EXTENSION,
    CFG_BLE_PREPARE_WRITE_LIST_SIZE,
    CFG_BLE_MBLOCK_COUNT,
    CFG_BLE_MAX_ATT_MTU,
    CFG_BLE_SLAVE_SCA,
    CFG_BLE_MASTER_SCA,
    CFG_BLE_LSE_SOURCE,
    CFG_BLE_MAX_CONN_EVENT_LENGTH,
    CFG_BLE_HSE_STARTUP_TIME,
    CFG_BLE_VITERBI_MODE,
    CFG_BLE_LL_ONLY,
    0}
  };

  // Initialize Ble Transport Layer
  Ble_Tl_Init( );
  // Register the hci transport layer to handle BLE User Asynchronous Events
  HciUserEvtProcessId = osThreadNew(HciUserEvtProcess, NULL, &HciUserEvtProcess_attr);
  // Starts the BLE Stack on CPU2
  return (SHCI_C2_BLE_Init( &ble_init_cmd_packet ) == SHCI_Success);
}

static void set_advertisment_service_uid(uint8_t* uid, uint8_t uin_len);

bool APP_BLE_Start() {
  if (APPE_Status() != BleGlueStatusStarted) {
    return false;
  }
  // Initialization of HCI & GATT & GAP layer
  Ble_Hci_Gap_Gatt_Init();
  // Initialization of the BLE Services
  SVCCTL_Init();
  // Initialization of the BLE App Context
  BleApplicationContext.Device_Connection_Status = APP_BLE_IDLE;
  BleApplicationContext.BleApplicationContext_legacy.connectionHandle = 0xFFFF;
  // From here, all initialization are BLE application specific
  AdvUpdateProcessId = osThreadNew(AdvUpdateProcess, NULL, &AdvUpdateProcess_attr);

  // Initialization of ADV - Ad Manufacturer Element - Support OTA Bit Masks
#if(BLE_CFG_OTA_REBOOT_CHAR != 0)
  manuf_data[sizeof(manuf_data)-8] = CFG_FEATURE_OTA_REBOOT;
#endif

  // Initialize DIS Application
  dev_info_service_init();
  // Initialize BAS Application
  battery_svc_init();
  // Initialize Serial application
  serial_svc_init();
  // Create timer to handle the connection state machine
  HW_TS_Create(CFG_TIM_PROC_ID_ISR, &(BleApplicationContext.Advertising_mgr_timer_Id), hw_ts_SingleShot, Adv_Mgr);
  uint8_t adv_service_uid[2];
  adv_service_uid[0] = 0x80 | furi_hal_version_get_hw_color();
  adv_service_uid[1] = 0x30;

  set_advertisment_service_uid(adv_service_uid, sizeof(adv_service_uid));
  /* Initialize intervals for reconnexion without intervals update */
  AdvIntervalMin = CFG_FAST_CONN_ADV_INTERVAL_MIN;
  AdvIntervalMax = CFG_FAST_CONN_ADV_INTERVAL_MAX;

  Adv_Request(APP_BLE_FAST_ADV);
  return true;
}

void SVCCTL_SvcInit() {
    // Dummy function to prevent unused services initialization
    // TODO refactor
}

SVCCTL_UserEvtFlowStatus_t SVCCTL_App_Notification( void *pckt )
{
  hci_event_pckt *event_pckt;
  evt_le_meta_event *meta_evt;
  evt_blue_aci *blue_evt;
  hci_le_phy_update_complete_event_rp0 *evt_le_phy_update_complete;
  uint8_t TX_PHY, RX_PHY;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;

  event_pckt = (hci_event_pckt*) ((hci_uart_pckt *) pckt)->data;

  switch (event_pckt->evt) {
    case EVT_DISCONN_COMPLETE:
    {
      hci_disconnection_complete_event_rp0 *disconnection_complete_event = (hci_disconnection_complete_event_rp0 *) event_pckt->data;

      if (disconnection_complete_event->Connection_Handle == BleApplicationContext.BleApplicationContext_legacy.connectionHandle) {
        BleApplicationContext.BleApplicationContext_legacy.connectionHandle = 0;
        BleApplicationContext.Device_Connection_Status = APP_BLE_IDLE;
        FURI_LOG_I(BLE_TAG, "DISCONNECTION EVENT WITH CLIENT ");
      }
      // Restart advertising
      Adv_Request(APP_BLE_FAST_ADV);
      furi_hal_power_insomnia_exit();
    }
    break;

    case EVT_LE_META_EVENT:
      meta_evt = (evt_le_meta_event*) event_pckt->data;
      switch (meta_evt->subevent) {
        case EVT_LE_CONN_UPDATE_COMPLETE:
          FURI_LOG_I(BLE_TAG, "Connection uodate event");
          break;

        case EVT_LE_PHY_UPDATE_COMPLETE:
          FURI_LOG_I(BLE_TAG, "EVT_UPDATE_PHY_COMPLETE ");
          evt_le_phy_update_complete = (hci_le_phy_update_complete_event_rp0*)meta_evt->data;
          if(evt_le_phy_update_complete->Status) {
            FURI_LOG_E(BLE_TAG, "Update PHY failed, status %d", evt_le_phy_update_complete->Status);
          } else {
            FURI_LOG_I(BLE_TAG, "Update PHY succeed");
          }
          ret = hci_le_read_phy(BleApplicationContext.BleApplicationContext_legacy.connectionHandle,&TX_PHY,&RX_PHY);
          if(ret) {
            FURI_LOG_E(BLE_TAG, "Read PHY failed, status: %d", ret);
          } else {
            FURI_LOG_I(BLE_TAG, "PHY Params  TX= %d, RX= %d ", TX_PHY, RX_PHY);
          }
          break;

        case EVT_LE_CONN_COMPLETE:
          furi_hal_power_insomnia_enter();
          hci_le_connection_complete_event_rp0* connection_complete_event = (hci_le_connection_complete_event_rp0 *) meta_evt->data;
          FURI_LOG_I(BLE_TAG, "Connection complete for connection handle 0x%x", connection_complete_event->Connection_Handle);

          // Stop advertising as connection completed
          HW_TS_Stop(BleApplicationContext.Advertising_mgr_timer_Id);

          // Update connection status and handle
          BleApplicationContext.Device_Connection_Status = APP_BLE_CONNECTED_SERVER;
          BleApplicationContext.BleApplicationContext_legacy.connectionHandle = connection_complete_event->Connection_Handle;

          // Start pairing by sending security request
          aci_gap_slave_security_req(connection_complete_event->Connection_Handle);
        break;

        default:
          break;
      }
    break;

    case EVT_VENDOR:
      blue_evt = (evt_blue_aci*) event_pckt->data;
      switch (blue_evt->ecode) {
        aci_gap_pairing_complete_event_rp0 *pairing_complete;

      case EVT_BLUE_GAP_LIMITED_DISCOVERABLE: 
        FURI_LOG_I(BLE_TAG, "Limited discoverable event");
          break;
          
      case EVT_BLUE_GAP_PASS_KEY_REQUEST:  
        aci_gap_pass_key_resp(
          BleApplicationContext.BleApplicationContext_legacy.connectionHandle,
          BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Fixed_Pin);
        FURI_LOG_I(BLE_TAG, "Pass key request event. Pin: %d", BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Fixed_Pin);
          break;

      case EVT_BLUE_GAP_AUTHORIZATION_REQUEST:    
        FURI_LOG_I(BLE_TAG, "Authorization request event");
          break;

      case EVT_BLUE_GAP_SLAVE_SECURITY_INITIATED:
        FURI_LOG_I(BLE_TAG, "Slave security initiated");
          break;

      case EVT_BLUE_GAP_BOND_LOST:    
        FURI_LOG_I(BLE_TAG, "Bond lost event. Start rebonding");
          aci_gap_allow_rebond(BleApplicationContext.BleApplicationContext_legacy.connectionHandle);
        FURI_LOG_I(BLE_TAG, "Send allow rebond ");
          break; /* EVT_BLUE_GAP_BOND_LOST */

      case EVT_BLUE_GAP_DEVICE_FOUND:  
        FURI_LOG_I(BLE_TAG, "EVT_BLUE_GAP_DEVICE_FOUND ");
          break; /* EVT_BLUE_GAP_DEVICE_FOUND */

      case EVT_BLUE_GAP_ADDR_NOT_RESOLVED:
         FURI_LOG_I(BLE_TAG, "EVT_BLUE_GAP_DEVICE_FOUND ");
          break; /* EVT_BLUE_GAP_DEVICE_FOUND */
      
      case (EVT_BLUE_GAP_KEYPRESS_NOTIFICATION):
         FURI_LOG_I(BLE_TAG, "EVT_BLUE_GAP_KEYPRESS_NOTIFICATION ");
          break; /* EVT_BLUE_GAP_KEY_PRESS_NOTIFICATION */    

       case (EVT_BLUE_GAP_NUMERIC_COMPARISON_VALUE):
          FURI_LOG_I(BLE_TAG, "numeric_value = %ld",
                      ((aci_gap_numeric_comparison_value_event_rp0 *)(blue_evt->data))->Numeric_Value);

          FURI_LOG_I(BLE_TAG, "Hex_value = %lx",
                      ((aci_gap_numeric_comparison_value_event_rp0 *)(blue_evt->data))->Numeric_Value);

          aci_gap_numeric_comparison_value_confirm_yesno(BleApplicationContext.BleApplicationContext_legacy.connectionHandle, 1); /* CONFIRM_YES = 1 */

          FURI_LOG_I(BLE_TAG, "aci_gap_numeric_comparison_value_confirm_yesno-->YES ");
          break;

        case (EVT_BLUE_GAP_PAIRING_CMPLT):
          {
            pairing_complete = (aci_gap_pairing_complete_event_rp0*)blue_evt->data;
            if (pairing_complete->Status) {
              FURI_LOG_E(BLE_TAG, "Pairing failed with status: %d. Terminating connection", pairing_complete->Status);
              aci_gap_terminate(BleApplicationContext.BleApplicationContext_legacy.connectionHandle, 5);
            } else {
              FURI_LOG_I(BLE_TAG, "Pairing complete");
            }
          }
          break;

        case EVT_BLUE_GAP_PROCEDURE_COMPLETE:
          FURI_LOG_I(BLE_TAG, "EVT_BLUE_GAP_PROCEDURE_COMPLETE ");
          break;
      }
      break;
      default:
        break;
  }

  return (SVCCTL_UserEvtFlowEnable);
}

static void set_advertisment_service_uid(uint8_t* uid, uint8_t uid_len) {
    BleApplicationContext.BleApplicationContext_legacy.advtServUUIDlen = 1;
    if(uid_len == 2) {
        BleApplicationContext.BleApplicationContext_legacy.advtServUUID[0] = AD_TYPE_16_BIT_SERV_UUID;
    } else if (uid_len == 4) {
        BleApplicationContext.BleApplicationContext_legacy.advtServUUID[0] = AD_TYPE_32_BIT_SERV_UUID;
    } else if(uid_len == 16) {
        BleApplicationContext.BleApplicationContext_legacy.advtServUUID[0] = AD_TYPE_128_BIT_SERV_UUID_CMPLT_LIST;
    }
    memcpy(&BleApplicationContext.BleApplicationContext_legacy.advtServUUID[1], uid, uid_len);
    BleApplicationContext.BleApplicationContext_legacy.advtServUUIDlen += uid_len;
}

APP_BLE_ConnStatus_t APP_BLE_Get_Server_Connection_Status() {
    return BleApplicationContext.Device_Connection_Status;
}

static void Ble_Tl_Init( void ) {
  HCI_TL_HciInitConf_t Hci_Tl_Init_Conf;

  MtxHciId = osMutexNew( NULL );
  SemHciId = osSemaphoreNew( 1, 0, NULL ); /*< Create the semaphore and make it busy at initialization */

  Hci_Tl_Init_Conf.p_cmdbuffer = (uint8_t*)&BleCmdBuffer;
  Hci_Tl_Init_Conf.StatusNotCallBack = BLE_StatusNot;
  hci_init(BLE_UserEvtRx, (void*) &Hci_Tl_Init_Conf);
}

static void Ble_Hci_Gap_Gatt_Init() {
  uint8_t role;
  uint16_t gap_service_handle, gap_dev_name_char_handle, gap_appearance_char_handle;
  const uint8_t *bd_addr;
  uint32_t srd_bd_addr[2];
  uint16_t appearance[1] = { BLE_CFG_GAP_APPEARANCE };

  /*HCI Reset to synchronise BLE Stack*/
  hci_reset();

  /**
   * Write the BD Address
   */
  bd_addr = BleGetBdAddress();
  aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                            CONFIG_DATA_PUBADDR_LEN,
                            (uint8_t*) bd_addr);

  /* BLE MAC in ADV Packet */
  manuf_data[ sizeof(manuf_data)-6] = bd_addr[5];
  manuf_data[ sizeof(manuf_data)-5] = bd_addr[4];
  manuf_data[ sizeof(manuf_data)-4] = bd_addr[3];
  manuf_data[ sizeof(manuf_data)-3] = bd_addr[2];
  manuf_data[ sizeof(manuf_data)-2] = bd_addr[1];
  manuf_data[ sizeof(manuf_data)-1] = bd_addr[0];

  /**
   * Write Identity root key used to derive LTK and CSRK
   */
    aci_hal_write_config_data(CONFIG_DATA_IR_OFFSET,
    CONFIG_DATA_IR_LEN,
                            (uint8_t*) BLE_CFG_IR_VALUE);

   /**
   * Write Encryption root key used to derive LTK and CSRK
   */
    aci_hal_write_config_data(CONFIG_DATA_ER_OFFSET,
    CONFIG_DATA_ER_LEN,
                            (uint8_t*) BLE_CFG_ER_VALUE);

   /**
   * Write random bd_address
   */
   /* random_bd_address = R_bd_address;
    aci_hal_write_config_data(CONFIG_DATA_RANDOM_ADDRESS_WR,
    CONFIG_DATA_RANDOM_ADDRESS_LEN,
                            (uint8_t*) random_bd_address);
  */

  /**
   * Static random Address
   * The two upper bits shall be set to 1
   * The lowest 32bits is read from the UDN to differentiate between devices
   * The RNG may be used to provide a random number on each power on
   */
  srd_bd_addr[1] =  0x0000ED6E;
  srd_bd_addr[0] =  LL_FLASH_GetUDN( );
  aci_hal_write_config_data( CONFIG_DATA_RANDOM_ADDRESS_OFFSET, CONFIG_DATA_RANDOM_ADDRESS_LEN, (uint8_t*)srd_bd_addr );

  /**
   * Write Identity root key used to derive LTK and CSRK
   */
    aci_hal_write_config_data( CONFIG_DATA_IR_OFFSET, CONFIG_DATA_IR_LEN, (uint8_t*)BLE_CFG_IR_VALUE );

   /**
   * Write Encryption root key used to derive LTK and CSRK
   */
    aci_hal_write_config_data( CONFIG_DATA_ER_OFFSET, CONFIG_DATA_ER_LEN, (uint8_t*)BLE_CFG_ER_VALUE );

  /**
   * Set TX Power to 0dBm.
   */
  aci_hal_set_tx_power_level(1, CFG_TX_POWER);

  /**
   * Initialize GATT interface
   */
  aci_gatt_init();

  /**
   * Initialize GAP interface
   */
  role = 0;

#if (BLE_CFG_PERIPHERAL == 1)
  role |= GAP_PERIPHERAL_ROLE;
#endif

#if (BLE_CFG_CENTRAL == 1)
  role |= GAP_CENTRAL_ROLE;
#endif

  if (role > 0)
  {
    const char *name = furi_hal_version_get_device_name_ptr();
    aci_gap_init(role, 0,
                 strlen(name),
                 &gap_service_handle, &gap_dev_name_char_handle, &gap_appearance_char_handle);

    if (aci_gatt_update_char_value(gap_service_handle, gap_dev_name_char_handle, 0, strlen(name), (uint8_t *) name))
    {
      BLE_DBG_SVCCTL_MSG("Device Name aci_gatt_update_char_value failed.");
    }
  }

  if(aci_gatt_update_char_value(gap_service_handle,
                                gap_appearance_char_handle,
                                0,
                                2,
                                (uint8_t *)&appearance))
  {
    BLE_DBG_SVCCTL_MSG("Appearance aci_gatt_update_char_value failed.");
  }
  /**
   * Initialize Default PHY
   */
  hci_le_set_default_phy(ALL_PHYS_PREFERENCE,TX_2M_PREFERRED,RX_2M_PREFERRED);

  /**
   * Initialize IO capability
   */
  BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.ioCapability = IO_CAP_DISPLAY_ONLY;
  aci_gap_set_io_capability(BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.ioCapability);

  /**
   * Initialize authentication
   */
  BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.mitm_mode = 1;
  BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMin = CFG_ENCRYPTION_KEY_SIZE_MIN;
  BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMax = CFG_ENCRYPTION_KEY_SIZE_MAX;
  BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Use_Fixed_Pin = 1;
  BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Fixed_Pin = 123321;
  BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode = 1;

  aci_gap_set_authentication_requirement(BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode,
                                         BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.mitm_mode,
                                         1,
                                         0,
                                         BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMin,
                                         BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMax,
                                         BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Use_Fixed_Pin,
                                         BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Fixed_Pin,
                                         PUBLIC_ADDR
                                         );

  /**
   * Initialize whitelist
   */
   if (BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode)
   {
     aci_gap_configure_whitelist();
   }
}

static void Adv_Request(APP_BLE_ConnStatus_t New_Status)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  uint16_t Min_Inter, Max_Inter;

  if (New_Status == APP_BLE_FAST_ADV)
  {
    Min_Inter = AdvIntervalMin;
    Max_Inter = AdvIntervalMax;
  }
  else
  {
    Min_Inter = CFG_LP_CONN_ADV_INTERVAL_MIN;
    Max_Inter = CFG_LP_CONN_ADV_INTERVAL_MAX;
  }

    /**
     * Stop the timer, it will be restarted for a new shot
     * It does not hurt if the timer was not running
     */
    HW_TS_Stop(BleApplicationContext.Advertising_mgr_timer_Id);

    FURI_LOG_I(BLE_TAG, "First index in %d state ", BleApplicationContext.Device_Connection_Status);

    if ((New_Status == APP_BLE_LP_ADV)
        && ((BleApplicationContext.Device_Connection_Status == APP_BLE_FAST_ADV)
            || (BleApplicationContext.Device_Connection_Status == APP_BLE_LP_ADV)))
    {
      /* Connection in ADVERTISE mode have to stop the current advertising */
      ret = aci_gap_set_non_discoverable();
      if (ret == BLE_STATUS_SUCCESS)
      {
        FURI_LOG_I(BLE_TAG, "Successfully Stopped Advertising ");
      }
      else
      {
        FURI_LOG_E(BLE_TAG, "Stop Advertising Failed, result: %d", ret);
      }
    }

    BleApplicationContext.Device_Connection_Status = New_Status;

    const char* name = furi_hal_version_get_ble_local_device_name_ptr();

    /* Start Fast or Low Power Advertising */
    ret = aci_gap_set_discoverable(
        ADV_IND,
        Min_Inter,
        Max_Inter,
        PUBLIC_ADDR,
        0,
        strlen(name),
        (uint8_t*)name,
        BleApplicationContext.BleApplicationContext_legacy.advtServUUIDlen,
        BleApplicationContext.BleApplicationContext_legacy.advtServUUID,
        0,
        0);
    if(ret) {
      FURI_LOG_E("APP ble", "Set discoverable err: %d", ret);
    }
    HW_TS_Start(BleApplicationContext.Advertising_mgr_timer_Id, INITIAL_ADV_TIMEOUT);
}

const uint8_t* BleGetBdAddress( void ) {
  uint8_t *otp_addr;
  const uint8_t *bd_addr;
  uint32_t udn;
  uint32_t company_id;
  uint32_t device_id;

  udn = LL_FLASH_GetUDN();

  if(udn != 0xFFFFFFFF) {
    company_id = LL_FLASH_GetSTCompanyID();
    device_id = LL_FLASH_GetDeviceID();

    bd_addr_udn[0] = (uint8_t)(udn & 0x000000FF);
    bd_addr_udn[1] = (uint8_t)( (udn & 0x0000FF00) >> 8 );
    bd_addr_udn[2] = (uint8_t)( (udn & 0x00FF0000) >> 16 );
    bd_addr_udn[3] = (uint8_t)device_id;
    bd_addr_udn[4] = (uint8_t)(company_id & 0x000000FF);;
    bd_addr_udn[5] = (uint8_t)( (company_id & 0x0000FF00) >> 8 );

    bd_addr = (const uint8_t *)bd_addr_udn;
  } else {
    otp_addr = OTP_Read(0);
    if(otp_addr) {
      bd_addr = ((OTP_ID0_t*)otp_addr)->bd_address;
    } else {
      bd_addr = M_bd_addr;
    }
  }

  return bd_addr;
}

/*************************************************************
 *
 *SPECIFIC FUNCTIONS
 *
 *************************************************************/
static void Adv_Mgr( void ) {
  /**
   * The code shall be executed in the background as an aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  osThreadFlagsSet( AdvUpdateProcessId, 1 );
}

static void AdvUpdateProcess(void *argument) {
  UNUSED(argument);

  for(;;) {
    osThreadFlagsWait( 1, osFlagsWaitAny, osWaitForever);
    Adv_Update( );
  }
}

static void Adv_Update( void ) {
  Adv_Request(APP_BLE_LP_ADV);

}

static void HciUserEvtProcess(void *argument) {
  UNUSED(argument);

  for(;;)
  {
    osThreadFlagsWait( 1, osFlagsWaitAny, osWaitForever);
    hci_user_evt_proc( );
  }
}

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/
void hci_notify_asynch_evt(void* pdata) {
  UNUSED(pdata);
  osThreadFlagsSet( HciUserEvtProcessId, 1 );
}

void hci_cmd_resp_release(uint32_t flag) {
  UNUSED(flag);
  osSemaphoreRelease( SemHciId );
}

void hci_cmd_resp_wait(uint32_t timeout) {
  UNUSED(timeout);
  osSemaphoreAcquire( SemHciId, osWaitForever );
}

static void BLE_UserEvtRx( void * pPayload ) {
  SVCCTL_UserEvtFlowStatus_t svctl_return_status;
  tHCI_UserEvtRxParam *pParam;

  pParam = (tHCI_UserEvtRxParam *)pPayload;

  svctl_return_status = SVCCTL_UserEvtRx((void *)&(pParam->pckt->evtserial));
  if (svctl_return_status != SVCCTL_UserEvtFlowDisable) {
    pParam->status = HCI_TL_UserEventFlow_Enable;
  } else {
    pParam->status = HCI_TL_UserEventFlow_Disable;
  }
}

static void BLE_StatusNot( HCI_TL_CmdStatus_t status ) {
  switch (status) {
    case HCI_TL_CmdBusy:
      osMutexAcquire( MtxHciId, osWaitForever );
      break;
    case HCI_TL_CmdAvailable:
      osMutexRelease( MtxHciId );
      break;
    default:
      break;
  }
}

void SVCCTL_ResumeUserEventFlow( void ) {
  hci_resume_flow();
}
