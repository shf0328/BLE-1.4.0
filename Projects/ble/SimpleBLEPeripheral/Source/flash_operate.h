/*********************************************************************
* INCLUDES
*/
#include "hal_types.h"
#include "osal_snv.h"


/*********************************************************************
* CONSTANT
*/
#define SERIAL_ADDRESS		0x80
#define SERIAL_LENGTH		3

#define CASH_ADDRESS		0x81
#define CASH_LENGTH			4


#define PASSWORD_ADDRESS	0x90  

#define INFO_LENGTH			150

#define TINFO_ADDRESS		0x91
#define TINFO_LEN_ADDRESS	0x92

#define MAXPAGES			5 

#define R_PAGES_ADDRESS		0x93

#define ID_LENGTH			16

#define RINFO_ADDRESS1		0x94
#define RINFO_ADDRESS2		0x95
#define RINFO_ADDRESS3		0x96
#define RINFO_ADDRESS4		0x97
#define RINFO_ADDRESS5		0x98


    
#define EXIST		1
#define NOEXIST		0
  


#define INFO_PAGES_ADDR    0x91
#define INFO_PAGES_LEN    1

/*********************************************************************
* FUNCTIONS
*/
//序列号**********************************************************************
/**************************************
* uint8 flash_serialNumberInit(void);
* 序列号的初始化
**************************************/
uint8 flash_serialNumberInit(void);

/**************************************
* uint8 flash_generateSerialNumber(void *pBuf);
* 产生一串3位序列号，数组地址开始位pBuf
**************************************/
extern uint8 flash_generateSerialNumber(void *pBuf);

/**************************************
* uint8 flash_getSerialNumber(void *pBuf);
* 获取产生的3位序列号，赋值给数组地址pBuf
**************************************/
extern uint8 flash_getSerialNumber(void *pBuf);
//结束序列号******************************************************************




//余额************************************************************************
/**************************************
* uint8 flash_cashInit(void);
* 余额的初始化
**************************************/
uint8 flash_cashInit();

/**************************************
* uint8 flash_save_cash(void *pBuf);
* 存储余额
**************************************/
extern uint8 flash_save_cash(void *pBuf);

/**************************************
* uint8 flash_get_cash(void *pBuf);
* 获取余额，放到一个pBuf的数组里面
**************************************/
extern uint8 flash_get_cash(void *pBuf);
//结束余额********************************************************************





//密码************************************************************************
/*************************************
* uint8 flash_pwd_init( void )
* 在flash内部密码的初始化函数
* 即在对应位存入相应的初始密码，8个0
**************************************/
extern uint8 flash_pwd_init(void);

/*************************************
* uint8 flash_pwd_write(void *pBuf)
* 向flash内部写入密码
* 参数pBuf为一个8位uint8的数组的地址
**************************************/
extern uint8 flash_pwd_write(void *pBuf);

/**************************************
* uint8 flash_pwd_read(void *pBuf)
* 读取flash内部存入的密码
* 参数pBuf为一个8位uint8数组的地址
**************************************/
extern uint8 flash_pwd_read(void *pBuf);

/**************************************
* uint8 flash_pwd_delete(void *pBuf)
* 清空flash内部存入的密码
**************************************/
extern uint8 flash_pwd_delete(void);
//结束密码*******************************************************************





//TINFO************************************************************************
/**************************************
* uint8 flash_Tinfo_init( void )
* 在flash内部初始化发送数据的存储
**************************************/
extern uint8 flash_Tinfo_init(void);

/**************************************
* uint8 flash_Tinfo_all_read(void *pBuf)
* 在flash内部发送数据的全部读取
* 参数是一个长度INFO_LENGTH的数组地址
* 返回值是osal flash操作的值，具体参见API文档
* 一般使用成功是SUCCESS
**************************************/
extern uint8 flash_Tinfo_all_read(void *pBuf);

/**************************************
* uint8 flash_Tinfo_short_write(void *pBuf, uint8 len)
* 在flash内部发送数据区域的接收数据的长度为s
* 则向s后面写入长度为len的数组，地址是pBuf
* 若超过存储长度的数据不写
**************************************/
extern uint8 flash_Tinfo_short_write(void *pBuf, uint8 len);

/**************************************
* uint8 flash_Tinfo_single_read( uint8 seq )
* 读取flash内部发送数据的存储区域的第seq个位存入的数据
* 若seq未超过存储区域的长度，返回对应值
* 若seq超过存储区域的长度，返回0xFF
**************************************/
extern uint8 flash_Tinfo_single_read(uint8 seq);

/**************************************
* uint8 flash_Tinfo_single_write(uint8 seq, uint8 value)
* 向flash内部发送数据的存储区域的第seq位存入数据value
* 若seq未超过存储区域的长度，返回读写成功对应值
* 若seq超过存储区域的长度，返回0xFF
**************************************/
extern uint8 flash_Tinfo_single_write(uint8 seq, uint8 value);
//结束TINFOLEN*******************************************************************




//TINFOLEN*******************************************************************
/**************************************
* uint8 flash_Tinfo_Length_init(void）
* 在flash内部初始化发送数据的长度存储位
**************************************/
extern uint8 flash_Tinfo_Length_init(void);

/**************************************
* uint8 flash_Tinfo_Length_set(uint8 length)
* 在flash内部写入发送数据的长度存储位
**************************************/
extern uint8 flash_Tinfo_Length_set(uint8 length);

/**************************************
* uint8 flash_Tinfo_Length_get(void)
* 在flash内部读取发送数据的长度存储位
**************************************/
extern uint8 flash_Tinfo_Length_get(void);
//结束TINFO*******************************************************************

//RINFO*******************************************************************
/**************************************
* uint8 flash_Rinfo_page_clear(uint8 page)
*让flash内部的接受区域第page页清零
**************************************/
extern uint8 flash_Rinfo_page_clear(uint8 page);

/**************************************
* uint8 flash_Rinfo_all_clear(void)
*让flash内部的接受区域全部清零
**************************************/
extern uint8 flash_Rinfo_all_clear(void);

/**************************************
* uint8 flash_RinfoPageAddress(uint8 num)
* 获取收到的存储信息第seq页的地址
**************************************/
extern uint8 flash_RinfoPageAddress(uint8 num);

/**************************************
* uint8 flash_Rinfo_init(uint8 Addr）
* 在flash内部初始化接收数据存储
**************************************/
uint8 flash_Rinfo_init(uint8 Addr);

/**************************************
* uint8 flash_Rinfo_ALLinit(void)
* 在flash内部初始化所有接收数据存储
**************************************/
extern uint8 flash_Rinfo_ALLinit(void);



////////NFC*******************************************************************
/**************************************
* uint8 flash_Rinfo_all_write(void *pBuf, uint8 addr)
* 在flash内部接收数据的全部写入add位置
* 参数是一个长度INFO_LENGTH的数组地址
* 返回值是osal flash操作的值，具体参见API文档
* 一般使用成功是SUCCESS
**************************************/
extern uint8 flash_Rinfo_all_write(void *pBuf, uint8 addr);

/**************************************
* uint8 flash_Rinfo_all_write(void *pBuf, uint8 addr)
**************************************/
extern uint8 flash_Rinfo_all_read(void *pBuf, uint8 pages);

/**************************************
* uint8 flash_Recinfo_getID(void *pBuf, uint8 addr)
* 获取收到的存储信息addr个部分的ID值
**************************************/
uint8 flash_Recinfo_getID(void *pBuf, uint8 addr);

/**************************************
* uint8 flash_Recinfo_Compare_Save(void *pBuf)
* 在已有5页flash接收区写入数据，若数组的ID已有或者存储已满
* 则不存储，一个区域的ID全为0代表该区域可以存储
**************************************/
extern uint8 flash_Recinfo_Compare_Save(void *pBuf);
////////NFC*******************************************************************


////////APP*******************************************************************
/**************************************
* uint8 flash_Rinfo_short_read(void *pBuf, uint8 seq, uint8 pages)
* 在flash内部接收数据区域的第page页的第seq处开始为第0位，
* 向后（包括seq）读取长度9的数组
* 若超过存储长度，则在数组后补零
* 赋值给pBuf处
**************************************/
extern uint8 flash_Rinfo_short_read(void *pBuf, uint8 seq, uint8 pages);
////////APP*******************************************************************


////////BLE*******************************************************************
/**************************************
* uint8 flash_Rinfo_single_read( uint8 Addr, uint8 index )
* 读取flash内部接收数据的存储区域的第Addr第index位存入的数据
* 若seq未超过存储区域的长度，返回对应值
* 若seq超过存储区域的长度，返回0xFF
**************************************/
extern uint8 flash_Rinfo_single_read(uint8 Addr, uint8 index);

/**************************************
* uint8 flash_Rinfo_single_write(uint8 Addr,uint8 index, uint8 value)
* 向flash内部接收数据的存储区域的第Addr块第index位存入数据value
* 若seq未超过存储区域的长度，返回读写成功对应值
* 若seq超过存储区域的长度，返回0xFF
**************************************/
extern uint8 flash_Rinfo_single_write(uint8 Addr, uint8 index, uint8 value);
////////BLE*******************************************************************
//结束RINFO*******************************************************************



//RPAGES****该值是用于存放当前flash内有多少数据********************************
/**************************************
* uint8 flash_Rinfo_pages_init(void)
* 在flash内部初始化发送数据的页数存储位
**************************************/
uint8 flash_Rinfo_pages_init(void);

/**************************************
* uint8 flash_Rinfo_add_pages(void)
* 在flash中增加发送数据的页数一页
**************************************/
extern uint8 flash_Rinfo_add_pages(void);

/**************************************
* uint8 flash_Rinfo_minus_pages(void)
* 在flash中减去发送数据的页数一页
**************************************/
extern uint8 flash_Rinfo_minus_pages(void);

/**************************************
* uint8 flash_Rinfo_minus_pages(void)
* 在flash中获取发送数据的页数
**************************************/
extern uint8 flash_Rinfo_get_pages(void);

/**************************************
* uint8 flash_Rinfo_get_pages(void)
* 在flash中获取发送数据的页数
**************************************/
extern uint8 flash_Rinfo_get_pages(void);
//结束RPAGES*******************************************************************



/**************************************
* uint8 flash_info_init( void )
* 在flash内部初始化所有信息
**************************************/
extern uint8 flash_info_init(void);
