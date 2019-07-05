# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '26/3/19 下午5:22'

from PyQt5.QtWidgets import QDialog, QStyleOption, QStyle
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtGui import QPainter

from .UI_Wait_Form import Ui_shadow, Ui_Wait, Ui_seal


class Shadow_Form(QDialog, Ui_shadow):
    """阴影页面"""

    def __init__(self, parent=None):
        super(Shadow_Form, self).__init__(parent=parent)

        self.setupUi(self)
        self.hide()


class Wait_Form(QDialog, Ui_Wait):
    destroy_signal = pyqtSignal()

    def __init__(self, parent=None):
        super(Wait_Form, self).__init__(parent=parent)
        self.setupUi(self)
        self.hide()

    def show_self(self):
        self.open()

    def close_self(self):
        """
        退出，销毁
        :return:
        """
        self.destroy_signal.emit()
        self.close()

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

    def mousePressEvent(self, event):
        """
        重写鼠标点击事件,隐藏主界面
        :param event:
        :return:
        """
        pass


class Seal_Form(QDialog, Ui_seal):
    """
    印章选择
    """
    def __init__(self, parent=None):
        super(Seal_Form, self).__init__(parent=parent)
        self.setupUi(self)
        self.hide()

    def show_self(self):
        self.show()

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





