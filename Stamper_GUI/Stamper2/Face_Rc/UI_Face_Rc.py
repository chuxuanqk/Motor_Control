# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '26/3/19 下午4:54'


from PyQt5.QtCore import Qt, QRect, QMetaObject, QCoreApplication
from PyQt5.QtWidgets import QPushButton, QLabel, QLineEdit
from Common.utils import CommonHelper, Face_Label

from setting import Face_QSS


class Ui_Face(object):
    """
    人脸识别界面
    """
    def setupUi(self, Form):
        Form.move(0, 0)
        Form.resize(1024, 768)
        Form.setObjectName('face')
        Form.setWindowFlags(Qt.FramelessWindowHint)

        # 加载QSS样式
        self.styleFile = Face_QSS
        style = CommonHelper.readQss(self.styleFile)
        Form.setStyleSheet(style)

        self.back_btn = QPushButton(Form)
        self.back_btn.setObjectName("back_btn")
        self.back_btn.setGeometry(QRect(21, 26, 27, 20))

        self.start_btn = QPushButton(Form)
        self.start_btn.setObjectName("start_btn")
        self.start_btn.setGeometry(QRect(390, 636, 259, 80))
        self.start_btn.setText("开始识别")

        self.face_label = QLabel(Form)
        self.face_label.setObjectName("face_label")
        self.face_label.setGeometry(QRect(412, 0, 207, 194))

        self.scan_label = QLabel(Form)
        self.scan_label.setObjectName("scan_label")
        self.scan_label.setGeometry(QRect(273, 170, 460, 460))

        # self.face_label1 = QtWidgets.QLabel(Form)
        self.face_label1 = Face_Label(Form)
        self.face_label1.setObjectName("face_label1")
        # self.face_label1.setGeometry(QtCore.QRect(320, 220, 360, 360))
        self.face_label1.move(320, 220)

        self.input_edit = QLineEdit(Form)
        self.input_edit.setObjectName("input_edit")
        self.input_edit.setGeometry(QRect(405, 612, 200, 40))

        self.input_edit.setAlignment(Qt.AlignCenter)
        self.input_edit.hide()

        self.retranslateUi(Form)
        QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QCoreApplication.translate
