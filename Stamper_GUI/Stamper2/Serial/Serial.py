# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '21/4/19 下午7:07'

import sys
from PyQt5.QtWidgets import QWidget, QApplication, QPushButton
from PyQt5.QtCore import QRect
from PyQt5.QtSerialPort import QSerialPort, QSerialPortInfo
from PyQt5.QtCore import pyqtSignal, QThread, QObject, QTimer


class SerialWork(QObject):
    """
    串口数据处理
    """
    def __init__(self):
        super().__init__()

    def init(self):
        self.com = QSerialPort()
        self.cominfo = QSerialPortInfo()
        self.infos = self.cominfo.availablePorts()

        for info in self.infos:
            print("Name:", info.portName())
            print("Description:", info.description())
            print("Manufacturer:", info.manufacturer())
            print("Serial Number:", info.serialNumber())
            print("System Location:", info.systemLocation())
            print("vendoridentifier:", info.vendorIdentifier())

            # 串口信息认证
            if info.vendorIdentifier() == 6790:
                self.serianame = info.portName()

        self.com.setPortName(self.serianame)
        self.com.setBaudRate(9600)

        if self.com.open(QSerialPort.ReadWrite) == False:
            return

        self.readtimer = QTimer()
        self.readtimer.timeout.connect(self.readData)     # 读信号
        self.readtimer.start(100)

    def Set_sendData(self, s_data):
        """
        设置要发送的数据
        :param s_data:
        :return:
        """
        self.sendData = bytes(s_data, encoding='utf8')

    def readData(self):
        """
        读数据
        :return:
        """
        revData = self.com.readAll()
        revData = bytes(revData)
        if revData.__len__() != 0:
            print('%d read:' % len(revData))
            print(revData)

    def writeData(self):
        """
        写数据b
        :return:
        """
        if self.com.isOpen():
            # self.com.writeData(bytes('helloworld\r\n', encoding='utf8'))
            self.com.writeData(self.sendData)


class PyQt_Serial(QWidget):
    """
    串口测试模块
    """
    def __init__(self):
        super().__init__()

        self.btn = QPushButton(self)
        self.btn.setObjectName('btn')
        self.btn.setGeometry(QRect(100, 100, 40, 40))

        self.serialthread = QThread()
        self.serialwork = SerialWork()
        self.serialwork.moveToThread(self.serialthread)

        self.serialthread.started.connect(self.serialwork.init)
        self.serialthread.start()

        # self.serialwork.Set_sendData('hello world\r\n')
        self.btn.clicked.connect(self.serialwork.writeData)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = PyQt_Serial()
    win.show()
    sys.exit(app.exec_())





