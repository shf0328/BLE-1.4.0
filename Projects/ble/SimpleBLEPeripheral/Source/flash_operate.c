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


//序列号**********************************************************************
/**************************************
* uint8 flash_serialNumberInit(void);
* 序列号的初始化
**************************************/
uint8 flash_serialNumberInit()
{
	uint8 serial[SERIAL_LENGTH] = { 0 };
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
		osal_memset(pwd, 0, 8);
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
/**************************************
* uint8 flash_Tinfo_init( void )
* 在flash内部初始化发送数据的存储
**************************************/
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
        return SUCCESS;
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

/**************************************
* uint8 flash_Tinfo_short_write(void *pBuf, uint8 len)
* 在flash内部发送数据区域的接收数据的长度为s
* 则向s后面写入长度为len的数组，地址是pBuf
* 若超过存储长度的数据不写
**************************************/
uint8 flash_Tinfo_short_write(void *pBuf, uint8 len)
{
	uint8 length = 0;
	length = flash_Tinfo_Length_get();

	//分配一个长度为INFO_LENGTH的数组
	//uint16* inMem;
	//inMem= (uint16 *)osal_mem_alloc(INFO_LENGTH);
	//osal_memset(inMem,0,INFO_LENGTH);
	uint8 inMem[INFO_LENGTH] = { 0 };

	osal_snv_read(TINFO_ADDRESS, INFO_LENGTH, inMem);
	uint8 i = 0;
	for (i = 0; i<len; i++)
	{
		if ((length + i)<INFO_LENGTH)
		{
			inMem[length + i] = ((uint8 *)pBuf)[i];
		}
		else
		{
			break;
		}
	}
	length = length + len;
	if (length>INFO_LENGTH)
	{
		length = INFO_LENGTH;
	}
#if (defined HAL_LCD) && (HAL_LCD == TRUE)
	HalLcdWriteStringValue("LVALUE = ", length, 10, HAL_LCD_LINE_6);
#endif
	flash_Tinfo_Length_set(length);

	//释放内存
	osal_snv_write(TINFO_ADDRESS, INFO_LENGTH, inMem);
	//osal_mem_free(inMem);
	return 0;
}

/**************************************
* uint8 flash_Tinfo_single_read( uint8 seq )
* 读取flash内部发送数据的存储区域的第seq个位存入的数据
* 若seq未超过存储区域的长度，返回对应值
* 若seq超过存储区域的长度，返回0xFF
**************************************/
uint8 flash_Tinfo_single_read(uint8 seq)
{
	uint8 temp[INFO_LENGTH] = { 0 };
	osal_snv_read(TINFO_ADDRESS, INFO_LENGTH, temp);
	if (seq<INFO_LENGTH)
	{
		return temp[seq];
	}
	else{
		return 0xFF;
	}
}

/**************************************
* uint8 flash_Tinfo_single_write(uint8 seq, uint8 value)
* 向flash内部发送数据的存储区域的第seq位存入数据value
* 若seq未超过存储区域的长度，返回读写成功对应值
* 若seq超过存储区域的长度，返回0xFF
**************************************/
uint8 flash_Tinfo_single_write(uint8 seq, uint8 value)
{
	uint8 temp[INFO_LENGTH] = { 0 };
	osal_snv_read(TINFO_ADDRESS, INFO_LENGTH, temp);
	if (seq<INFO_LENGTH)
	{
		temp[seq] = value;
		return osal_snv_write(TINFO_ADDRESS, INFO_LENGTH, temp);
	}
	else
	{
		return 0xFF;
	}
}
//结束TINFO*******************************************************************




//TINFOLEN*******************************************************************
/**************************************
* uint8 flash_Tinfo_Length_init(void）
* 在flash内部初始化发送数据的长度存储位
**************************************/
uint8 flash_Tinfo_Length_init(void)
{
	uint8 temp = 0;
	//地址TINFO_LEN_ADDRESS是发送数据的长度的存储区域
	int8 ret8 = osal_snv_read(TINFO_LEN_ADDRESS, 1, &temp);
	// 如果该段内存未曾写入过数据， 直接读，会返回 NV_OPER_FAILED 
	if (NV_OPER_FAILED == ret8)
	{
		// 把数据结构保存到flash
		osal_snv_write(TINFO_LEN_ADDRESS, 1, &temp);
		osal_snv_read(TINFO_LEN_ADDRESS, 1, &temp);
	}
	return SUCCESS;
}

/**************************************
* uint8 flash_Tinfo_Length_set(uint8 length)
* 在flash内部写入发送数据的长度存储位
**************************************/
uint8 flash_Tinfo_Length_set(uint8 length)
{
	uint8 temp = 0;
	temp = length;
	return osal_snv_write(TINFO_LEN_ADDRESS, 1, &temp);
}

/**************************************
* uint8 flash_Tinfo_Length_get(void)
* 在flash内部读取发送数据的长度存储位
**************************************/
uint8 flash_Tinfo_Length_get(void)
{
	uint8 temp = 1;
	osal_snv_read(TINFO_LEN_ADDRESS, 1, &temp);
	return temp;
}
//结束TINFOLEN*******************************************************************



//RINFO*******************************************************************
/**************************************
* uint8 flash_RinfoPageAddress(uint8 num)
* 获取收到的存储信息第num页的地址
**************************************/
uint8 flash_RinfoPageAddress(uint8 num)
{
	if ((num >= 0) && (num<MAXPAGES))
	{
		return RINFO_ADDRESS1 + num;
	}
	return RINFO_ADDRESS1;
}

/**************************************
* uint8 flash_Rinfo_init(uint8 Addr)
* 在flash内部初始化接收数据存储
**************************************/
uint8 flash_Rinfo_init(uint8 Addr)
{
	uint16* temp;
	temp = (uint16 *)osal_mem_alloc(250);
	osal_memset(temp, 0, 250);
	int8 ret8 = osal_snv_read(Addr, INFO_LENGTH, temp);
	// 如果该段内存未曾写入过数据， 直接读，会返回 NV_OPER_FAILED 
	if (NV_OPER_FAILED == ret8)
	{
		// 把数据结构保存到flash
		osal_snv_write(Addr, INFO_LENGTH, temp);
		osal_snv_read(Addr, INFO_LENGTH, temp);
	}
	osal_mem_free(temp);
	return SUCCESS;
}

/**************************************
* uint8 flash_Rinfo_ALLinit(void)
* 在flash内部初始化所有接收数据存储
**************************************/
uint8 flash_Rinfo_ALLinit(void)
{
	for (int i = 0; i < MAXPAGES; i++)
	{
		flash_Rinfo_init(flash_RinfoPageAddress(i));
	}
        return SUCCESS;
}

////////NFC*******************************************************************
/**************************************
* uint8 flash_Rinfo_all_write(void *pBuf, uint8 add)
* 在flash内部接收数据的全部写入add位置
* 参数是一个长度INFO_LENGTH的数组地址
* 返回值是osal flash操作的值，具体参见API文档
* 一般使用成功是SUCCESS
**************************************/
uint8 flash_Rinfo_all_write(void *pBuf, uint8 addr)
{
	return osal_snv_write(addr, INFO_LENGTH, pBuf);
}

/**************************************
* uint8 flash_Recinfo_Compare_Save(void *pBuf)
* 在已有5页flash接收区写入数据，若数组的ID已有或者存储已满
* 则不存储，一个区域的ID全为0代表该区域可以存储
**************************************/
uint8 flash_Recinfo_Compare_Save(void *pBuf)
{
	uint8 uID[ID_LENGTH] = { 0 };
	//若一个数组的ID全为0，代表该数组可以存储
	uint8 availID[ID_LENGTH] = { 0 };
	int availableSEQ = -1;


	uint8 state = NOEXIST;

	for (int i = 0; i<MAXPAGES; i++)
	{
		flash_Recinfo_getID(uID, flash_RinfoPageAddress(i));
		if (osal_memcmp(uID, pBuf, ID_LENGTH))
		{
			state = EXIST;
			continue;
		}
		else{
			if (osal_memcmp(uID, availID, ID_LENGTH))
			{
				availableSEQ = i;
				break;
			}
		}
	}

	if ((state == NOEXIST) && (availableSEQ != -1))
	{
		flash_Rinfo_add_pages();
		flash_Rinfo_all_write(pBuf, flash_RinfoPageAddress(availableSEQ));
	}
	return 0;
}
/**************************************
* uint8 flash_Recinfo_getID(void *pBuf, uint8 addr)
* 获取收到的存储信息addr个部分的ID值
**************************************/
uint8 flash_Recinfo_getID(void *pBuf, uint8 addr)
{
	uint8 inMem[ID_LENGTH] = { 0 };
	osal_snv_read(addr, ID_LENGTH, inMem);

	for (int i = 0; i<ID_LENGTH; i++)
	{
		((uint8 *)pBuf)[i] = inMem[i];
	}
	return 0;
}
////////NFC*******************************************************************


////////APP*******************************************************************
/**************************************
* uint8 flash_Rinfo_short_read(void *pBuf, uint8 seq, uint8 pageID)
* 在flash内部接收数据区域的第pageID页的第seq处开始为第0位，
* 向后（包括seq）读取长度9的数组
* 若超过存储长度，则在数组后补零
* 赋值给pBuf处
* 注意：这里使用的pageID和内部存储的pages不是一样的值。比如内部存储了5页数据，用flash_Rinfo_get_pages()
* 得到的值是5，但是要输入这里的值是4。也就是说，内部有5页数据，但page的标号是从0到4。
**************************************/
uint8 flash_Rinfo_short_read(void *pBuf, uint8 seq, uint8 pageID)
{
	//分配一个长度为INFO_LENGTH的数组
	uint16* inMem;
	inMem = (uint16 *)osal_mem_alloc(INFO_LENGTH);
	osal_memset(inMem, 0, INFO_LENGTH);

	osal_snv_read(flash_RinfoPageAddress(pageID), INFO_LENGTH, inMem);

	uint8 temp[9] = { 0 };


	uint8 i = 0;
	for (i = 0; i<9; i++)
	{
		if ((seq + i)<INFO_LENGTH)
		{
			temp[i] = inMem[seq + i];
		}
		else
		{
			break;
		}
	}

	for (i = 0; i<9; i++)
	{
		((uint8 *)pBuf)[i] = temp[i];
	}

	//释放内存
	osal_mem_free(inMem);
	return 0;
}
////////APP*******************************************************************




////////BLE*******************************************************************
/**************************************
* uint8 flash_Rinfo_single_read( uint8 Addr, uint8 index )
* 读取flash内部接收数据的存储区域的第Addr第index位存入的数据
* 若seq未超过存储区域的长度，返回对应值
* 若seq超过存储区域的长度，返回0xFF
**************************************/
uint8 flash_Rinfo_single_read(uint8 Addr, uint8 index)
{
	uint8 temp[INFO_LENGTH] = { 0 };
	osal_snv_read(Addr, INFO_LENGTH, temp);
	if (index<INFO_LENGTH)
	{
		return temp[index];
	}
	else{
		return 0xFF;
	}
}

/**************************************
* uint8 flash_Rinfo_single_write(uint8 Addr, uint8 value)
* 向flash内部接收数据的存储区域的第Addr块第index位存入数据value
* 若seq未超过存储区域的长度，返回读写成功对应值
* 若seq超过存储区域的长度，返回0xFF
**************************************/
uint8 flash_Rinfo_single_write(uint8 Addr, uint8 index, uint8 value)
{
	uint8 temp[INFO_LENGTH] = { 0 };
	osal_snv_read(Addr, INFO_LENGTH, temp);
	if (index<INFO_LENGTH)
	{
		temp[index] = value;
		return osal_snv_write(Addr, INFO_LENGTH, temp);
	}
	else
	{
		return Addr;
	}
}
////////BLE*******************************************************************
//结束RINFO*******************************************************************


//RPAGES****该值是用于存放当前flash内有多少数据********************************
/**************************************
* uint8 flash_Rinfo_pages_init(void)
* 在flash内部初始化发送数据的页数存储位
**************************************/
uint8 flash_Rinfo_pages_init(void)
{
	uint8 temp = 0;
	int8 ret8 = osal_snv_read(R_PAGES_ADDRESS, 1, &temp);
	// 如果该段内存未曾写入过数据， 直接读，会返回 NV_OPER_FAILED 
	if (NV_OPER_FAILED == ret8)
	{
		// 把数据结构保存到flash
		osal_snv_write(R_PAGES_ADDRESS, 1, &temp);
		osal_snv_read(R_PAGES_ADDRESS, 1, &temp);
	}
	return SUCCESS;
}

/**************************************
* uint8 flash_Rinfo_add_pages(void)
* 在flash中增加发送数据的页数一页
**************************************/
uint8 flash_Rinfo_add_pages(void)
{
	uint8 temp = 0;
	osal_snv_read(R_PAGES_ADDRESS, 1, &temp);
	temp = temp + 1;
	osal_snv_write(R_PAGES_ADDRESS, 1, &temp);
	return temp;
}

/**************************************
* uint8 flash_Rinfo_minus_pages(void)
* 在flash中减去发送数据的页数一页
**************************************/
uint8 flash_Rinfo_minus_pages(void)
{
	uint8 temp = 0;
	osal_snv_read(R_PAGES_ADDRESS, 1, &temp);
	if (temp>0)
	{
		temp = temp - 1;
	}
	osal_snv_write(R_PAGES_ADDRESS, 1, &temp);
	return temp;
}

/**************************************
* uint8 flash_Rinfo_get_pages(void)
* 在flash中获取发送数据的页数
**************************************/
uint8 flash_Rinfo_get_pages(void)
{
	uint8 temp = 0;
	osal_snv_read(R_PAGES_ADDRESS, 1, &temp);
	return temp;
}
//结束RPAGES*******************************************************************




/**************************************
* uint8 flash_info_init( void )
* 在flash内部初始化所有信息
**************************************/
uint8 flash_info_init(void)
{
	flash_serialNumberInit();
	flash_cashInit();
	flash_pwd_init();
	flash_Tinfo_init();
	flash_Tinfo_Length_init();
	flash_Rinfo_pages_init();
	flash_Rinfo_ALLinit();
        return SUCCESS;
}
