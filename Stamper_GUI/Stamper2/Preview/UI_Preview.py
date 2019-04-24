# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '22/4/19 下午5:34'


from PyQt5.QtCore import Qt, QRect, QMetaObject, QCoreApplication, QObject
from PyQt5.QtWidgets import QPushButton, QLabel, QComboBox, QWidget, QStyleOption, QStyle
from PyQt5.QtGui import QPainter

from Common.utils import CommonHelper
from setting import Preview_QSS, Seal_enum


class Ui_preview(object):
    """
    盖章文件预览
    """
    def __init__(self):
        pass

    def setupUi(self, Form):
        Form.move(0, 0)
        Form.resize(1024, 768)
        Form.setObjectName('preview')
        # Form.setWindowFlags(Qt.FramelessWindowHint)

        # 加载QSS样式
        self.styleFile =Preview_QSS
        style = CommonHelper.readQss(self.styleFile)
        Form.setStyleSheet(style)

        self.cancel_btn = QPushButton(Form)
        self.cancel_btn.setGeometry(QRect(27, 25, 100, 40))
        self.cancel_btn.setObjectName("cancel_btn")

        self.ensure_btn = QPushButton(Form)
        self.ensure_btn.setGeometry(QRect(900, 25, 100, 40))
        self.ensure_btn.setObjectName("ensure_btn")

        self.Seal_type = QComboBox(Form)
        self.Seal_type.setGeometry(QRect(810, 480, 100, 100))
        self.Seal_type.setObjectName("seal_type")
        self.Seal_type.addItem("Seal_1", Seal_enum['Seal_1'])
        self.Seal_type.addItem("Seal_2", Seal_enum['Seal_2'])
        self.Seal_type.addItem("Seal_3", Seal_enum['Seal_3'])
        self.Seal_type.addItem("Seal_4", Seal_enum['Seal_4'])
        self.Seal_type.setMaximumHeight(100)

        self.Seal_label = QLabel(Form)
        self.Seal_label.setGeometry(QRect(920, 480, 100, 100))
        self.Seal_label.setObjectName("Seal_label")

        self.Rc_btn = QPushButton(Form)
        self.Rc_btn.setGeometry(QRect(800, 560, 146, 47))
        self.Rc_btn.setObjectName("Rc_btn")

        self.Hand_btn = QPushButton(Form)
        self.Hand_btn.setGeometry(QRect(800, 620, 146, 47))
        self.Hand_btn.setObjectName("Hand_btn")

        # 改变盖章位置
        # self.change_psition_btn = QPushButton(Form)
        # self.change_psition_btn.setGeometr(QRect(850, 680, 146, 47))
        # self.change_psition_btn.setObjectName("change_psition_btn")

        self.title_lab = QLabel(Form)
        self.title_lab.setGeometry(QRect(450, 82, 209, 28))
        self.title_lab.setObjectName("title_lab")

        self.preview_lab = QLabel(Form)
        self.preview_lab.setGeometry(QRect(326, 143, 370, 522))
        self.preview_lab.setObjectName("preview_lab")

        self.retranslateUi(Form)
        QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QCoreApplication.translate
        self.cancel_btn.setText(_translate("Form", "取消"))
        self.ensure_btn.setText(_translate("Form", "确定"))
        self.title_lab.setText(_translate("Form", "文件预览"))
        self.Rc_btn.setText(_translate("Form", "识别盖章"))
        self.Hand_btn.setText(_translate("Form", "手动盖章"))
        self.Seal_label.setText(_translate("Form", "选择印章"))


class Test_Form(QWidget, Ui_preview):
    """
    测试模块
    """

    def __init__(self, parent=None):
        super(Test_Form, self).__init__(parent=parent)

        self.setupUi(self)

    def paintEvent(self, event):
        """
        重写paintEvent 否则不能使用样式表定义外观
        :param evt:
        :return:
        """
        opt = QStyleOption()
        opt.initFrom(self)
        painter = QPainter(self)
        # 反锯齿
        painter.setRenderHint(QPainter.Antialiasing)
        self.style().drawPrimitive(QStyle.PE_Widget, opt, painter, self)



