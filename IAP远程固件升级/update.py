from ctypes import *
import struct


_RobotID = bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99])

_frame_head = 0x10      #帧头
_frame_tail = 0x88      #帧尾
_frame_type_cmd = 0x41  #帧类型：命令帧
_frame_type_data = 0x4A #帧类型：数据帧

_cmd_type_start = 0     #命令类型：开始传输固件
_cmd_type_end = 1       #命令类型：结束传输固件
_cmd_type_upgrade = 2   #命令类型：刷新固件

_data_type_name = 0     #数据类型：固件名
_data_type_content = 1  #数据类型：固件内容
_data_type_md5 = 2      #数据类型：MD5校验值

_tramsmit_max = 230     #一次传输的最大数据量

def checksum(data):
    sum = 0
    for i in data:
        sum ^= i
    return sum

def update_show(str_):
    for i in str_:
        print('{:02x}'.format(i), end=' ')
    print('', end='\n')

class UpdateTransmitCmd_Req(BigEndianStructure):
    """
    固件传输命令问询帧，由上位机发送下位机接收。
    """
    _pack_=1
    _fields_=[
        ('head', c_ubyte),
        ('type', c_ubyte),
        ('length', c_ubyte),
        ('ID', c_ubyte * 9),
        ('command', c_ubyte),
        ('firmwareSize', c_uint32),
        ('reserve', c_uint32),
        ('checkSum', c_ubyte),
        ('tail', c_ubyte)
    ]

class UpdateTransmitCmd_Ack(BigEndianStructure):
    """"
    固件传输命令回应帧，由下位机发送上位机接收。
    """
    _pack_=1
    _fields_=[
        ('head', c_ubyte),
        ('type', c_ubyte),
        ('length', c_ubyte),
        ('ID', c_ubyte * 9),
        ('acknowledge', c_ubyte),
        ('reserve', c_uint32),
        ('checkSum', c_ubyte),
        ('tail', c_ubyte)
    ]

class UpdateTransmit_Ack(BigEndianStructure):
    """
    固件传输数据帧的回应帧，由下位机发送上位机接收。
    """
    _pack_=1
    _fields_=[
        ('head', c_ubyte),
        ('type', c_ubyte),
        ('length', c_ubyte),
        ('ID', c_ubyte * 9),
        ('acknowledge', c_ubyte),
        ('nextoffset', c_uint32),
        ('reserve', c_uint32),
        ('checkSum', c_ubyte),
        ('tail', c_ubyte)
    ]

def update_get_transmitCmd_frame(cmd, size_):
    """
    cmd: 0表示开始传输，1表示结束传输。
    size_: 固件的大小。
    """
    frame = UpdateTransmitCmd_Req()
    frame.head = _frame_head
    frame.type = _frame_type_cmd
    frame.length = sizeof(frame) - 5
    frame.ID = (c_ubyte * 9)(*_RobotID)
    # frame.ID = (c_ubyte * 9)(0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99)
    frame.command = cmd
    frame.firmwareSize = size_ 
    string = string_at(addressof(frame), sizeof(frame))
    frame.checkSum = checksum(string[:-2])
    frame.tail = _frame_tail
    return string_at(addressof(frame), sizeof(frame))

def update_get_transmit_frame(type_, data, offset = 0):
    """
    type_: 数据类型 0：固件名 1：固件内容 2：固件的MD5校验值
    data: 数据
    offset: 只有数据类型type_为1固件内容时这个参数才有意义。它表示当前传输的数据
        在整个固件中的偏移位置。
    """
    length = 19 + len(data)
    frame = struct.pack('BBB', _frame_head, _frame_type_data, length)   # 帧头 帧类型 数据长度
    frame += struct.pack('{}s'.format(len(_RobotID)), _RobotID)         # 机器人ID
    frame += struct.pack('B', type_)                                    # 数据类型 0：固件名 1：固件内容 2：固件的MD5校验值
    frame += struct.pack('!BI', len(data), offset)                      # 数据长度，和数据偏移
    frame += data
    frame += struct.pack('I', 0)                                        # reserve
    cksum = checksum(frame)                                             # 校验码
    frame += struct.pack('BB', cksum, _frame_tail)                      # 帧尾
    return frame

def update_receive_check(reqFrame, recData, ackFrame, pos):
    """
    reqFrame: 发送的数据帧，数据类型的str。
    recData: 接收到的回复数据，数据类型是str。
    ackFrame: UpdateTransmitCmd_Ack、UpdateTransmit_Ack这两个类的实例。
    pos: 传输步骤。
    """
    if len(recData) == 0:
        print("通信超时 {} ！".format(pos))
        sys.exit()
    if reqFrame[1] != recData[1] - 1:
        print("回复帧类型错误 {} ！".format(pos))
        sys.exit()
    if checksum(recData[:-1]) != 0:
        print("校验错误 {} ！".format(pos))
        sys.exit()
    memmove(addressof(ackFrame), recData, sizeof(ackFrame)) # 把接收到的数据移入类的实例中
    if string_at(addressof(ackFrame.ID), sizeof(ackFrame.ID)) != _RobotID:
        print("设备ID错误 {} ！".format(pos))
        sys.exit()
    if ackFrame.acknowledge & (1<<0):
        print("存储器错误 {} ！".format(pos))
        sys.exit()
    if ackFrame.acknowledge & (1<<1):
        print("设备硬件错误 {} ！".format(pos))
        sys.exit()
    if ackFrame.acknowledge & (1<<2):
        print("设备任务错误 {} ！".format(pos))
        sys.exit()
    if ackFrame.acknowledge & (1<<3):
        print("通信错误 {} !".format(pos))
        sys.exit()
    return ackFrame

def socket_write(frame):
    """
    使用网络发送数据
    """
    global sock
    global NetAddress
    sock.sendto(frame, NetAddress)

def socket_read(count):
    """
    使用网络读取数据
    """
    global sock
    global NetAddress
    try:
        data, addr = sock.recvfrom(1024)
    except:
        data = ''
    return data

def serial_write(frame):
    """
    使用串口发送数据
    """
    global COM
    COM.write(frame)

def serial_read(count):
    """
    使用串口读取数据
    """
    global COM
    return COM.read(count)

if __name__ == "__main__":
    import sys, hashlib, time, serial
    from socket import *

    #初始化网络，并使用网络通信
    # NetAddress = ('192.168.3.20', 9000)
    # sock = socket(AF_INET, SOCK_DGRAM)
    # sock.setblocking(True)
    # sock.settimeout(2)
    # update_write = socket_write
    # update_read = socket_read

    #初始化串口，并使用者串口通信
    COM = serial.Serial('COM10', 115200, timeout = 2)
    update_write = serial_write
    update_read = serial_read

    if len(sys.argv) == 2:
        fileName = sys.argv[1] 
    else:
        fileName = 'firmware.bin'
    data = open(fileName, 'rb').read()

    # 开始传输固件
    frame = update_get_transmitCmd_frame(_cmd_type_start, len(data))
    update_write(frame)
    #读取回复并做处理
    recData = update_read(sizeof(UpdateTransmitCmd_Ack))
    ack = UpdateTransmitCmd_Ack()
    update_receive_check(frame, recData, ack, 1)

    #传输固件名。固件名不要超过200个字节。因为下位机的接收缓存只有256个字节。
    if len(fileName) > 12: # FATFS使用了短文件名只支持12个字符的文件名
        print('文件名过长，文件名不能超过12个字符')
        sys.exit()
    fileName += '\0' #转成C字符串形式
    frame = update_get_transmit_frame(_data_type_name, bytes(fileName, encoding='ASCII'))
    update_write(frame)
    #读取回复并做处理
    recData = update_read(sizeof(UpdateTransmit_Ack))
    ack = UpdateTransmit_Ack()
    update_receive_check(frame, recData, ack, 2)

    #传输固件数据
    count = len(data)
    offset = 0
    ack = UpdateTransmit_Ack()
    while count > 0:
        if count <= _tramsmit_max:
            frame = update_get_transmit_frame(_data_type_content, data[offset:], offset)
            offset += count
            count -= count
        else:
            frame = update_get_transmit_frame(_data_type_content, data[offset:offset+230], offset)
            offset += _tramsmit_max
            count -= _tramsmit_max
        update_write(frame)
        #读取回复并做处理
        recData = update_read(sizeof(UpdateTransmit_Ack))
        ack = update_receive_check(frame, recData, ack, 3)
        if ack.nextoffset != offset:
            print("传输不同步")
            sys.exit()
 
    #传输固件的MD5校验值
    md5 = hashlib.md5(data).digest()
    frame = update_get_transmit_frame(_data_type_md5, md5)
    update_write(frame)
    #读取回复并做处理
    recData = update_read(sizeof(UpdateTransmit_Ack))
    ack = UpdateTransmit_Ack()
    update_receive_check(frame, recData, ack, 4)

    #结束固件传输
    frame = update_get_transmitCmd_frame(_cmd_type_end, len(data))
    update_write(frame)
    recData = update_read(sizeof(UpdateTransmitCmd_Ack))
    ack = UpdateTransmitCmd_Ack()
    update_receive_check(frame, recData, ack, 5)
    print("固件下载成功")

    #升级固件
    print("固件升级...")
    frame = update_get_transmitCmd_frame(_cmd_type_upgrade, len(data))
    update_write(frame)
