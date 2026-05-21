from PySide6.QtWidgets import QApplication, QMessageBox
from PySide6.QtCore import QFile, QIODevice
from PySide6.QtUiTools import QUiLoader
from MQTT.mqtt_pub import publishMQTT

app = QApplication()

loader = QUiLoader()
file = QFile("main.ui")
file.open(QFile.ReadOnly)

window = loader.load(file)
file.close()

window.pushButton.clicked.connect(
    lambda: publishMQTT(window.lineEdit.text())
    #this will be connected to send a uart signal to the esp32 on the com port
    #use mqtt to send a message then mqtt -> i2c
    #QMessageBox.information(window, "Message", window.lineEdit.text())

)

window.show()

app.exec()