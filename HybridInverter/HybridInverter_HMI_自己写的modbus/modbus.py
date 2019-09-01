from pymodbus.client.sync import ModbusSerialClient

def set_data_mode(mode = 2):
    result = client.write_register(0x0009, mode)
    print(result)


client = ModbusSerialClient(method='rtu', port='COM1', baudrate=115200, bytesize=8, parity ='N', stopbits=1)
client.connect()
# result = client.write_register(1, 0xff, unit=1)
result = client.write_registers(1, [123, 456, 789, 0xffff], unit=1)
print(result)
result = client.read_holding_registers(1, 4, unit = 1)
print(result.registers)
