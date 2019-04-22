# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '26/3/19 下午4:09'


from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtGui import QPainter, QPixmap
from PyQt5.QtCore import pyqtSignal

from .UI_Welcome import Ui_Main
from Stamper2.Face_Rc.Face_Rc import Rc_Form
from setting import welcome


# 主控制界面
class MainForm(QMainWindow, Ui_Main):
    """
    首页欢迎界面
    QWidget 的 paintEvent() 是空的，所以无法使用QSS,
    """
    close_signal = pyqtSignal()

    def __init__(self, parent=None):
        super(MainForm, self).__init__(parent=parent)

        self.welcome = welcome
        self.setupUi(self)

        self.Face_Rc = Rc_Form(self)

        self.close_signal.connect(self.Show_Face_Rc)
        self.Face_Rc.close_signal.connect(self.show_self)
        self.Face_Rc.success_signal.connect(self.Display)

    def show_self(self):
        """
        显示当前页面
        :return:
        """
        try:
            self.Face_Rc.close()
            self.show()
        except Exception as e:
            print("Exception:", e)

    def Display(self, name):
        """
        显示下一个界面
        :param name:
        :return:
        """
        print(name)
        self.Face_Rc.Device_Release()
        self.show_self()

    def mousePressEvent(self, event):
        """
        重写鼠标点击事件,隐藏主界面
        :param event:
        :return:
        """
        self.close_signal.emit()
        # 主控制界面不允许close
        # self.close()

    def Show_Face_Rc(self):
        """
        显示人脸页面
        :return:
        """
        self.Face_Rc.show_self()

    def paintEvent(self, event):
        """
        重新实现绘图事件，背景图
        :return:
        """

        painter = QPainter(self)
        pixmap  = QPixmap(self.welcome)
        # 设置窗口背景图片， 平铺到整个窗口，随着窗口的改变而改变
        painter.drawPixmap(self.rect(), pixmap)
