# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '21/4/19 下午7:07'

import sys
from PyQt5.QtWidgets import QWidget, QApplication
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
        self.serialinfo = QSerialPortInfo()
        self.serialport = self.serialinfo.availablePorts()
        self.serianame = self.serialinfo.portName()
        print("port:", self.serialport)
        print('port_name:', self.serianame)

        self.com.setPortName('COM7')
        self.com.setBaudRate(9600)

        if self.com.open(QSerialPort.ReadWrite) == False:
            return

        self.readtimer = QTimer()
        self.readtimer.timeout.connect(self.readData)     # 读信号
        self.readtimer.start(100)

    def readData(self):
        """
        读数据处理
        :return:
        """
        revData = self.com.readAll()
        revData = bytes(revData)
        print('%d read' %len(revData))


class PyQt_Serial(QWidget):
    def __init__(self):
        super().__init__()

        self.serialthread = QThread()
        self.serialwork = SerialWork()
        self.serialwork.moveToThread(self.serialthread)
        self.serialthread.started.connect(self.serialwork.init)
        self.serialthread.start()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = PyQt_Serial()
    win.show()
    sys.exit(app.exec_())





