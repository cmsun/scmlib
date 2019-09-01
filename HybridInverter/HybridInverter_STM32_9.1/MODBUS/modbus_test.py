import sys
import time
from pymodbus.client.sync import ModbusSerialClient

client = ModbusSerialClient(method='rtu', port='COM1', baudrate=115200, timeout=0.5)
client.write_register(0, 12345, unit=1)
while True:
    result = client.read_holding_registers(0, 1, unit=1)
    print(result, result.registers)
    sys.stdout.flush()
    time.sleep(0.05)
