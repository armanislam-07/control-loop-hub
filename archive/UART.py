import serial

# Number refers to the value we will get from the pyside Application
# COM address is the port that our board is connected to from our computer | Note that this will change depending on different test environments
def sendUart(number, port):
    sendAddress = serial.Serial(port, 9600) 
    message = number #number that we will get from the pyside application
    sendAddress.write(str(message).encode('utf-8'))
    sendAddress.close()
    



