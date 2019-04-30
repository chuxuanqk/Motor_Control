# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '21/4/19 下午7:07'

import sys
# import cv2


from PyQt5.QtWidgets import QWidget, QStyle, QApplication, QStyleOption, QPushButton
from PyQt5.QtCore import QRect, pyqtSignal, QThread, QObject, QTimer
from PyQt5.QtGui import QPainter, QPixmap, QImage
from PyQt5.QtSerialPort import QSerialPort, QSerialPortInfo

# from .UI_Serial import Ui_Serial
# from Common.utils import Timer
# from setting import face_camera
# from Stamper2.Wait_Form.Wait_Form import Wait_Form


class SerialWork(QObject):
    """
    串口数据处理
    """
    def __init__(self):
        super(SerialWork, self).__init__()

    def init(self):

        self.serianame = ''
        self.com = QSerialPort()
        self.cominfo = QSerialPortInfo()
        self.infos = self.cominfo.availablePorts()
        for info in self.infos:
            print("Name:", info.portName())
            print("vendoridentifier:", info.vendorIdentifier())

            # 串口信息认证
            if info.vendorIdentifier() == 6790:
                self.serianame = info.portName()


        self.com.setPortName(self.serianame)
        self.com.setBaudRate(9600)

        # 打开串口
        if self.com.open(QSerialPort.ReadWrite) == False:
            print("open fault")
            return

        # 设置定时函数
        self.readtimer = QTimer()
        self.readtimer.timeout.connect(self.readData)     # 读信号
        self.readtimer.start(200)                         # 每200ms读一次数据

    def Set_sendData(self, s_data):
        """
        设置要发送的数据
        :param s_data:
        :return:
        """
        self.sendData = bytes(s_data, encoding='utf8')

    def readData(self):
        """
        读数据
        :return:
        """
        revData = ''
        revData = self.com.readAll()
        revData = bytes(revData)
        if revData.__len__() != 0:
            print('%d read:' % len(revData))
            print(revData)

        return len(revData)

    def writeData(self):
        """
        写数据b
        :return:
        """
        try:
            if self.com.isOpen():
                print("send:", self.sendData, len(self.sendData))
                self.com.writeData(self.sendData)
        except Exception as e:
            print("Write:", str(e))

    def SerialClsoe(self):
        """
        关闭串口
        :return:
        """
        if self.com.isOpen():
            self.com.close()


class PyQt_Serial(QWidget):
    """
    串口测试模块
    """
    def __init__(self):
        super(PyQt_Serial, self).__init__()

        self.resize(600, 400)

        self.btn = QPushButton(self)
        self.btn.setObjectName('btn')
        self.btn.setText("Send_Data")
        self.btn.setGeometry(QRect(100, 100, 80, 40))

        self.Start_btn = QPushButton(self)
        self.Start_btn.setObjectName('start')
        self.Start_btn.setText("Start")
        self.Start_btn.setGeometry(QRect(200, 100, 60, 40))

        self.Stop_btn = QPushButton(self)
        self.Stop_btn.setObjectName('stop')
        self.Stop_btn.setText("Stop")
        self.Stop_btn.setGeometry(QRect(300, 100, 60, 40))

        self.serialwork = SerialWork()
        self.serialthread = QThread()
        self.serialwork.moveToThread(self.serialthread)                   # 将耗时操作放到QObjecgt的子类中实现
        self.serialthread.started.connect(self.serialwork.init)

        self.serialwork.Set_sendData('1000000002\r\n')
        self.btn.clicked.connect(self.serialwork.writeData)
        self.Start_btn.clicked.connect(self.Start)
        self.Stop_btn.clicked.connect(self.Stop)

    def Start(self):
        """
        开启定时器,打开串口
        :return:
        """
        self.serialthread.start()

    def Stop(self):
        """
        关闭定时器, 关闭串口
        :return:
        """
        try:
            self.serialthread.quit()
        except Exception as e:
            print(str(e))


# class Serial_Form(QWidget, Ui_Serial):
#     """
#     盖章文件显示及控制
#     """
#
#     cap_signal = pyqtSignal()
#
#     def __init__(self, parent=None):
#         super(Serial_Form, self).__init__()
#
#         self.setupUi(self)
#
#         self.image = QImage()
#         self.playTimer = Timer()
#         self.playTimer.Camer.connect(self.showCamer)
#
#         self.cap_signal.connect(self.cap_shuted)
#
#     def show_self(self):
#         """
#         显示页面
#         :return:
#         """
#         try:
#             self.device = cv2.VideoCapture(face_camera)
#             # 设置摄像头获得的像素大小, CV的宽高和正常显示是反的
#             self.device.set(cv2.CAP_PROP_FRAME_HEIGHT, 1960)
#             self.device.set(cv2.CAP_PROP_FRAME_WIDTH, 1372)
#
#             self.playTimer.start()
#         except:
#             self.cap_signal.emit()
#
#         self.show()
#
#     def close_self(self):
#         """
#         关闭当前界面，返回欢迎界面
#         :return:
#         """
#         try:
#             self.playTimer.stop()
#             self.device.release()
#             # 发送退出信号
#             self.close_signal.emit()
#         except Exception as e:
#             print("返回页面:{}".format(str(e)))
#
#     def Device_Release(self):
#         """
#         识别成功后，释放资源
#         :return:
#         """
#         try:
#             self.wait.close_self()
#             self.shadow.close()
#             self.device.release()
#             self.timer.stop()
#         except:
#             print("重复释放资源")
#
#     def cap_shuted(self):
#         """
#         摄像头未打开处理
#         :return:
#         """
#         cap_message = Wait_Form(self)
#         cap_message.raise_()
#         cap_message.wait_lab.setText("摄像头未打开")
#         cap_message.open()         # 修改，.exec_()
#         self.timer.singleShot(2000, cap_message.close_self)
#
#     def showCamer(self):
#         """
#         # 读摄像头
#         :return:
#         """
#         if self.device.isOpened():
#             ret, frame = self.device.read()
#             frame.resize()
#             cv2.flip(frame, 90)
#
#         else:
#             ret = False
#
#         # 当关掉摄像头的信号发出后，还有5个左右的定时任务在执行，所以会抛出异常
#         try:
#             """
#             # 提取中心图片178*2
#             r = 178     # 设置图片
#             height, width, bytesPerComponent = frame.shape
#             center = (int(height / 2), int(width / 2))
#             left_angle = (center[0] - r, center[1] - r)
#             right_angle = (center[0] + r, center[1] + r)
#             frame2 = frame[left_angle[0]:right_angle[0], left_angle[1]:right_angle[1]]
#             """
#
#             height, width, bytesPerComponent = frame.shape
#             frame1 = frame.copy()
#             height1, width1, bytesPerComponent1 = frame1.shape
#             bytesPerLine = bytesPerComponent * width1
#
#             # 变换彩色空间顺序
#             cv2.cvtColor(frame1, cv2.COLOR_BGR2RGB, frame1)
#
#             # 转为QImage对象
#             self.image = QImage(frame1.data, width1, height1, bytesPerLine, QImage.Format_RGB888)
#             self.preview_lab.setPixmap(QPixmap.fromImage(self.image))
#             # self.face_label1.raise_()
#             self.image = frame
#
#             self.update()
#         except Exception as e:
#             print("退出")
#
#     def paintEvent(self, event):
#         """
#         重写paintEvent 否则不能使用样式表定义外观
#         :param evt:
#         :return:
#         """
#         opt = QStyleOption()
#         opt.initFrom(self)
#         painter = QPainter(self)
#         # 反锯齿
#         painter.setRenderHint(QPainter.Antialiasing)
#         self.style().drawPrimitive(QStyle.PE_Widget, opt, painter, self)




if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = PyQt_Serial()
    win.show()
    sys.exit(app.exec_())





