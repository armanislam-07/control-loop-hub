import paho.mqtt.publish as mqtt

def publishMQTT(message):
    message = int(int(message) * 10) # Scaling for percentages in PWM (Prescaler 1023) 
    mqtt.single("dutycycle", str(message), hostname="192.168.4.152")
