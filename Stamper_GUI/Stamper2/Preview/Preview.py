# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '22/4/19 下午5:41'


import cv2
from PyQt5.QtWidgets import QWidget, QStyle, QStyleOption
from PyQt5.QtCore import QRect, pyqtSignal, QThread, QObject, QTimer
from PyQt5.QtGui import QPainter, QPixmap, QImage
from math import *
from Stamper2.Wait_Form.Wait_Form import Shadow_Form, Wait_Form
from .UI_Preview import Ui_preview
from Common.utils import Timer
from Common.new_contract import get_frame

from setting import camera, contract_path


class Preview_Form(QWidget, Ui_preview):
    """
    盖章文件预览模块
    """
    show_stamper_signal = pyqtSignal(QWidget)
    show_hand_signal = pyqtSignal()
    back_signal = pyqtSignal()
    close_signal = pyqtSignal()
    cap_signal = pyqtSignal()

    def __init__(self, parent=None):
        super(Preview_Form, self).__init__(parent=parent)
        self.setupUi(self)

        self.image = QImage()
        self.playTimer = Timer()

        self.preview_lab.setStyleSheet("#preview_lab{border-image:url(" + contract_path + ");}")

        self.playTimer.Camer.connect(self.showCamer)
        self.cap_signal.connect(self.cap_shuted)

        self.Seal_type.activated[str].connect(self.OnActivated)   # 当改变QComboBox的信号

    def OnActivated(self, text):
        """
        获取印章编号信息
        :return:
        """
        pass
        # print("Seal:", text)
        # print("Seal_value:", self.Seal_type.currentData())

    def show_Repaint(self):
        """
        :return:
        """
        self.preview_lab.setStyleSheet("#preview_lab{border-image:url(" + contract_path + ");}")
        self.show()

    def show_self(self):
        """
        显示页面
        :return:
        """
        self.Open_capture()
        self.show()

    def Open_capture(self):
        try:
            self.device = cv2.VideoCapture(camera)
            # 设置摄像头获得的像素大小, CV的宽高和正常显示是反的
            self.device.set(cv2.CAP_PROP_FRAME_HEIGHT, 2048)
            self.device.set(cv2.CAP_PROP_FRAME_WIDTH, 1536)

            self.playTimer.start()
        except:
            self.cap_signal.emit()

    def close_self(self):
        """
        关闭当前界面，返回欢迎界面
        :return:
        """
        try:
            self.playTimer.stop()
            self.device.release()
            # 发送退出信号
            self.close_signal.emit()
            self.close()
        except Exception as e:
            print("返回页面:{}".format(str(e)))

    def Device_Release(self):
        """
        识别成功后，释放资源
        :return:
        """
        try:
            # self.wait.close_self()
            # self.shadow.close()
            self.playTimer.stop()
            self.device.release()
            # self.timer.stop()

        except Exception as e:
            print("重复释放资源:", e)

    def cap_shuted(self):
        """
        摄像头未打开处理
        :return:
        """
        cap_message = Wait_Form(self)
        cap_message.raise_()
        cap_message.wait_lab.setText("摄像头未打开")
        cap_message.open()         # 修改，.exec_()
        self.timer.singleShot(2000, cap_message.close_self)

    def showCamer(self):
        """
        # 读摄像头
        :return:
        """
        if self.device.isOpened():
            ret, frame = self.device.read()
            # print("frame_shape:", frame.shape)
            ########################################
            degree = 90
            height, width = frame.shape[:2]
            height_rotating = int(width * fabs(sin(radians(degree))) + height * fabs(cos(radians(degree))))
            width_rotating = int(height * fabs(sin(radians(degree))) + width * fabs(cos(radians(degree))))
            Rotation = cv2.getRotationMatrix2D((width / 2, height / 2), degree, 1)
            Rotation[0, 2] += (width_rotating - width) / 2
            Rotation[1, 2] += (height_rotating - height) / 2
            frame = cv2.warpAffine(frame, Rotation, (width_rotating, height_rotating),
                                         borderValue=(255, 255, 255))

            #########################################
            # frame = get_frame(frame, 420, 600)
            # cv2.flip(frame, -1)

            # 当关掉摄像头的信号发出后，还有5个左右的定时任务在执行，所以会抛出异常
            try:
                height, width, bytesPerComponent = frame.shape
                frame1 = frame.copy()
                height1, width1, bytesPerComponent1 = frame1.shape
                bytesPerLine = bytesPerComponent * width1

                # 变换彩色空间顺序
                cv2.cvtColor(frame1, cv2.COLOR_BGR2RGB, frame1)

                # 转为QImage对象
                self.image = QImage(frame1.data, width1, height1, bytesPerLine, QImage.Format_RGB888)
                self.preview_lab.setScaledContents(True)
                self.preview_lab.setPixmap(QPixmap.fromImage(self.image))
                self.image_frame = frame

                self.update()
            except Exception as e:
                print("退出:", e)
        else:
            ret = False

    def Currentframe_Save(self, path):
        """
        保存当前frame图片
        :return:
        """
        cv2.imwrite(path, self.image_frame)

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



