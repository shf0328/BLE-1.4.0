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
#include "FlashOperate.h"


/*********************************************************************
* FUNCTIONS
*/


//序列号**********************************************************************
/**************************************
* uint8 flash_serialNumberInit(void);
* 序列号的初始化
**************************************/
uint8 flash_serialNumberInit()
{
	uint8 serial[SERIAL_LENGTH] = { 0 };
	//地址0x8E是序列号
	int8 ret8 = osal_snv_read(SERIAL_ADDRESS, SERIAL_LENGTH, serial);
	// 如果该段内存未曾写入过数据， 直接读，会返回 NV_OPER_FAILED 
	if (NV_OPER_FAILED == ret8)
	{
		// 把数据结构保存到flash
		flash_generateSerialNumber(serial);
		osal_snv_write(SERIAL_ADDRESS, SERIAL_LENGTH, serial);
		osal_snv_read(SERIAL_ADDRESS, SERIAL_LENGTH, serial);
	}
	return SUCCESS;
}

/**************************************
* uint8 flash_generateSerialNumber(void *pBuf);
* 产生一串3位序列号，数组地址开始位pBuf
**************************************/
uint8 flash_generateSerialNumber(void *pBuf)
{
	uint8 serial[SERIAL_LENGTH] = { 0 };
	uint16 fulserial[SERIAL_LENGTH] = { 0 };

	for (int i = 0; i<SERIAL_LENGTH; i++)
	{
		fulserial[i] = osal_rand();
		serial[i] = LO_UINT16(fulserial[i]);
	}
	for (int i = 0; i<SERIAL_LENGTH; i++)
	{
		((uint8 *)pBuf)[i] = serial[i];
	}
	return 0;
}

/**************************************
* uint8 flash_getSerialNumber(void *pBuf);
* 获取产生的3位序列号，赋值给数组地址pBuf
**************************************/
uint8 flash_getSerialNumber(void *pBuf)
{
	return osal_snv_read(SERIAL_ADDRESS, SERIAL_LENGTH, pBuf);
}
//结束序列号******************************************************************






//余额************************************************************************
/**************************************
* uint8 flash_cashInit(void);
* 余额的初始化
**************************************/
uint8 flash_cashInit()
{
	uint8 cash[CASH_LENGTH] = { 0 };
	int8 ret8 = osal_snv_read(CASH_ADDRESS, CASH_LENGTH, cash);
	// 如果该段内存未曾写入过数据， 直接读，会返回 NV_OPER_FAILED 
	if (NV_OPER_FAILED == ret8)
	{
		// 把数据结构保存到flash
		osal_snv_write(CASH_ADDRESS, CASH_LENGTH, cash);
		osal_snv_read(CASH_ADDRESS, CASH_LENGTH, cash);
	}
	return SUCCESS;
}

/**************************************
* uint8 flash_save_cash(void *pBuf);
* 存储余额
**************************************/
uint8 flash_save_cash(void *pBuf)
{
	return osal_snv_write(CASH_ADDRESS, CASH_LENGTH, pBuf);
}

/**************************************
* uint8 flash_get_cash(void *pBuf);
* 获取余额，放到一个pBuf的数组里面
**************************************/
uint8 flash_get_cash(void *pBuf)
{
	uint8 cash[CASH_LENGTH] = { 0 };
	osal_snv_read(CASH_ADDRESS, CASH_LENGTH, cash);
	for (int i = 0; i<CASH_LENGTH; i++)
	{
		((uint8 *)pBuf)[i] = cash[i];
	}
	return SUCCESS;
}
//结束余额********************************************************************




//密码************************************************************************
/*************************************
* uint8 flash_pwd_init( void )
* 在flash内部密码的初始化函数
* 即在对应位存入相应的初始密码，8个0
**************************************/
uint8 flash_pwd_init(void)
{
	uint8 pwd[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	int8 ret8 = osal_snv_read(PASSWORD_ADDRESS, 8, pwd);
	// 如果该段内存未曾写入过数据， 直接读，会返回 NV_OPER_FAILED ,
	// 我们利用这个特点作为第一次烧录后的运行， 从而设置参数的出厂设置
	if (NV_OPER_FAILED == ret8)
	{
		// 把数据结构保存到flash
		osal_memset(PASSWORD_ADDRESS, 0, 8);
		osal_snv_write(PASSWORD_ADDRESS, 8, pwd);
		osal_snv_read(PASSWORD_ADDRESS, 8, pwd);
	}
	return SUCCESS;
}

/*************************************
* uint8 flash_pwd_write(void *pBuf)
* 向flash内部写入密码
* 参数pBuf为一个8位uint8的数组的地址
**************************************/
uint8 flash_pwd_write(void *pBuf)
{
	return osal_snv_write(PASSWORD_ADDRESS, 8, pBuf);
}

/**************************************
* uint8 flash_pwd_read(void *pBuf)
* 读取flash内部存入的密码
* 参数pBuf为一个8位uint8数组的地址
**************************************/
uint8 flash_pwd_read(void *pBuf)
{
	return osal_snv_read(PASSWORD_ADDRESS, 8, pBuf);
}

/**************************************
* uint8 flash_pwd_delete(void *pBuf)
* 清空flash内部存入的密码
**************************************/
uint8 flash_pwd_delete(void)
{
	uint8 pwd[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	return osal_snv_write(PASSWORD_ADDRESS, 8, pwd);
}
//结束密码*******************************************************************



//TINFO************************************************************************
uint8 flash_Tinfo_init(void)
{
	uint16* T_info;
	T_info = (uint16 *)osal_mem_alloc(250);
	osal_memset(T_info, 0, 250);

	int8 ret8 = osal_snv_read(TINFO_ADDRESS, INFO_LENGTH, T_info);
	// 如果该段内存未曾写入过数据， 直接读，会返回 NV_OPER_FAILED 
	if (NV_OPER_FAILED == ret8)
	{
		// 把数据结构保存到flash
		osal_memset(T_info, 0, INFO_LENGTH);
		osal_snv_write(TINFO_ADDRESS, INFO_LENGTH, T_info);
		osal_snv_read(TINFO_ADDRESS, INFO_LENGTH, T_info);
	}
	//初始化发送信息的长度
	osal_mem_free(T_info);
}

/**************************************
* uint8 flash_Tinfo_all_read(void *pBuf)
* 在flash内部发送数据的全部读取
* 参数是一个长度INFO_LENGTH的数组地址
* 返回值是osal flash操作的值，具体参见API文档
* 一般使用成功是SUCCESS
**************************************/
uint8 flash_Tinfo_all_read(void *pBuf)
{
	return osal_snv_read(TINFO_ADDRESS, INFO_LENGTH, pBuf);
}
//结束TINFO*******************************************************************
