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

#define INFO_LENGTH			250  
#define TINFO_ADDRESS		0x90


#define MAXPAGES   5     
  
   

#define CASH_ADDRESS    0x81
#define CASH_LENGTH    4

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

//结束TINFO*******************************************************************
