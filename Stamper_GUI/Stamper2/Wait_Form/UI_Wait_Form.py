# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '26/3/19 下午5:23'

from PyQt5.QtCore import Qt, QRect, QMetaObject, QCoreApplication
from PyQt5.QtWidgets import QApplication, QLabel

from setting import back_img, wait_img


class Ui_shadow(object):
    """
    阴影界面
    """
    def setupUi(self, Form):
        # 创建罩层界面
        Form.move(0, 0)
        Form.resize(1024, 768)
        Form.setWindowOpacity(0.6)    # 虚化 0.7
        Form.setObjectName("shadow")
        Form.setStyleSheet('#shadow{\
                            background-color: rgba(0, 0, 0, 0);\
                            border-image:url('+back_img+');}')
        Form.setWindowFlags(Qt.FramelessWindowHint)    # 无边框


class Ui_Wait(object):
    """
    等待界面,
    """
    def setupUi(self, Form):
        Form.setObjectName("wait")
        Form.resize(450, 140)
        Form.raise_()

        screenResolution = QApplication.desktop()
        x = (screenResolution.width() - Form.geometry().width()) / 2
        y = (screenResolution.height() - Form.geometry().height()) / 2
        self.move(x, y)

        # 设置无边框、置顶
        self.setWindowFlags(Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint)
        # 设置背景透明而控件不透明
        self.setWindowOpacity(1)
        self.setAttribute(Qt.WA_TranslucentBackground)

        self.setStyleSheet("#wait{\
                                background-color: rgba(0, 0, 0, 0);\
                                border-image:url(" + wait_img +");}")

        self.wait_lab = QLabel(Form)
        self.wait_lab.setObjectName("wait_lab")
        self.wait_lab.setGeometry(QRect(10, 10, 440, 130))
        self.wait_lab.setStyleSheet("#wait_lab{\
                                        background-color: rgba(0, 0, 0, 0);\
                                        font-weight:Regular; \
                                        font-family:SourceHanSansCN-Regular;\
                                        font-size:36px;\
                                        line-height:7px;\
                                        color:#FFFFFF;\
                                        text-align:center; \
                                        }")
        self.wait_lab.setWordWrap(True)

        # 该AlignCenter标志不属于该PyQt5.Qt模块，而属于PyQt5.QtCore.Qt该类。设置文字水平垂直居中
        self.wait_lab.setAlignment(Qt.AlignHCenter | Qt.AlignVCenter)

        self.wait_lab.setText("打印中，请稍候...")

        self.retranslateUi(Form)
        QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QCoreApplication.translate

