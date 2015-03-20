//3月20日更新
在osal里面设置了省电和耗能模式的切换功能osal_changepowerstate
请将下载的文件替换原有的osal.c和osal.h


//3月17日更新
预定义旗变为
INT_HEAP_LEN=3072
HALNODEBUG
OSAL_CBTIMER_NUM_TASKS=1
HAL_AES_DMA=TRUE
HAL_DMA=TRUE
POWER_SAVING
xPLUS_BROADCASTER
HAL_LCD=TRUE
HAL_LED=FALSE
HAL_UART_ISR=1
HAL_UART_DMA=0
HAL_UART=TRUE
DEBUG
HAL_UART_ISR_HIGH=9

//2月17日 更新说明
文件说明：
Profile\SimpleProfile\
有四个文件
simpleGATTprofile.c
simpleGATTprofile.h
flash_operate.c	
flash_operate.h	


SimpleBLEPeripheral\PN532_NFCLIB
PN532_NFC.c
PN532_NFC.h

SimpleBLEPeripheral\Source
OSAL_SimpleBLEPeripheral.c	
PN532_NFC.h//该文件与上面的文件相同，为防止IAR报错找不到	
SimpleBLEPeripheral_Main.c	
flash_operate.c	
flash_operate.h	
simpleBLEPeripheral.c	
simpleBLEPeripheral.h

使用说明
找到\BLE-1.4.0\Projects\ble\Profiles\SimpleProfile这个文件夹
替换2个同名文件
simpleGATTprofile.c
simpleGATTprofile.h
并新添加文件
flash_operate.c	
flash_operate.h	

找到\BLE-1.4.0\Projects\ble\
用下载的SimpleBLEPeripheral移动到电脑上的SimpleBLEPeripheral文件夹，若提示替换，点击确定

工程编译说明
请修改工程.eww文件
添加group"PN532_NFCLIB"
并在该group下添加PN532_NFCLIB文件夹中的PN532_NFC.c和PN532_NFC.h两个文件
在APP下添加SimpleBLEPeripheral文件夹中的文件
flash_operate.c	
flash_operate.h	

关于预定义头的问题，设置如下
右键工程名，option，c++ compiler，preprocessor
Defined symbol：

INT_HEAP_LEN=3072
HALNODEBUG
OSAL_CBTIMER_NUM_TASKS=1
HAL_AES_DMA=TRUE
HAL_DMA=TRUE
POWER_SAVING
xPLUS_BROADCASTER
HAL_LCD=TRUE
HAL_LED=FALSE
HAL_UART_ISR=1
HAL_UART_DMA=0
HAL_UART=TRUE




//2月11日 更新
文件说明：
Profile\SimpleProfile\
有两个文件
simpleGATTprofile.c
simpleGATTprofile.h

SimpleBLEPeripheral\PN532_NFCLIB
PN532_NFC.c
PN532_NFC.c

SimpleBLEPeripheral\Source
OSAL_SimpleBLEPeripheral.c	
PN532_NFC.h//该文件与上面的文件相同，为防止IAR报错找不到	
SimpleBLEPeripheral_Main.c	
flash_operate.c	
flash_operate.h	
simpleBLEPeripheral.c	
simpleBLEPeripheral.h

使用说明
找到\BLE-1.4.0\Projects\ble\Profiles\SimpleProfile这个文件夹
替换2个同名文件
simpleGATTprofile.c
simpleGATTprofile.h

找到\BLE-1.4.0\Projects\ble\
用下载的SimpleBLEPeripheral移动到电脑上的SimpleBLEPeripheral文件夹，若提示替换，点击确定

工程编译说明
请修改工程.eww文件
添加group"PN532_NFCLIB"
并在该group下添加PN532_NFCLIB文件夹中的PN532_NFC.c和PN532_NFC.h两个文件
在APP下添加SimpleBLEPeripheral文件夹中的文件
flash_operate.c	
flash_operate.h	



//1月29日分支合并
添加了ble\SimpleBLEPeripheral\PN532_NFCLIB文件夹,以及其中的PN532_NFC.c和PN532_NFC.h文件.
请修改工程.eww文件,添加group"PN532_NFCLIB",并在该group下添加PN532_NFCLIB文件夹中的PN532_NFC.c和PN532_NFC.h两个文件

//1月27日更新说明
下载下来后,解压一共有6个文件
simpleGATTprofile.c
simpleGATTprofile.h
OSAL_SimpleBLEPeripheral.c
simpleBLEPeripheral.h
simpleBLEPeripheral.c
SimpleBLEPeripheral_Main.c

找到\BLE-1.4.0\Projects\ble\Profiles\SimpleProfile这个文件夹
替换2个同名文件
simpleGATTprofile.c
simpleGATTprofile.h

找到\BLE-1.4.0\Projects\ble\SimpleBLEPeripheral\Source这个文件夹
替换4同名文件
OSAL_SimpleBLEPeripheral.c
simpleBLEPeripheral.h
simpleBLEPeripheral.c
SimpleBLEPeripheral_Main.c

直接用IAR打开，编译下载即可