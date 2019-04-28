# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '23/4/19 下午2:00'


import cv2
from PyQt5.QtWidgets import QDialog, QApplication
from PyQt5.QtCore import pyqtSignal, QPoint
from PyQt5.QtGui import QCursor

from .UI_HnadMode import Ui_hand_movement
from setting import contract_path, drawn_img_path
from Common.new_contract import offset_of_image_and_a4


class Hand_movement_Form(QDialog, Ui_hand_movement):
    """
    手动盖章功能
    """

    img_signal = pyqtSignal(dict)
    mouse_signal = pyqtSignal(dict)

    def __init__(self, parent=None):
        super(Hand_movement_Form, self).__init__(parent=parent)

        self.setupUi(self)

        # 位置坐标信息
        self.img_info = {}
        self.label_imag = ''
        self.postion = {'x': 0, 'y': 0}

        self.mouse_signal.connect(self.stamper_coordinate)
        self.cancel_btn.clicked.connect(self.cancel_btn_released)

    def show_self(self):
        """
        显示当前页面
        :param Form:
        :return:
        """
        # 设置label图片
        self.label.setStyleSheet("#show_lab{border-image:url("+self.label_imag+");}")
        self.show()

    def Set_label_image(self, path):
        """
        设置label图片
        :return:
        """
        self.label_imag = path

    def cancel_btn_released(self):
        """
        返回印章预览页面
        :return:
        """
        self.close()

    def ensure_btn_released(self):
        """
        发送盖章图片，盖章坐标
        :return:
        """
        self.img_signal.emit(self.img_info)
        self.close()

    def Get_img_info(self):
        """
        获取img_info
        :return:
        """
        return self.img_info

    def stamper_coordinate(self, pos):
        """
        显示重新绘制的盖章图片
        :param pos:
        :return:
        """
        # x = int(pos['x']*(1372/420))
        # y = int(pos['y']*(1960/600))
        x = int(pos['x'])
        y = int(pos['y'])
        print("x:{}, y:{}".format(x, y))

        pos_tuple = tuple((x, y))
        center = self.draw_img(pos_tuple)

        drawn_img = cv2.imread(drawn_img_path)
        real_center, region_ = offset_of_image_and_a4(drawn_img, center)
        print("real_center:", real_center)

        self.img_info["final_center"] = real_center
        self.img_info["region"] = region_

        self.label.setStyleSheet("#show_lab{border-image:url("+drawn_img_path+");}")
        self.repaint()

    def draw_img(self, center):
        """
        绘制盖章图
        return:
        """
        center1 = [0, 0]
        img = cv2.imread(contract_path)
        if img.shape[0] == 0 or img.shape[1] == 0:
            raise IOError("Please put image!")
        else:
            center = (int(center[0] / (420 / img.shape[1])), int(center[1] / (600 / img.shape[0])))

        cv2.circle(img, center, radius=250, color=(255, 0, 0), thickness=5)
        cv2.imwrite(drawn_img_path, img)
        center = center # + tuple(center1)

        return center

    def mouseReleaseEvent(self, event):
        """
        重写鼠标点击释放事件
        :param event:
        :return:
        """
        mouse_x = QCursor.pos().x()
        mouse_y = QCursor.pos().y()

        active = QApplication.widgetAt(mouse_x, mouse_y)
        global_pos = QPoint(active.mapToGlobal(QPoint(0, 0)))
        x = global_pos.x()
        y = global_pos.y()

        relative_pos_x = mouse_x - x
        relative_pos_y = mouse_y - y
        self.postion['x'] = relative_pos_x
        self.postion['y'] = relative_pos_y

        print("postion:", self.postion)

        if ((mouse_x > 326) and (mouse_x<746) and (mouse_y>163) and (mouse_y<763)):
            # 发送点击坐标的信号
            self.mouse_signal.emit(self.postion)




