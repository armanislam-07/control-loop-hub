import paho.mqtt.client as mqtt
from i2cSend import i2c_send

#The code that will be running on the Raspberry Pi

def on_connect(client, userdata, flags, rc):
    client.subscribe("dutycycle")

def on_message(client, userdata, msg):
    dutycycle = int(msg.payload.decode("utf-8"))
    if (msg.topic == "dutycycle"):
        print("dutycycle received")
        # i2c_send(dutycycle)



broker_address = "localhost"
port = 1883
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(broker_address, port)
client.loop_forever()


