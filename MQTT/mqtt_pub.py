import paho.mqtt.publish as mqtt

def publishMQTT(message):
    mqtt.single("dutycycle", str(message), hostname="192.168.4.152")
