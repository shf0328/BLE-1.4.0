/**************************************************************************************************
  Filename:       simpleBLEPeripheral.c
 
  Description:    This file contains the Simple BLE Peripheral sample application
                  for use with the CC2540 Bluetooth Low Energy Protocol Stack.

**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "PN532_NFC.h"
#include "NfcTask.h"
#include "bcomdef.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"

#include "OnBoard.h"
#include "hal_adc.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_lcd.h"

#include "gatt.h"
#include "Osal_snv.h"
#include "Osal_Nv.h"
#include "hci.h"
#include "flash_operate.h"
//#include "FlashOperate.h"

#include "gapgattserver.h"
#include "gattservapp.h"
#include "devinfoservice.h"
#include "simpleGATTprofile.h"

#if defined( CC2540_MINIDK )
  #include "simplekeys.h"
#endif

#include "peripheral.h"

#include "gapbondmgr.h"

#include "simpleBLEPeripheral.h"

#if defined FEATURE_OAD
  #include "oad.h"
  #include "oad_target.h"
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// How often to perform periodic event
#define SBP_PERIODIC_EVT_PERIOD                   5000

// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// Limited discoverable mode advertises for 30.72s, and then stops
// General discoverable mode advertises indefinitely

#if defined ( CC2540_MINIDK )
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_LIMITED
#else
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL
#endif  // defined ( CC2540_MINIDK )

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     80

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         TRUE

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         6

// Company Identifier: Texas Instruments Inc. (13)
#define TI_COMPANY_ID                         0x000D

#define INVALID_CONNHANDLE                    0xFFFF

// Length of bd addr as a string
#define B_ADDR_STR_LEN                        15

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
 extern uint8 start;
 extern uint8 ble_count;
 extern uint8 ble_state;
 extern uint8 next;
 extern uint8 p;


/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 simpleBLEPeripheral_TaskID;   // Task ID for internal task/event processing
//按键回调函数里的"seq"
uint8 s=0;

uint8 seq=0x84;
uint8 value=0;
uint8 index=0;
uint8 p1=0; //total pages not blank in flash

/*********************************************************************
特征值                 UUID    初始权限	         验证后权限	
主机命令位	FFF1	                NONE	              	Read&Write	
app_notif       FFF3                  NONE                        WriteOnly
通知主机位	FFF4	              ReadOnly                    Read&Write
机器储存密码	FFF6	          	   NONE	              	Read&Write	
手机写入密码	FFF7	              WriteOnly	              Read&Write	
数据块1	             FFF8	                 NONE	               	Read&Write
数据块2	             FFF9	                 NONE	               	Read&Write	
*/
uint8 identity= 0;  // 0为初始权限，1为验证后权限

/*********************************************************************
ble通知
0 密码配对不成功，无权限
1 密码配对成功，获得权限
2 从机准备写入
3 写入数据包成功
4 从机准备读出
5 读出数据包成功
+10 索引号
*/
uint8 notification= 0;


static gaprole_States_t gapProfileState = GAPROLE_INIT;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8 scanRspData[] =
{
  // complete name
  0x14,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  0x53,   // 'S'
  0x69,   // 'i'
  0x6d,   // 'm'
  0x70,   // 'p'
  0x6c,   // 'l'
  0x65,   // 'e'
  0x42,   // 'B'
  0x4c,   // 'L'
  0x45,   // 'E'
  0x50,   // 'P'
  0x65,   // 'e'
  0x72,   // 'r'
  0x69,   // 'i'
  0x70,   // 'p'
  0x68,   // 'h'
  0x65,   // 'e'
  0x72,   // 'r'
  0x61,   // 'a'
  0x6c,   // 'l'

  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),   // 100ms
  HI_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),
  LO_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),   // 1s
  HI_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8 advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
  0x03,   // length of this data
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
  LO_UINT16( SIMPLEPROFILE_SERV_UUID ),
  HI_UINT16( SIMPLEPROFILE_SERV_UUID ),

};

// GAP GATT Attributes
static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "Simple BLE Peripheral";

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void simpleBLEPeripheral_ProcessOSALMsg( osal_event_hdr_t *pMsg );
static void peripheralStateNotificationCB( gaprole_States_t newState );
static void performPeriodicTask( void );
static void simpleProfileChangeCB( uint8 paramID );

/*
// Application states
enum
{
  BLE_STATE_IDLE,
  BLE_STATE_CONNECTED,
};
// Application state
static uint8 simpleBLEState = BLE_STATE_IDLE;
static uint8 gPairStatus=0;/*用来管理当前的状态，如果密码不正确，立即取消连接，0表示未配对，1表示已配对*/
/*void ProcessPasscodeCB(uint8 *deviceAddr,uint16 connectionHandle,uint8 uiInputs,uint8 uiOutputs );
static void ProcessPairStateCB( uint16 connHandle, uint8 state, uint8 status );
*/

#if defined( CC2540_MINIDK )
static void simpleBLEPeripheral_HandleKeys( uint8 shift, uint8 keys );
#endif

#if (defined HAL_LCD) && (HAL_LCD == TRUE)
static char *bdAddr2Str ( uint8 *pAddr );
#endif // (defined HAL_LCD) && (HAL_LCD == TRUE)



/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t simpleBLEPeripheral_PeripheralCBs =
{
  peripheralStateNotificationCB,  // Profile State Change Callbacks
  NULL                            // When a valid RSSI is read from controller (not used by application)
};

// GAP Bond Manager Callbacks
static gapBondCBs_t simpleBLEPeripheral_BondMgrCBs =
{
  NULL,                     // Passcode callback (not used by application)
  NULL                      // Pairing / Bonding state Callback (not used by application)
};

// Simple GATT Profile Callbacks
static simpleProfileCBs_t simpleBLEPeripheral_SimpleProfileCBs =
{
  simpleProfileChangeCB    // Charactersitic value change callback
};
/*********************************************************************
 * PUBLIC FUNCTIONS
 */



/*********************************************************************
 * @fn      simpleBLEPeripheral_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 
 *                 
 *
 * @return  none
 */
static void simpleBLEPeripheral_HandleKeys( uint8 shift, uint8 keys )
{

  uint8 SK_Keys = 0;

  VOID shift;  // Intentionally unreferenced parameter

  //HalLcdWriteStringValue( "key = 0x", keys, 16, HAL_LCD_LINE_3 );
  
 /* if(keys& HAL_KEY_SW_6)
  {
    HalLedBlink (HAL_LED_1, 5, 50, 2000);
  }*/
  
 
  if ( keys & HAL_KEY_UP )
  {  
    HalLcdWriteString( "HAL_KEY_UP", HAL_LCD_LINE_5 );
    if(index<INFO_LENGTH)
    {
      index++;
    }
    HalLcdWriteStringValue( "INDEX = ", index, 10, HAL_LCD_LINE_6 );
    //osal_changepowerstate(SAVEPOWER);
    
  }

 /* if ( keys & HAL_KEY_LEFT )
  {
    //HalLcdWriteString( "HAL_KEY_LEFT", HAL_LCD_LINE_5 );
    /*uint8 temp=0;
    temp=flash_Rinfo_single_write(seq,index,index);
    //HalLcdWriteStringValue( "send VALUE = ", send[seq], 10, HAL_LCD_LINE_6 );
    HalLcdWriteStringValue( "W VALUE = ", temp, 10, HAL_LCD_LINE_7 );*/
    //test[1]=test[1]+1;
    //flash_Recinfo_Compare_Save(test);
/*    uint8 temp[3]={0};
    flash_getSerialNumber(temp);
    HalLcdWriteStringValue( "1 VALUE = ", temp[0], 10, HAL_LCD_LINE_6 );
    HalLcdWriteStringValue( "2 VALUE = ", temp[1], 10, HAL_LCD_LINE_7 );
    HalLcdWriteStringValue( "3 VALUE = ", temp[2], 10, HAL_LCD_LINE_8 );
    //
    uint8 temp=0;
    temp=flash_Tinfo_single_write(index,index);
  }*/

  if ( keys & HAL_KEY_RIGHT )
  {
    HalLcdWriteString( "HAL_KEY_RIGHT", HAL_LCD_LINE_5 );
    uint8 temp=0;
    temp=flash_Rinfo_single_read(0x94,index);
    //HalLcdWriteStringValue( "rcv VALUE = ", receive[seq], 10, HAL_LCD_LINE_6 );
    HalLcdWriteStringValue( "R VALUE = ", temp, 10, HAL_LCD_LINE_7 );
  }
  
  if ( keys & HAL_KEY_CENTER )
  {
    HalLcdWriteString( "HAL_KEY_CENTER", HAL_LCD_LINE_5 );
    /*seq=seq+2;
    if(seq>0x8C)
    {
      seq=0x84;
    }
    HalLcdWriteStringValue( "seq = 0x", seq, 16, HAL_LCD_LINE_8 );
    //osal_changepowerstate(0);
    uint8 temp[3]={0};
    flash_getSerialNumber(temp);
    HalLcdWriteStringValue( "1 VALUE = ", temp[0], 10, HAL_LCD_LINE_6 );
    HalLcdWriteStringValue( "2 VALUE = ", temp[1], 10, HAL_LCD_LINE_7 );
    HalLcdWriteStringValue( "3 VALUE = ", temp[2], 10, HAL_LCD_LINE_8 );*/

    osal_event_hdr_t *msgPtr;
    msgPtr = (osal_event_hdr_t *)osal_msg_allocate( sizeof(osal_event_hdr_t) );
    if ( msgPtr )
    {
      msgPtr->event=SOCIAL;
      osal_msg_send( 12, (uint8 *)msgPtr );
    }

    
  }
  
  if ( keys & HAL_KEY_DOWN )
  {
    if(index>0)
    {
      index--;
    }
    HalLcdWriteStringValue( "INDEX = ", index, 10, HAL_LCD_LINE_6 );
    
    //HalLedBlink (HAL_LED_1, 5, 50, 2000);
  }
}


/*********************************************************************
 * @fn      SimpleBLEPeripheral_Init
 *
 * @brief   Initialization function for the Simple BLE Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SimpleBLEPeripheral_Init( uint8 task_id )
{
  simpleBLEPeripheral_TaskID = task_id;

  
   
   
  //初始化权限
      identity=0;
  
  // Setup the GAP
  VOID GAP_SetParamValue( TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL );
  
  // Setup the GAP Peripheral Role Profile
  {
    #if defined( CC2540_MINIDK )
      // For the CC2540DK-MINI keyfob, device doesn't start advertising until button is pressed
      uint8 initial_advertising_enable = FALSE;
    #else
      // For other hardware platforms, device starts advertising upon initialization
      uint8 initial_advertising_enable = TRUE;
    #endif

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16 gapRole_AdvertOffTime = 0;

    uint8 enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16 desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16 desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16 desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16 desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
    GAPRole_SetParameter( GAPROLE_ADVERT_OFF_TIME, sizeof( uint16 ), &gapRole_AdvertOffTime );

    GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( scanRspData ), scanRspData );
    GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advertData ), advertData );

    GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
    GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &desired_min_interval );
    GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &desired_max_interval );
    GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &desired_slave_latency );
    GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &desired_conn_timeout );
  }

  // Set the GAP Characteristics
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName );

  // Set advertising interval
  {
    uint16 advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MIN, advInt );
    GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MAX, advInt );
    GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, advInt );
    GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, advInt );
  }

  // Setup the GAP Bond Manager
  {
    uint32 passkey = 0; // passkey "000000"
    uint8 pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    uint8 mitm = TRUE;
    uint8 ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    uint8 bonding = TRUE;
    GAPBondMgr_SetParameter( GAPBOND_DEFAULT_PASSCODE, sizeof ( uint32 ), &passkey );
    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8 ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8 ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8 ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_BONDING_ENABLED, sizeof ( uint8 ), &bonding );
  }

  // Initialize GATT attributes
  GGS_AddService( GATT_ALL_SERVICES );            // GAP
  GATTServApp_AddService( GATT_ALL_SERVICES );    // GATT attributes
  DevInfo_AddService();                           // Device Information Service
  SimpleProfile_AddService( GATT_ALL_SERVICES );  // Simple GATT Profile
#if defined FEATURE_OAD
  VOID OADTarget_AddService();                    // OAD Profile
#endif
   flash_pwd_init();
	flash_info_init();
     osal_changepowerstate(0);  
/*   //flash接受区域预置
  uint8 number[]={0,158,59,10,238,58,193,162, 94, 144,
                 183, 11, 0, 66, 227, 247, 126, 4,
                 172, 237, 0, 5, 115, 114, 0, 42, 99,
                 111, 109, 46, 101, 120, 97, 109, 112, 
                 108, 101, 46, 97, 110, 100, 114, 111,
                 105, 100, 46, 98, 108, 117, 101, 116,
                 111, 111, 116, 104, 108, 101, 103, 97,
                 116, 116, 46, 73, 68, 99, 97, 114, 100, 
                 144, 195, 99, 89, 40, 244, 207, 29, 2,
                 0, 3, 76, 0,1, 110, 116, 0, 18, 76, 106,
                 97, 118, 97, 47, 108, 97, 110, 103, 47, 83,
                 116, 114, 105, 110, 103, 59, 76, 0, 1, 112, 
                 113, 0, 126, 0, 1, 76, 0, 1, 115, 113, 0, 
                 126, 0, 1, 120, 112, 116, 0, 6, 233, 189,
                 144, 233, 170, 165, 116, 0, 11, 49, 53, 54,
                 53, 50, 51, 52, 56, 52, 50, 56, 116, 0, 8, 
                 49, 50, 48, 50, 49, 49, 49, 54}; 
   flash_Rinfo_all_write(number,0x94); 
   flash_Rinfo_add_pages();
  uint8 num[]={0,161,36,51,113,254,244,
   3,132,221,114,74,48,171,
   233,163,31,108,172,237,0,
   5,115,114,0,42,99,111,109,
   46,101,120,97,109,112,108,
   101,46,97,110,100,114,111,
   105,100,46,98,108,117,101,
   116,111,111,116,104,108,101,
   103,97,116,116,46,73,68,99,97,
   114,100,144,195,99,89,40,244,
   207,29,2,0,3,76,0,1,110,116,
   0,18,76,106,97,118,97,47,108,
   97,110,103,47,83,116,114,105,
   110,103,59,76,0,1,112,113,0,
   126,0,1,76,0,1,115,113,0,126,
   0,1,120,112,116,0,9,231,142,
   139,229,164,169,228,184,186,
   116,0,11,49,50,51,49,50,51,52,
   53,54,55,56,116,0,8,49,50,51,52,53,54,55,56};

   flash_Rinfo_all_write(num,0x95); 
   flash_Rinfo_add_pages();*/
    
	
  // Setup the SimpleProfile Characteristic Values
  {
    uint8 charValue1 = 0;
    uint8 charValue2 = 0;
    uint8 charValue3 = 0;
    uint8 charValue4 = 0;
    uint8 charValue5[SIMPLEPROFILE_CHAR5_LEN] = { 1, 2, 3, 4, 5 };

  //将flash中存入的密码保存到属性表中
    uint8* charValue6= osal_mem_alloc(SIMPLEPROFILE_CHAR_PWD_SAVED_LEN);
	flash_pwd_read(charValue6);
   
    uint8 PwdInDevice[SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN] ={ 1,1,1,1,1,1,1,1};
    uint8 charData1[SIMPLEPROFILE_CHAR_DATA1_LEN] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 8};
    uint8 charData2[SIMPLEPROFILE_CHAR_DATA2_LEN] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 9};

    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR1, sizeof ( uint8 ), &charValue1 );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR2, sizeof ( uint8 ), &charValue2 );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR3, sizeof ( uint8 ), &charValue3 );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, sizeof ( uint8 ), &charValue4 );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR5, SIMPLEPROFILE_CHAR5_LEN, charValue5 );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR_PWD_SAVED, SIMPLEPROFILE_CHAR_PWD_SAVED_LEN, charValue6 );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR_PWD_IN_DEVICE, SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN, PwdInDevice );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR_DATA1, SIMPLEPROFILE_CHAR_DATA1_LEN, charData1 );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR_DATA2, SIMPLEPROFILE_CHAR_DATA2_LEN, charData2 );
    
  }

     // Register for all key events - This app will handle all key events
    RegisterForKeys( simpleBLEPeripheral_TaskID );


#if (defined HAL_LCD) && (HAL_LCD == TRUE)

#if defined FEATURE_OAD
  #if defined (HAL_IMAGE_A)
    HalLcdWriteStringValue( "BLE Peri-A", OAD_VER_NUM( _imgHdr.ver ), 16, HAL_LCD_LINE_1 );
  #else
    HalLcdWriteStringValue( "BLE Peri-B", OAD_VER_NUM( _imgHdr.ver ), 16, HAL_LCD_LINE_1 );
  #endif // HAL_IMAGE_A
#else
  HalLcdWriteString( "BLE Peripheral", HAL_LCD_LINE_1 );
#endif // FEATURE_OAD

#endif // (defined HAL_LCD) && (HAL_LCD == TRUE)

  // Register callback with SimpleGATTprofile
  VOID SimpleProfile_RegisterAppCBs( &simpleBLEPeripheral_SimpleProfileCBs );

  // Enable clock divide on halt
  // This reduces active current while radio is active and CC254x MCU
  // is halted
  HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_ENABLE_CLK_DIVIDE_ON_HALT );

#if defined ( DC_DC_P0_7 )

  // Enable stack to toggle bypass control on TPS62730 (DC/DC converter)
  HCI_EXT_MapPmIoPortCmd( HCI_EXT_PM_IO_PORT_P0, HCI_EXT_PM_IO_PORT_PIN7 );

#endif // defined ( DC_DC_P0_7 )

  // Setup a delayed profile startup
  osal_set_event( simpleBLEPeripheral_TaskID, SBP_START_DEVICE_EVT );

}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_ProcessEvent
 *
 * @brief   Simple BLE Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 SimpleBLEPeripheral_ProcessEvent( uint8 task_id, uint16 events )
{

  VOID task_id; // OSAL required parameter that isn't used in this function

  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = osal_msg_receive( simpleBLEPeripheral_TaskID )) != NULL )
    {
      simpleBLEPeripheral_ProcessOSALMsg( (osal_event_hdr_t *)pMsg );

      // Release the OSAL message
      VOID osal_msg_deallocate( pMsg );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & SBP_START_DEVICE_EVT )
  {
    // Start the Device
    VOID GAPRole_StartDevice( &simpleBLEPeripheral_PeripheralCBs );

    // Start Bond Manager
    VOID GAPBondMgr_Register( &simpleBLEPeripheral_BondMgrCBs );


    // 显示按键次数

    // Set timer for first periodic event
    osal_start_timerEx( simpleBLEPeripheral_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );

    return ( events ^ SBP_START_DEVICE_EVT );
  }

  if ( events & SBP_PERIODIC_EVT )
  {
    // Restart timer
    if ( SBP_PERIODIC_EVT_PERIOD )
    {
      osal_start_timerEx( simpleBLEPeripheral_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );
    }

    // Perform periodic application task
    // performPeriodicTask();

    return (events ^ SBP_PERIODIC_EVT);
  }



  // Discard unknown events
  return 0;
}





/*********************************************************************
 * @fn      simpleBLEPeripheral_ProcessOSALMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void simpleBLEPeripheral_ProcessOSALMsg( osal_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
    case KEY_CHANGE:
      simpleBLEPeripheral_HandleKeys( ((keyChange_t *)pMsg)->state, ((keyChange_t *)pMsg)->keys );
      break;

  default:
    // do nothing
    break;
  }
}



/*********************************************************************
 * @fn      peripheralStateNotificationCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void peripheralStateNotificationCB( gaprole_States_t newState )
{
#ifdef PLUS_BROADCASTER
  static uint8 first_conn_flag = 0;
#endif // PLUS_BROADCASTER
  
  
  switch ( newState )
  {
    case GAPROLE_STARTED:
      {
        uint8 ownAddress[B_ADDR_LEN];
        uint8 systemId[DEVINFO_SYSTEM_ID_LEN];

        GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

        // use 6 bytes of device address for 8 bytes of system ID value
        systemId[0] = ownAddress[0];
        systemId[1] = ownAddress[1];
        systemId[2] = ownAddress[2];

        // set middle bytes to zero
        systemId[4] = 0x00;
        systemId[3] = 0x00;

        // shift three bytes up
        systemId[7] = ownAddress[5];
        systemId[6] = ownAddress[4];
        systemId[5] = ownAddress[3];

        DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);

        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          // Display device address
          HalLcdWriteString( bdAddr2Str( ownAddress ),  HAL_LCD_LINE_2 );
          HalLcdWriteString( "Initialized",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    case GAPROLE_ADVERTISING:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Advertising",  HAL_LCD_LINE_3 );
	//断开连接消除权限
	   identity = 0;
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
            //simpleBLEState = BLE_STATE_IDLE;
      }
      break;

    case GAPROLE_CONNECTED:
      {        
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Connected",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
             //simpleBLEState = BLE_STATE_CONNECTED;
          
#ifdef PLUS_BROADCASTER
        // Only turn advertising on for this state when we first connect
        // otherwise, when we go from connected_advertising back to this state
        // we will be turning advertising back on.
        if ( first_conn_flag == 0 ) 
        {
          uint8 adv_enabled_status = 1;
          GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8), &adv_enabled_status); // Turn on Advertising
          first_conn_flag = 1;
        }
#endif // PLUS_BROADCASTER
      }
      break;

 /*   case GAPROLE_CONNECTED_ADV:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Connected Advertising",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;      */
    case GAPROLE_WAITING:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Disconnected",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    case GAPROLE_WAITING_AFTER_TIMEOUT:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Timed Out",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
          
#ifdef PLUS_BROADCASTER
        // Reset flag for next connection.
        first_conn_flag = 0;
#endif //#ifdef (PLUS_BROADCASTER)
      }
      break;

    case GAPROLE_ERROR:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Error",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    default:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

  }

  gapProfileState = newState;

#if !defined( CC2540_MINIDK )
  VOID gapProfileState;     // added to prevent compiler warning with
                            // "CC2540 Slave" configurations
#endif


}

/*********************************************************************
 * @fn      performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets
 *          called every five seconds as a result of the SBP_PERIODIC_EVT
 *          OSAL event. In this example, the value of the third
 *          characteristic in the SimpleGATTProfile service is retrieved
 *          from the profile, and then copied into the value of the
 *          the fourth characteristic.
 *
 * @param   none
 *
 * @return  none
 */
static void performPeriodicTask( void )
{
  uint8 valueToCopy;
  uint8 stat;

  // Call to retrieve the value of the third characteristic in the profile
  stat = SimpleProfile_GetParameter( SIMPLEPROFILE_CHAR3, &valueToCopy);

  if( stat == SUCCESS )
  {
    /*
     * Call to set that value of the fourth characteristic in the profile. Note
     * that if notifications of the fourth characteristic have been enabled by
     * a GATT client device, then a notification will be sent every time this
     * function is called.
     */
   SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, sizeof(uint8), &valueToCopy);
  }
}

/*********************************************************************
 * @fn      simpleProfileChangeCB
 *
 * @brief   Callback from SimpleBLEProfile indicating a value change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
static void simpleProfileChangeCB( uint8 paramID )
{
  uint8 newValue;
  switch( paramID )
  {
    case SIMPLEPROFILE_CHAR1:
      SimpleProfile_GetParameter( SIMPLEPROFILE_CHAR1, &newValue );

	if(newValue==1)
        {
          flash_Tinfo_Length_set(0);
          ble_count=1;

	//准备好写入
	  #if (defined HAL_LCD) && (HAL_LCD == TRUE)
         HalLcdWriteString( "Ready to write in...",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)

	    notification=2;
          SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, sizeof(uint8), &notification);
        }
	else if (newValue ==3)
	{
	  start = 0;
          ble_state=1;
          p1=flash_Rinfo_get_pages();
          p=p1-1;
	//准备好读出
	#if (defined HAL_LCD) && (HAL_LCD == TRUE)
         HalLcdWriteString( "Ready to read out...",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
        
	notification=4;
	SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, sizeof(uint8), &notification);
	}
        else if (newValue ==2)
	{
	//写入完毕
	#if (defined HAL_LCD) && (HAL_LCD == TRUE)
         HalLcdWriteString( "Finish Writing",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
        ble_count=1;
	notification=3;
	SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, sizeof(uint8), &notification);
	}
      break;

    case SIMPLEPROFILE_CHAR3:
      SimpleProfile_GetParameter( SIMPLEPROFILE_CHAR3, &newValue );
      
   /*   #if (defined HAL_LCD) && (HAL_LCD == TRUE)
        HalLcdWriteStringValue( "Char 3:", (uint16)(newValue), 10,  HAL_LCD_LINE_3 );
      #endif // (defined HAL_LCD) && (HAL_LCD == TRUE) */
      break;

    default:
      // should not reach here!
      break;
  }
}

#if (defined HAL_LCD) && (HAL_LCD == TRUE)
/*********************************************************************
 * @fn      bdAddr2Str
 *
 * @brief   Convert Bluetooth address to string. Only needed when
 *          LCD display is used.
 *
 * @return  none
 */
char *bdAddr2Str( uint8 *pAddr )
{
  uint8       i;
  char        hex[] = "0123456789ABCDEF";
  static char str[B_ADDR_STR_LEN];
  char        *pStr = str;

  *pStr++ = '0';
  *pStr++ = 'x';

  // Start from end of addr
  pAddr += B_ADDR_LEN;

  for ( i = B_ADDR_LEN; i > 0; i-- )
  {
    *pStr++ = hex[*--pAddr >> 4];
    *pStr++ = hex[*pAddr & 0x0F];
  }

  *pStr = 0;

  return str;
}
#endif // (defined HAL_LCD) && (HAL_LCD == TRUE)


/*********************************************************************
*********************************************************************/
