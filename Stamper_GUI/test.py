# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '24/4/19 下午9:00'

import sys

from PyQt5.QtWidgets import QWidget, QPushButton, QLabel, QApplication
# from PyQt5.QtGui import
from PyQt5.QtCore import  QObject
from PyQt5.QtMultimedia import QCamera, QCameraInfo, QCameraImageCapture
from PyQt5.QtMultimediaWidgets import QCameraViewfinder


class Camer(QWidget):
    """
    测试Camer
    """
    def __init__(self):
        super(Camer, self).__init__()
        self.setupUi()

    def setupUi(self):
        """
        设置UI界面
        :return:
        """
        self.resize(600, 600)

        m_camer = QCamera()
        m_ImageCapture = QCameraImageCapture(m_camer)
        m_camerinfo = QCameraInfo()




if __name__ == '__main__':
        app = QApplication(sys.argv)
        win = Camer()
        win.show()
        sys.exit(app.exec_())








