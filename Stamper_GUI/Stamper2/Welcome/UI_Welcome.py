# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '26/3/19 下午4:13'

from PyQt5.QtCore import Qt, QMetaObject, QCoreApplication


class Ui_Main(object):
    """
    欢迎主界面
    """
    def setupUi(self, Form):
        Form.resize(1024, 768)
        Form.move(0, 0)
        # Form.setWindowFlags(Qt.FramelessWindowHint)

        self.retranslateUi(Form)
        QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QCoreApplication.translate