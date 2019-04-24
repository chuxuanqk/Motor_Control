# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '26/3/19 下午4:53'

import cv2
import numpy as np
import face_recognition

from PyQt5.QtWidgets import QWidget, QStyleOption, QStyle
from PyQt5.QtGui import QPainter, QPixmap, QImage
from PyQt5.QtCore import pyqtSignal, QTimer

from .UI_Face_Rc import Ui_Face
from Stamper2.Wait_Form.Wait_Form import Shadow_Form, Wait_Form
from Common.utils import Timer
from setting import face_camera
from Common.date_operation import select_user_info


def face_recog(known_face_encodings, known_face_names, frame):
    """
    :param known_face_encodings:
    :param known_face_names:
    :param frame:
    :return: 验证未通过：Uknown,  通过返回name
    """
    # img_face_encoding = face_recognition.face_encodings(img)[0]

    face_locations = []
    # face_encodings = []
    face_names = []
    name = 'Uknown'
    process_this_frame = True
    lis = []
    limite = ''

    small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)
    rgb_small_frame = small_frame[:, :, ::-1]

    if process_this_frame:
        face_locations = face_recognition.face_locations(rgb_small_frame)
        face_encodings = face_recognition.face_encodings(rgb_small_frame, face_locations)

        face_names = []
        for face_encoding in face_encodings:
            matches = face_recognition.compare_faces(known_face_encodings, face_encoding, tolerance=0.39)
            limite = 'Unauthorised'

            # 获取匹配成功的名字
            if True in matches:
                limite = 'Authorised'
                first_match_index = matches.index(True)
                name = known_face_names[first_match_index]

            face_names.append(name)
        process_this_frame = False

    return name


def face_login(frame):
    """
    人脸认证
    :param frame:
    :return:
    """
    cv2.cvtColor(frame, cv2.COLOR_BGR2RGB, frame)

    # 读取数据库中保存的人脸信息，进行信息匹配
    user_info = select_user_info()
    known_face_encodings = [i['user_face_info'] for i in user_info]
    known_face_names = [i['user_name'] for i in user_info]
    known_face_encodings = [np.array(eval(','.join(i.split()))) for i in known_face_encodings]
    # known_face_encodings, known_face_names = load_img()
    name = face_recog(known_face_encodings, known_face_names, frame)

    return name


class Rc_Form(QWidget, Ui_Face):
    """
    人脸识别页面
    """
    cap_signal = pyqtSignal()
    success_signal = pyqtSignal(str)
    close_signal = pyqtSignal()

    def __init__(self, parent=None):
        super(Rc_Form, self).__init__(parent=parent)

        self.setupUi(self)

        self.shadow = Shadow_Form(self)
        self.wait = Wait_Form(self)

        self.timer = QTimer(self)
        self.image = QImage()

        self.playTimer = Timer()
        self.playTimer.Camer.connect(self.showCamer)

        self.back_btn.clicked.connect(self.close_self)
        self.start_btn.clicked.connect(self.face_login)

        self.cap_signal.connect(self.cap_shuted)
        # self.success_signal.connect(self.show_home)

    def show_self(self):
        """
        欢迎页面跳转人脸识别页面
        :return:
        """
        try:
            self.device = cv2.VideoCapture(face_camera)
            # 设置摄像头获得的像素大小
            self.device.set(cv2.CAP_PROP_FRAME_HEIGHT, 450)
            self.device.set(cv2.CAP_PROP_FRAME_WIDTH, 450)
            self.playTimer.start()
        except:
            self.cap_signal.emit()

        self.show()

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
            # self.close()
        except Exception as e:
            print("返回页面:{}".format(str(e)))

    def face_login(self):
        """
        人脸识别模块
        :return:
        """
        image_save = self.image

        self.playTimer.stop()
        self.timer.singleShot(1000, lambda: self.face_rc(image_save))

        self.wait.wait_lab.setText("正在验证中....")
        self.shadow.show()
        self.wait.show_self()

    def face_rc(self, image):
        """
        人脸验证
        :return:
        """
        try:
            image_save = image
            # cv2.imwrite('./aa.png', image_save)
            # 1.人脸识别模块
            name = face_login(image_save)

            if name != 'Uknown':
                # 发送成功的信号，并传递登录者
                self.success_signal.emit(name)
            else:
                self.playTimer.start()
                # print("识别失败：", name)
                self.wait.wait_lab.setText("身份验证未通过请\r\n重新验证")
                self.timer.singleShot(2000, self.wait.close_self)
                self.timer.singleShot(2100, self.shadow.close)
        except Exception as e:
            self.playTimer.start()
            self.wait.wait_lab.setText("识别超时\r\n请重新验证")
            self.timer.singleShot(2000, self.wait.close_self)
            self.timer.singleShot(2100, self.shadow.close)

    def Device_Release(self):
        """
        识别成功后，释放资源
        :return:
        """
        try:
            self.wait.close_self()
            self.shadow.close()
            self.device.release()
            self.timer.stop()
        except:
            print("重复释放资源")

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
            cv2.flip(frame, 90)
        else:
            ret = False

        # 当关掉摄像头的信号发出后，还有5个左右的定时任务在执行，所以会抛出异常
        try:
            r = 178
            height, width, bytesPerComponent = frame.shape
            center = (int(height / 2), int(width / 2))
            left_angle = (center[0] - r, center[1] - r)
            right_angle = (center[0] + r, center[1] + r)
            frame2 = frame[left_angle[0]:right_angle[0], left_angle[1]:right_angle[1]]
            frame1 = frame2.copy()
            height1, width1, bytesPerComponent1 = frame1.shape
            bytesPerLine = bytesPerComponent * width1

            # 变换彩色空间顺序
            cv2.cvtColor(frame1, cv2.COLOR_BGR2RGB, frame1)

            print("width, height:", width1, height1)

            # 转为QImage对象
            self.image = QImage(frame1.data, width1, height1, bytesPerLine, QImage.Format_RGB888)
            self.face_label1.setPixmap(QPixmap.fromImage(self.image))
            # self.face_label1.raise_()
            self.image = frame

            self.update()
        except Exception as e:
            print("退出123")

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


