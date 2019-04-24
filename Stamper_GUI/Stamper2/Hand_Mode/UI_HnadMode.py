# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '23/4/19 下午2:00'

from PyQt5.QtCore import Qt, QRect, QMetaObject, QCoreApplication, QObject
from PyQt5.QtWidgets import QPushButton, QLabel

from Common.utils import CommonHelper
from setting import Hand_QSS


class Ui_hand_movement(object):
    """
    手动盖章页面
    """
    def setupUi(self, Form):
        Form.move(0, 0)
        Form.resize(1024, 768)
        Form.setObjectName("hand_movement")

        self.styleFile = Hand_QSS
        style = CommonHelper.readQss(self.styleFile)
        Form.setStyleSheet(style)

        Form.setWindowOpacity(0.9)    # 虚化 0.9
        Form.setWindowFlags(Qt.FramelessWindowHint)

        self.label = QLabel(Form)
        self.label.setGeometry(QRect(326, 140, 420, 600))
        self.label.setObjectName("show_lab")
        self.title_lab = QLabel(Form)
        self.title_lab.setGeometry(QRect(400, 50, 263, 32))
        self.title_lab.setObjectName("title_lab")
        self.cancel_btn = QPushButton(Form)
        self.cancel_btn.setGeometry(QRect(40, 25, 75, 30))
        self.cancel_btn.setObjectName("cancel_btn")
        self.ensure_btn = QPushButton(Form)
        self.ensure_btn.setGeometry(QRect(910, 25, 75, 30))
        self.ensure_btn.setObjectName("ensure_btn")

        self.retranslateUi(Form)
        QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Form"))
        self.title_lab.setText(_translate("Form", "请点击盖章位置"))
        self.cancel_btn.setText(_translate("Form", "取消"))
        self.ensure_btn.setText(_translate("Form", "确定"))
