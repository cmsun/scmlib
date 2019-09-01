#ifndef MBERRORH
#define MBERRORH

//这里定义的返回错误不是modubs的异常码。
#define MODBUS_NO_ERROR         0   //接收到的modbus报文没有错误
#define MODBUS_CHKSUM_ERROR     1   //校验错误
#define MODBUS_DEVID_ERROR      2   //设备地址错误。比如master查询的是设备地址为1的设备的数据，但是设备地址为2的设备回复的主机查询
#define MODBUS_SLAVE_ERROR      3   //slave返回错误码。比如master用01功能码查询，而查询的地址超出范围，slave就返回了错误码
#define MODBUS_FUNCODE_ERROR    4   //功能码错误。比如master用功能码01查询的是位变量，但是slave用功能码03回复了寄存器变量
#define MODBUS_FORMAT_ERROR     5   //格式错误。比如master查询3个寄存器变量，但是slave回复了5个寄存器变量
#define MODBUS_TIMEOUT_ERROR    6   //通讯超时。重复发送三次没有接收到slave的回复，则通信超时。
#define MODBUS_UNEXPECT_ERROR   7   //主机询问后，下位机才能回复，如果主机没有询问而收到modbus报文，则认为是未知信息通信错误

#endif
