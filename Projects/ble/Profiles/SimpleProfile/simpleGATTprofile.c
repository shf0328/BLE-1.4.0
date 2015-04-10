/**************************************************************************************************
  Filename:       simpleGATTprofile.c

  Description:    This file contains the Simple GATT profile sample GATT service 
                  profile for use with the BLE sample application.


**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"
#include "hal_lcd.h"
#include "flash_operate.h"
#include "simpleGATTprofile.h"
//#include "FlashOperate.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED        36

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
 extern uint8 notification;
// Simple GATT Profile Service UUID: 0xFFF0
CONST uint8 simpleProfileServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_SERV_UUID), HI_UINT16(SIMPLEPROFILE_SERV_UUID)
};

// Characteristic 1 UUID: 0xFFF1
CONST uint8 simpleProfilechar1UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR1_UUID), HI_UINT16(SIMPLEPROFILE_CHAR1_UUID)
};

// Characteristic 2 UUID: 0xFFF2
CONST uint8 simpleProfilechar2UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR2_UUID), HI_UINT16(SIMPLEPROFILE_CHAR2_UUID)
};

// Characteristic 3 UUID: 0xFFF3
CONST uint8 simpleProfilechar3UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR3_UUID), HI_UINT16(SIMPLEPROFILE_CHAR3_UUID)
};

// Characteristic 4 UUID: 0xFFF4
CONST uint8 simpleProfilechar4UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR4_UUID), HI_UINT16(SIMPLEPROFILE_CHAR4_UUID)
};

// Characteristic 5 UUID: 0xFFF5
CONST uint8 simpleProfilechar5UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR5_UUID), HI_UINT16(SIMPLEPROFILE_CHAR5_UUID)
};

// Characteristic 6 UUID: 0xFFF6
CONST uint8 simpleProfilePwdSavedUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR_PWD_SAVED_UUID), HI_UINT16(SIMPLEPROFILE_CHAR_PWD_SAVED_UUID)
};


// Characteristic 7 UUID: 0xFFF7
CONST uint8 simpleProfilePwdInDeviceUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_UUID), HI_UINT16(SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_UUID)
};


// Characteristic 8 UUID: 0xFFF8
CONST uint8 simpleProfileData1UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR_DATA1_UUID), HI_UINT16(SIMPLEPROFILE_CHAR_DATA1_UUID)
};


// Characteristic 9 UUID: 0xFFF9
CONST uint8 simpleProfileData2UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR_DATA2_UUID), HI_UINT16(SIMPLEPROFILE_CHAR_DATA2_UUID)
};

// Characteristic 10 UUID: 0xFFFA
CONST uint8 simpleProfileIDCardUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR_IDCARD_UUID), HI_UINT16(SIMPLEPROFILE_CHAR_IDCARD_UUID)
};

// Characteristic 11 UUID: 0xFFFB
CONST uint8 simpleProfilechar11UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR11_UUID), HI_UINT16(SIMPLEPROFILE_CHAR11_UUID)
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern uint8 identity;
extern uint8 p1;
/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 start = 0;
uint8 ble_count=1;
uint8 ble_state=1;
uint8 app_notif=0;
uint8 p=0;  //当前页码
uint8 leng=0;  //当前读的页的数据长度      
uint8 j=0;    //当前读的页的数据包数
uint8 re=0;
static simpleProfileCBs_t *simpleProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Simple Profile Service attribute
static CONST gattAttrType_t simpleProfileService = { ATT_BT_UUID_SIZE, simpleProfileServUUID };


// Simple Profile Characteristic 1 Properties
static uint8 simpleProfileChar1Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 1 Value
static uint8 simpleProfileChar1 = 0;

// Simple Profile Characteristic 1 User Description
static uint8 simpleProfileChar1UserDesp[17] = "Characteristic 1\0";


// Simple Profile Characteristic 2 Properties
static uint8 simpleProfileChar2Props = GATT_PROP_READ;

// Characteristic 2 Value
static uint8 simpleProfileChar2 = 0;

// Simple Profile Characteristic 2 User Description
static uint8 simpleProfileChar2UserDesp[17] = "Characteristic 2\0";


// Simple Profile Characteristic 3 Properties
static uint8 simpleProfileChar3Props = GATT_PROP_WRITE;

// Characteristic 3 Value
static uint8 simpleProfileChar3 = 0;

// Simple Profile Characteristic 3 User Description
static uint8 simpleProfileChar3UserDesp[17] = "Characteristic 3\0";


// Simple Profile Characteristic 4 Properties
static uint8 simpleProfileChar4Props = GATT_PROP_NOTIFY;

// Characteristic 4 Value
static uint8 simpleProfileChar4 = 0;

// Simple Profile Characteristic 4 Configuration Each client has its own
// instantiation of the Client Characteristic Configuration. Reads of the
// Client Characteristic Configuration only shows the configuration for
// that client and writes only affect the configuration of that client.
static gattCharCfg_t simpleProfileChar4Config[GATT_MAX_NUM_CONN];
                                        
// Simple Profile Characteristic 4 User Description
static uint8 simpleProfileChar4UserDesp[17] = "Characteristic 4\0";


// Simple Profile Characteristic 5 Properties
static uint8 simpleProfileChar5Props = GATT_PROP_READ;

// Characteristic 5 Value
static uint8 simpleProfileChar5[SIMPLEPROFILE_CHAR5_LEN] = { 0, 0, 0, 0, 0 };

// Simple Profile Characteristic 5 User Description
static uint8 simpleProfileChar5UserDesp[17] = "Characteristic 5\0";



// Simple Profile Characteristic 6 Properties
static uint8 simpleProfilePwdSavedProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 6 Value
static uint8 simpleProfilePwdSaved[SIMPLEPROFILE_CHAR_PWD_SAVED_LEN] = { 0, 0, 0, 0, 0, 0, 0,0 };

// Simple Profile Characteristic 6 User Description
static uint8 simpleProfilePwdSavedDesp[17] = "Characteristic 6\0";


// Simple Profile Characteristic 7 PWD-IN-DEVICE Properties
static uint8 simpleProfilePwdInDeviceProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 7 Value
static uint8 simpleProfilePwdInDevice[SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// Simple Profile Characteristic 7 User Description
static uint8 simpleProfilePwdInDeviceUserDesp[17] = "Characteristic 7\0";

// Simple Profile Characteristic 8 PWD-IN-DEVICE Properties
static uint8 simpleProfileData1Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 8 Value
static uint8 simpleProfileData1[SIMPLEPROFILE_CHAR_DATA1_LEN] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Simple Profile Characteristic 8 User Description
static uint8 simpleProfileData1UserDesp[17] = "Characteristic 8\0";

// Simple Profile Characteristic 9 PWD-IN-DEVICE Properties
static uint8 simpleProfileData2Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 9 Value
static uint8 simpleProfileData2[SIMPLEPROFILE_CHAR_DATA2_LEN] = { 0, 0, 0, 0};

// Simple Profile Characteristic 9 User Description
static uint8 simpleProfileData2UserDesp[17] = "Characteristic 9\0";

// Simple Profile Characteristic 10 IDCARD Properties
static uint8 simpleProfileIDCardProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 10 Value
static uint8 simpleProfileIDCard[SIMPLEPROFILE_CHAR_IDCARD_LEN] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Simple Profile Characteristic 10 User Description
static uint8 simpleProfileIDCardUserDesp[18] = "Characteristic 10\0";

// Simple Profile Characteristic 11 Properties
static uint8 simpleProfileChar11Props = GATT_PROP_NOTIFY;

// Characteristic 11 Value
static uint8 simpleProfileChar11[SIMPLEPROFILE_CHAR11_LEN] = {0,0,0,0};

// Simple Profile Characteristic 11 Configuration Each client has its own
// instantiation of the Client Characteristic Configuration. Reads of the
// Client Characteristic Configuration only shows the configuration for
// that client and writes only affect the configuration of that client.
static gattCharCfg_t simpleProfileChar11Config[GATT_MAX_NUM_CONN];
                                        
// Simple Profile Characteristic 11 User Description
static uint8 simpleProfileChar11UserDesp[18] = "Characteristic 11\0";
/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t simpleProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = 
{
  // Simple Profile Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&simpleProfileService            /* pValue */
  },

    // Characteristic 1 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar1Props 
    },

      // Characteristic Value 1
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar1UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        &simpleProfileChar1 
      },

      // Characteristic 1 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar1UserDesp 
      },      

    // Characteristic 2 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar2Props 
    },

      // Characteristic Value 2
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar2UUID },
        GATT_PERMIT_READ, 
        0, 
        &simpleProfileChar2 
      },

      // Characteristic 2 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar2UserDesp 
      },           
      
    // Characteristic 3 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar3Props 
    },

      // Characteristic Value 3
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar3UUID },
        GATT_PERMIT_WRITE, 
        0, 
        &simpleProfileChar3 
      },

      // Characteristic 3 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar3UserDesp 
      },

    // Characteristic 4 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar4Props 
    },

      // Characteristic Value 4
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar4UUID },
        0, 
        0, 
        &simpleProfileChar4 
      },

      // Characteristic 4 configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)simpleProfileChar4Config 
      },
      
      // Characteristic 4 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar4UserDesp 
      },
      
    // Characteristic 5 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar5Props 
    },

      // Characteristic Value 5
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar5UUID },
        GATT_PERMIT_AUTHEN_READ, 
        0, 
        simpleProfileChar5 
      },

      // Characteristic 5 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar5UserDesp 
      },


    // Characteristic 6 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfilePwdSavedProps 
    },

      // Characteristic Value 6
      { 
        { ATT_BT_UUID_SIZE, simpleProfilePwdSavedUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0, 
        simpleProfilePwdSaved 
      },

      // Characteristic 6 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfilePwdSavedDesp 
      },



	// Characteristic 7 Declaration
	{ 
	 { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ, 
       0,
        &simpleProfilePwdInDeviceProps 
       },
 
      // Characteristic Value 7
      { 
        { ATT_BT_UUID_SIZE, simpleProfilePwdInDeviceUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0, 
        simpleProfilePwdInDevice 
      },

      // Characteristic 7 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfilePwdInDeviceUserDesp 
      },


	// Characteristic 8 Declaration
	{ 
	 { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ, 
       0,
        &simpleProfileData1Props 
       },
 
      // Characteristic Value 8
      { 
        { ATT_BT_UUID_SIZE, simpleProfileData1UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0, 
        simpleProfileData1 
      },

      // Characteristic 8 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileData1UserDesp 
      },

	// Characteristic 9 Declaration
	{ 
	 { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ, 
       0,
        &simpleProfileData2Props 
       },
 
      // Characteristic Value 9
      { 
        { ATT_BT_UUID_SIZE, simpleProfileData2UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0, 
        simpleProfileData2 
      },

      // Characteristic 9 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileData2UserDesp 
      },
	// Characteristic 10 Declaration
	{ 
	 { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ, 
       0,
        &simpleProfileIDCardProps 
       },
 
      // Characteristic Value 10
      { 
        { ATT_BT_UUID_SIZE, simpleProfileIDCardUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0, 
        simpleProfileIDCard 
      },

      // Characteristic 10 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileIDCardUserDesp 
      },

    // Characteristic 11 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar11Props 
    },

      // Characteristic Value 11
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar11UUID },
        0, 
        0, 
        simpleProfileChar11 
      },

      // Characteristic 11 configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)simpleProfileChar11Config 
      },
      
      // Characteristic 11 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar11UserDesp 
      },
     


};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8 simpleProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen );
static bStatus_t simpleProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset );

static void simpleProfile_HandleConnStatusCB( uint16 connHandle, uint8 changeType );

static void confirmIdentity(void);
/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t simpleProfileCBs =
{
  simpleProfile_ReadAttrCB,  // Read callback function pointer
  simpleProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleProfile_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t SimpleProfile_AddService( uint32 services )
{
  uint8 status = SUCCESS;

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, simpleProfileChar4Config );
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, simpleProfileChar11Config );

  // Register with Link DB to receive link status change callback
  VOID linkDB_Register( simpleProfile_HandleConnStatusCB );  
  
  if ( services & SIMPLEPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( simpleProfileAttrTbl, 
                                          GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                          &simpleProfileCBs );
  }

  return ( status );
}


/*********************************************************************
 * @fn      SimpleProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t SimpleProfile_RegisterAppCBs( simpleProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    simpleProfile_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}
  

/*********************************************************************
 * @fn      SimpleProfile_SetParameter
 *
 * @brief   Set a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t SimpleProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case SIMPLEPROFILE_CHAR1:
      if ( len == sizeof ( uint8 ) ) 
      {
        simpleProfileChar1 = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SIMPLEPROFILE_CHAR2:
      if ( len == sizeof ( uint8 ) ) 
      {
        simpleProfileChar2 = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SIMPLEPROFILE_CHAR3:
      if ( len == sizeof ( uint8 ) ) 
      {
        simpleProfileChar3 = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SIMPLEPROFILE_CHAR4:
      if ( len == sizeof ( uint8 ) ) 
      {
        simpleProfileChar4 = *((uint8*)value);
        
        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg( simpleProfileChar4Config, &simpleProfileChar4, FALSE,
                                    simpleProfileAttrTbl, GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                    INVALID_TASK_ID );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SIMPLEPROFILE_CHAR5:
      if ( len == SIMPLEPROFILE_CHAR5_LEN ) 
      {
        VOID osal_memcpy( simpleProfileChar5, value, SIMPLEPROFILE_CHAR5_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    case SIMPLEPROFILE_CHAR_PWD_SAVED:
      if ( len == SIMPLEPROFILE_CHAR_PWD_SAVED_LEN ) 
      {
        VOID osal_memcpy( simpleProfilePwdSaved, value, SIMPLEPROFILE_CHAR_PWD_SAVED_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

     case SIMPLEPROFILE_CHAR_PWD_IN_DEVICE:
      if ( len == SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN ) 
      {
        VOID osal_memcpy( simpleProfilePwdInDevice, value, SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

	case SIMPLEPROFILE_CHAR_DATA1:
      if ( len == SIMPLEPROFILE_CHAR_DATA1_LEN ) 
      {
        VOID osal_memcpy( simpleProfileData1, value, SIMPLEPROFILE_CHAR_DATA1_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;


	case SIMPLEPROFILE_CHAR_DATA2:
      if ( len == SIMPLEPROFILE_CHAR_DATA2_LEN ) 
      {
        VOID osal_memcpy( simpleProfileData2, value, SIMPLEPROFILE_CHAR_DATA2_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
        case SIMPLEPROFILE_CHAR_IDCARD:
      if ( len == SIMPLEPROFILE_CHAR_IDCARD_LEN ) 
      {
        VOID osal_memcpy( simpleProfileIDCard, value, SIMPLEPROFILE_CHAR_IDCARD_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;	
      
    default:
      ret = INVALIDPARAMETER;
      break;
      
    case SIMPLEPROFILE_CHAR11:
       if ( len == SIMPLEPROFILE_CHAR11_LEN ) 
      {
        VOID osal_memcpy( simpleProfileChar11, value, SIMPLEPROFILE_CHAR11_LEN );        
        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg( simpleProfileChar11Config, simpleProfileChar11, FALSE,
                                    simpleProfileAttrTbl, GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                    INVALID_TASK_ID );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

  }
  
  return ( ret );
}

/*********************************************************************
 * @fn      SimpleProfile_GetParameter
 *
 * @brief   Get a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t SimpleProfile_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case SIMPLEPROFILE_CHAR1:
      *((uint8*)value) = simpleProfileChar1;
      break;

    case SIMPLEPROFILE_CHAR2:
      *((uint8*)value) = simpleProfileChar2;
      break;      

    case SIMPLEPROFILE_CHAR3:
      *((uint8*)value) = simpleProfileChar3;
      break;  

    case SIMPLEPROFILE_CHAR4:
      *((uint8*)value) = simpleProfileChar4;
      break;

    case SIMPLEPROFILE_CHAR5:
      VOID osal_memcpy( value, simpleProfileChar5, SIMPLEPROFILE_CHAR5_LEN );
      break;      
      
    case SIMPLEPROFILE_CHAR_PWD_SAVED:
      VOID osal_memcpy( value, simpleProfilePwdSaved, SIMPLEPROFILE_CHAR_PWD_SAVED_LEN);
      break;  
      
    case SIMPLEPROFILE_CHAR_PWD_IN_DEVICE:
      VOID osal_memcpy( value, simpleProfilePwdInDevice, SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN );
      break;


    case SIMPLEPROFILE_CHAR_DATA1:
      VOID osal_memcpy( value, simpleProfileData1, SIMPLEPROFILE_CHAR_DATA1_LEN );
      break;

    case SIMPLEPROFILE_CHAR_DATA2:
      VOID osal_memcpy( value, simpleProfileData2, SIMPLEPROFILE_CHAR_DATA2_LEN );
      break;

    case SIMPLEPROFILE_CHAR_IDCARD:
      VOID osal_memcpy( value, simpleProfileIDCard, SIMPLEPROFILE_CHAR_IDCARD_LEN );
      break;      
    
    case SIMPLEPROFILE_CHAR11:
      VOID osal_memcpy( value, simpleProfileChar11, SIMPLEPROFILE_CHAR11_LEN );
      break; 
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          simpleProfile_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 *
 * @return      Success or Failure
 */
static uint8 simpleProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen )
{
  bStatus_t status = SUCCESS;

  // If attribute permissions require authorization to read, return error
  if ( gattPermitAuthorRead( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
  
  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }
 
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    uint8 leng=0;  //当前读的页的数据长度      
    uint8 j=0;    //当前读的页的数据包数
    uint8 re=0;
    if(p>=0) //p为当前页
    {
    re=flash_Rinfo_single_read(0x94+p,1);
    if(re!=0)
    {
      leng=re;
      HalLcdWriteStringValue( "3 VALUE = ", leng, 10, HAL_LCD_LINE_8 );
    }
    else {leng=0;}
    if(leng%9!=0)
    {j=(leng/9)+1;}
    else {j=leng/9;}
    }
    
     if(identity == 1)
    {
    	switch ( uuid )
    	{
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those reads
      // characteristics 1 and 2 have read permissions
      // characteritisc 3 does not have read permissions; therefore it is not
      //   included here
      // characteristic 4 does not have read permissions, but because it
      //   can be sent as a notification, it is included here
      
      	case SIMPLEPROFILE_CHAR1_UUID:
      	case SIMPLEPROFILE_CHAR4_UUID:
        *pLen = 1;
        pValue[0] = *pAttr->pValue;
        break;
   
        case SIMPLEPROFILE_CHAR2_UUID: 
        *pLen = 1;
         pValue[0] = leng;

        break;
          
      	case SIMPLEPROFILE_CHAR5_UUID:
        *pLen = SIMPLEPROFILE_CHAR5_LEN;
        VOID osal_memcpy( pValue, pAttr->pValue, SIMPLEPROFILE_CHAR5_LEN );
        break;
        
      	case SIMPLEPROFILE_CHAR_PWD_SAVED_UUID:
        *pLen = SIMPLEPROFILE_CHAR_PWD_SAVED_LEN;
        VOID osal_memcpy( pValue, pAttr->pValue, SIMPLEPROFILE_CHAR_PWD_SAVED_LEN );
        break;  


	case SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_UUID:
        *pLen = SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN;
        VOID osal_memcpy( pValue, pAttr->pValue, SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN );
        break;  


	case SIMPLEPROFILE_CHAR_DATA1_UUID:
        *pLen = SIMPLEPROFILE_CHAR_DATA1_LEN;
         SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR3,&app_notif);  
         //如果不是最后一页
         if(p>0)
         {
         //若APP通知位与包头一样
         if(app_notif==ble_state)
         {
		flash_Rinfo_short_read(pAttr->pValue, start, p );
                //判断是否最后一个包
                //如果不是最后一个包
                if(ble_state<j)
                {
                uint8 i;
                for(i=10;i>0;i--)
                {
                  pAttr->pValue[i]=pAttr->pValue[i-1];
                }
                pAttr->pValue[0]=ble_state;       
                VOID osal_memcpy( pValue,pAttr->pValue, SIMPLEPROFILE_CHAR_DATA1_LEN );
                    
                //读出数据包成功
                ble_state++;
                start+=9;
                }
                
                //如果是最后一个包
                else if(ble_state==j)
                {                      
                 uint8 i;       
                  for(i=10;i>0;i--)
                {
                  pAttr->pValue[i]=pAttr->pValue[i-1];
                }
                pAttr->pValue[0]=ble_state;             
                VOID osal_memcpy( pValue,pAttr->pValue, SIMPLEPROFILE_CHAR_DATA1_LEN );

                //该页读完
		notification = 5;
		SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, sizeof(uint8), &notification);
                }
         }
         //若APP通知位与包头不一样，则重发数据包
         else if(app_notif==ble_state-1)
         {
           flash_Rinfo_short_read(pAttr->pValue, start-9, p ); 
           uint8 i;
           for(i=10;i>0;i--)
                {
                  pAttr->pValue[i]=pAttr->pValue[i-1];
                }
           pAttr->pValue[0]=ble_state-1;      
           VOID osal_memcpy( pValue,pAttr->pValue, SIMPLEPROFILE_CHAR_DATA1_LEN );
         }
         }
         //如果是最后一页
         else if(p==0)
         {
          //若APP通知位与包头一样
         if(app_notif==ble_state)
         {           
		flash_Rinfo_short_read(pAttr->pValue, start, p );
                //判断是否最后一个包
                //如果不是最后一个包
                if(ble_state<j)
                {
                uint8 i;
                 for(i=10;i>0;i--)
                {
                  pAttr->pValue[i]=pAttr->pValue[i-1];
                }
                pAttr->pValue[0]=ble_state;

                 VOID osal_memcpy( pValue,pAttr->pValue, SIMPLEPROFILE_CHAR_DATA1_LEN );

       /* #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "CCCCCCC",  HAL_LCD_LINE_4 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE) */
                //读出数据包成功
                ble_state++;
                start+=9;              
                }
                //如果是最后一个包
                else if(ble_state==j)
                {              
                 uint8 i;
                  for(i=10;i>0;i--)
                {
                  pAttr->pValue[i]=pAttr->pValue[i-1];
                }
                pAttr->pValue[0]=ble_state;
                 VOID osal_memcpy( pValue,pAttr->pValue, SIMPLEPROFILE_CHAR_DATA1_LEN );               
                 //最后一页读成功
		notification = 6;
		SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, sizeof(uint8), &notification);
     /*            #if (defined HAL_LCD) && (HAL_LCD == TRUE)
                 HalLcdWriteString( "TEST_TEST",  HAL_LCD_LINE_6 );
                 #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)*/
                }
         }
         //若APP通知位与包头不一样，则重发数据包
         else if(app_notif==ble_state-1)
         {
           flash_Rinfo_short_read(pAttr->pValue, start-9, p ); 
           uint8 i;
            for(i=10;i>0;i--)
                {
                  pAttr->pValue[i]=pAttr->pValue[i-1];
                }
                pAttr->pValue[0]=ble_state-1;
          VOID osal_memcpy( pValue,pAttr->pValue, SIMPLEPROFILE_CHAR_DATA1_LEN );
         }
         }
         
        break; 

        case SIMPLEPROFILE_CHAR_DATA2_UUID:
        *pLen = SIMPLEPROFILE_CHAR_DATA2_LEN;
		 flash_get_cash(pAttr->pValue);
        VOID osal_memcpy( pValue, pAttr->pValue, SIMPLEPROFILE_CHAR_DATA2_LEN );
        break; 

/*        case SIMPLEPROFILE_CHAR_IDCARD:
        *pLen = SIMPLEPROFILE_CHAR_IDCARD;
		 flash_get_cash(pAttr->pValue);
        VOID osal_memcpy( pValue, pAttr->pValue, SIMPLEPROFILE_CHAR_IDCARD_LEN );
        break; */

        case SIMPLEPROFILE_CHAR11_UUID:
        *pLen = SIMPLEPROFILE_CHAR11_LEN;
        VOID osal_memcpy( pValue, pAttr->pValue, SIMPLEPROFILE_CHAR11_LEN );  
      default:
        // Should never get here! (characteristics 3 and 4 do not have read permissions)
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    	}}
	else if (identity==0)
	{
		switch ( uuid )
    		{
    		case SIMPLEPROFILE_CHAR4_UUID:
        	*pLen = 1;
        	pValue[0] = *pAttr->pValue;
        	break;

		default:
             // Should never get here! (characteristics 3 and 4 do not have read permissions)
             *pLen = 0;
               status =ATT_ERR_READ_NOT_PERMITTED;
              break;
    		}
	}
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }
 /*   #if (defined HAL_LCD) && (HAL_LCD == TRUE)
         HalLcdWriteString( "TEST_TEST",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)*/
  
  return ( status );
}

/*********************************************************************
 * @fn      simpleProfile_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 *
 * @return  Success or Failure
 */
static bStatus_t simpleProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset )
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
  
  // If attribute permissions require authorization to write, return error
  if ( gattPermitAuthorWrite( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
  
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    if(identity==1)
    {
    switch ( uuid )
    {
      case SIMPLEPROFILE_CHAR1_UUID:
      case SIMPLEPROFILE_CHAR3_UUID:
        // Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != 1 )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;        
          *pCurValue = pValue[0];

          if( pAttr->pValue == &simpleProfileChar1 )
          {
            notifyApp = SIMPLEPROFILE_CHAR1;        
          }
          else
          {
            notifyApp = SIMPLEPROFILE_CHAR3;       
            if(pAttr->pValue[0]==50)
            {
              if(p>0)
              {   
              p--;
              ble_state=1;
              start=0;
              }
              else if(p==0)
              {
               flash_Rinfo_all_clear();
                for(int i=0;i<p1;i++)
                {
                  flash_Rinfo_minus_pages();
                }
                leng=0;
              }             
            }    
          }
        }
             
        break;
        
        
      case SIMPLEPROFILE_CHAR_PWD_SAVED_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != SIMPLEPROFILE_CHAR_PWD_SAVED_LEN )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          VOID osal_memcpy( pAttr->pValue, pValue, SIMPLEPROFILE_CHAR_PWD_SAVED_LEN );
	   flash_pwd_write(pAttr->pValue); 

	  #if (defined HAL_LCD) && (HAL_LCD == TRUE)
      		HalLcdWriteString( "ChangePwdSuccessful",HAL_LCD_LINE_3);
         #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)

          notifyApp = SIMPLEPROFILE_CHAR_PWD_SAVED_LEN;
        }
             
        break;


	case SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          VOID osal_memcpy( pAttr->pValue, pValue, SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN );
          notifyApp = SIMPLEPROFILE_CHAR_PWD_IN_DEVICE;
           confirmIdentity();	
		#if (defined HAL_LCD) && (HAL_LCD == TRUE)
        	HalLcdWriteStringValue( "Idendity:", (uint16)(identity), 10,  HAL_LCD_LINE_3 );
      		#endif // (defined HAL_LCD) && (HAL_LCD == TRUE)  
        }
             
        break;


	case SIMPLEPROFILE_CHAR_DATA1_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != SIMPLEPROFILE_CHAR_DATA1_LEN )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          VOID osal_memcpy( pAttr->pValue, pValue, SIMPLEPROFILE_CHAR_DATA1_LEN );
          notifyApp = SIMPLEPROFILE_CHAR_DATA1;
          if(pValue[0]==ble_count)
          {
            uint8 i;
            uint8* temp;
	    temp= (uint8 *)osal_mem_alloc(9);
	    osal_memset(temp,0,9);
            for(i=0;i<9;i++)
            {
              temp[i]=pValue[i+1];
            } 
            flash_Tinfo_short_write(temp,9);
            osal_mem_free(temp);
	   //写入数据成功
	/*   #if (defined HAL_LCD) && (HAL_LCD == TRUE)
      		HalLcdWriteString( "WriteInSuccessful",HAL_LCD_LINE_3);
         #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)*/
              ble_count++;
             notification=ble_count+10;
             SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, sizeof(uint8), &notification);   
          }
          else
          {
             notification=ble_count+10;
             SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, sizeof(uint8), &notification); 
          }
         
        }
             
        break;

/*	case SIMPLEPROFILE_CHAR_DATA2_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != SIMPLEPROFILE_CHAR_DATA2_LEN )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          VOID osal_memcpy( pAttr->pValue, pValue, SIMPLEPROFILE_CHAR_DATA2_LEN );
          notifyApp = SIMPLEPROFILE_CHAR_DATA2;
        }
             
        break;
	*/ 
        
/*	case SIMPLEPROFILE_CHAR_IDCARD_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != SIMPLEPROFILE_CHAR_IDCARD_LEN )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          VOID osal_memcpy( pAttr->pValue, pValue, SIMPLEPROFILE_CHAR_IDCARD_LEN );
          notifyApp = SIMPLEPROFILE_CHAR_IDCARD;
        }
             
        break;
	*/ 
      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
		#if (defined HAL_LCD) && (HAL_LCD == TRUE)
          		HalLcdWriteString( "Notifaction ON", HAL_LCD_LINE_3);
        	#endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
		
        break;
       
      default:
        // Should never get here! (characteristics 2 and 4 do not have write permissions)
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
      }
      }
    else if (identity==0){
      switch(uuid)
      	{
	case SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          VOID osal_memcpy( pAttr->pValue, pValue, SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN );
          notifyApp = SIMPLEPROFILE_CHAR_PWD_IN_DEVICE;
		   confirmIdentity();	
		#if (defined HAL_LCD) && (HAL_LCD == TRUE)
        	HalLcdWriteStringValue( "Idendity:", (uint16)(identity), 10,  HAL_LCD_LINE_3 );
      		#endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
	  
        }
             
        break;
      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
		#if (defined HAL_LCD) && (HAL_LCD == TRUE)
          		HalLcdWriteString( "Notifaction ON", HAL_LCD_LINE_3);
        	#endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
		
        break;        

	 default:
        // Should never get here! (characteristics 2 and 4 do not have write permissions)
        status = ATT_ERR_WRITE_NOT_PERMITTED;
        break;
      	}
  }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }

  // If a charactersitic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && simpleProfile_AppCBs && simpleProfile_AppCBs->pfnSimpleProfileChange )
  {
    simpleProfile_AppCBs->pfnSimpleProfileChange( notifyApp );  
  }
  
  return ( status );
}

/*********************************************************************
 * @fn          simpleProfile_HandleConnStatusCB
 *
 * @brief       Simple Profile link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
static void simpleProfile_HandleConnStatusCB( uint16 connHandle, uint8 changeType )
{ 
  // Make sure this is not loopback connection
  if ( connHandle != LOOPBACK_CONNHANDLE )
  {
    // Reset Client Char Config if connection has dropped
    if ( ( changeType == LINKDB_STATUS_UPDATE_REMOVED )      ||
         ( ( changeType == LINKDB_STATUS_UPDATE_STATEFLAGS ) && 
           ( !linkDB_Up( connHandle ) ) ) )
    { 
      GATTServApp_InitCharCfg( connHandle, simpleProfileChar4Config );
      GATTServApp_InitCharCfg( connHandle, simpleProfileChar11Config );
    }
  }
}

static void confirmIdentity(void)
{
	uint8* InputPWD;
	InputPWD = osal_mem_alloc(SIMPLEPROFILE_CHAR_PWD_IN_DEVICE_LEN);
	SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR_PWD_IN_DEVICE, InputPWD);

	uint8* SavedPWD;
	SavedPWD = osal_mem_alloc(SIMPLEPROFILE_CHAR_PWD_SAVED_LEN);
	SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR_PWD_SAVED, SavedPWD);

	uint8 status;
	status = osal_memcmp(InputPWD, SavedPWD, SIMPLEPROFILE_CHAR_PWD_SAVED_LEN);

// 修改权限
	if(status==TRUE)
       	  identity=1;
	else if(status==FALSE)
	  	 identity=0;
//通知主机	
	notification = identity;
	SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, sizeof(uint8), &notification);
}
/*********************************************************************
*********************************************************************/
