/*********************************************************************
* INCLUDES
*/
#include "hal_adc.h"
#include "hal_flash.h"
#include "hal_types.h"
#include "comdef.h"
#include "OSAL.h"
#include "osal_snv.h"
#include "hal_assert.h"
#include "saddr.h"
#include "bcomdef.h"
#include "OSAL_PwrMgr.h"

#include "OnBoard.h"
#include "hal_adc.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_lcd.h"
#include "flash_operate.h"

/*********************************************************************
* FUNCTIONS
*/

/**************************************
* uint8 led_start_social(void);
* 开始社交模式
**************************************/
uint8 led_start_social(void)
{
	HalLedBlink(HAL_LED_1, 15, 50, 2000);
        return SUCCESS;
}

/**************************************
* uint8 led_info_rec(void);
* 收到信息
**************************************/
uint8 led_info_rec(void)
{
	HalLedBlink(HAL_LED_2, 3, 50, 600);
        return SUCCESS;
}

/**************************************
* uint8 led_notif(uint8 rank);
* 收到信息
**************************************/
uint8 led_notif(uint8 rank)
{
        if((rank>=1)&&(rank<=4))
	HalLedBlink(HAL_LED_3, 20, 50, 300+rank*100);
        return SUCCESS;
}