# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '22/4/19 下午2:59'


from PyQt5.QtCore import Qt, QRect, QMetaObject, QCoreApplication, QObject
from PyQt5.QtWidgets import QPushButton, QLabel, QLineEdit

from Common.utils import CommonHelper
from setting import Serial_QSS


class Ui_Serial(object):
    """
    串口命令发送界面
    """
    def setupUi(self, Form):
        """
        设置界面样式
        :param Form:
        :return:
        """
        Form.move(0, 0)
        Form.resize(1024, 768)
        Form.setObjectName('Serial')
        Form.setWindowFlags(Qt.FramelessWindowHint)

        # 加载QSS样式
        self.styleFile = Serial_QSS
        style = CommonHelper.readQss(self.styleFile)
        Form.setStyleSheet(style)

        # 显示盖章文件
        self.preview_lab = QLabel(Form)
        self.preview_lab.setGeometry(QRect(20, 0, 538, 768))
        self.preview_lab.setObjectName("preview_lab")

        self.retranslateUi(Form)
        QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QCoreApplication.translate

