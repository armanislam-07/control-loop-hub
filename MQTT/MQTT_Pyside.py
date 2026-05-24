import paho.mqtt.client as mqtt

def on_message(client, userdata, message):
    # send this message to matlab
    if(message.topic == "temperature" ):
        return message.payload.decode("utf-8")

broker_address = "localhost" # Changes depending on network
port = 1883

client = mqtt.Client()
client.on_message = on_message
client.connect(broker_address, port)
client.loop_forever()

