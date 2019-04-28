# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '26/3/19 下午4:09'


from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtGui import QPainter, QPixmap, QImage
from PyQt5.QtCore import pyqtSignal, QThread, QTimer, Qt

from .UI_Welcome import Ui_Main
from Stamper2.Face_Rc.Face_Rc import Rc_Form
from Stamper2.Preview.Preview import Preview_Form
from Stamper2.Wait_Form.Wait_Form import Shadow_Form, Wait_Form
from Stamper2.Hand_Mode.HandMode import Hand_movement_Form
from Stamper2.Serial.Serial import SerialWork

from setting import welcome, drawn_img_path, contract_path
from Common.new_contract import contract_detecting


# 主控制界面
class MainForm(QMainWindow, Ui_Main):
    """
    首页欢迎界面
    QWidget 的 paintEvent() 是空的，所以无法使用QSS,
    """
    close_signal = pyqtSignal()

    def __init__(self, parent=None):
        super(MainForm, self).__init__(parent=parent)

        self.senddata = ''
        self.coord_dict = {}
        self.welcome = welcome
        self.setupUi(self)

        self.Preview = Preview_Form()
        self.Preview.Rc_btn.clicked.connect(self.RcMode)
        self.Preview.Hand_btn.clicked.connect(self.HandMode)
        self.Preview.ensure_btn.clicked.connect(self.SerialMode)
        self.Preview.cancel_btn.clicked.connect(self.show_self)

        self.Hand = Hand_movement_Form(self.Preview)
        self.Hand.ensure_btn.clicked.connect(self.HandData)
        self.Hand.close()

        self.close_signal.connect(self.Show_Preview)

    def show_self(self):
        """
        显示当前页面
        :return:
        """
        try:
            self.Preview.close_self()

            self.show()
        except Exception as e:
            print("Exception:", e)

    def Show_Preview(self):
        """
        显示文件预览页面
        :return:
        """
        # self.Preview.show_Repaint()
        self.Preview.show_self()
        self.hide()

    def SerialMode(self):
        """
        设置串口，并发送指令
        :return:
        """
        self.serialthread = QThread()
        self.serialwork = SerialWork()
        self.serialwork.moveToThread(self.serialthread)
        self.serialthread.started.connect(self.serialwork.init)
        self.serialthread.start()

        # 设置发送数据
        # timer_sleep = QTimer()
        # timer_sleep.setSingleShot(True)
        # timer_sleep.timeout.connect(lambda: self.serialwork.Set_sendData(self.senddata))
        # timer_sleep.start(1000)
        self.serialwork.Set_sendData(self.senddata)
        self.serialwork.writeData()
        print("serial:", self.senddata)


    def RcMode(self):
        """
        自动识别盖章
        :return:
        """
        try:
            if self.Preview.device.isOpened():
                self.Preview.Currentframe_Save(contract_path)
                self.coord_dict = contract_detecting(contract_path, drawn_img_path)
                self.Hand.Set_label_image(drawn_img_path)
                self.Hand.show_self()
            else:
                self.Preview.Open_capture()

                timer_sleep = QTimer()
                timer_sleep.setSingleShot(True)
                timer_sleep.timeout.connect(lambda: self.Preview.Currentframe_Save(contract_path))
                timer_sleep.start(1000)
                self.coord_dict = contract_detecting(contract_path, drawn_img_path)
                # 设置要发送的数据
                self.SetData(self.coord_dict)

                self.Hand.Set_label_image(drawn_img_path)
                self.Hand.show_self()
        except Exception as e:
            print("RcMode:", str(e))

    def SetData(self, coord_info):
        """
        设置要发送的数据
        :param coord_info:
        :return:
        """
        center = coord_info["final_center"]
        region = coord_info["region"]
        # 设置X轴的行程
        X_ = center[0]
        if(len(X_)!=3):
            zero_str = '0'*(3-len(X_))
            X_ = zero_str + str(X_)

        # 目前Y轴2个电机带动纸行走的距离无法确定，先设置默认值0
        Y_ = center[1]
        Y_1_MM = '000'
        Y_2_MM = '000'

        # 设置印章ID
        Seal_id = self.Preview.Seal_type.currentData()
        Seal_id = str(Seal_id)

        self.senddata = X_+Y_1_MM+Y_2_MM+Seal_id
        print("senddata:", self.senddata)

    def HandMode(self):
        """
        手动设置盖章位置
        :return:
        """
        try:
            self.Preview.Currentframe_Save(contract_path)
            self.Preview.Device_Release()
            self.Hand.Set_label_image(contract_path)
            self.Hand.show_self()
        except Exception as e:
            print("HandMode:", str(e))

    def HandData(self, info):
        """
        手动设置盖章位置
        :param info: 位置坐标信息
        :return:
        """
        coord_img = self.Hand.Get_img_info()
        if len(coord_img) != 0:
            self.coord_dict = coord_img
        print("coo_info:", self.coord_dict)
        self.Preview.Device_Release()

        self.timer = QTimer()
        self.timer.setSingleShot(1000)
        self.Set_preview_lab()
        self.Hand.close()
        # self.Preview.repaint()

    def Set_preview_lab(self):
        """
        重新设置预览图片
        """
        pixmap = QPixmap(drawn_img_path)

        self.Preview.preview_lab.setScaledContents(True)
        self.Preview.preview_lab.setPixmap(pixmap)

    def mousePressEvent(self, event):
        """
        重写鼠标点击事件,隐藏主界面
        :param event:
        :return:
        """
        self.close_signal.emit()

    def paintEvent(self, event):
        """
        重新实现绘图事件，背景图
        :return:
        """
        painter = QPainter(self)
        pixmap  = QPixmap(self.welcome)
        # 设置窗口背景图片， 平铺到整个窗口，随着窗口的改变而改变
        painter.drawPixmap(self.rect(), pixmap)
