from PySide6.QtWidgets import QApplication, QMessageBox
from PySide6.QtCore import QFile, QIODevice
from PySide6.QtUiTools import QUiLoader

app = QApplication()

loader = QUiLoader()
file = QFile("main.ui")
file.open(QFile.ReadOnly)

window = loader.load(file)
file.close()

window.pushButton.clicked.connect(
    lambda: QMessageBox.information(window, "Message", window.lineEdit.text())
    #this will be connected to send a uart signal to the esp32 on the com port

)

window.show()

app.exec()