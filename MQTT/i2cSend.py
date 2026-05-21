from smbus2 import SMBus

def i2c_send(message):
    with SMBus(1) as bus:
        # Send the I2C message
        bus.write_byte(0x42, message)