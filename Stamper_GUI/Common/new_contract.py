# -*- coding: UTF-8 -*-

import os
import cv2
import time
from math import *
import logging
import numpy as np
import pytesseract
from .config import Config

class RaiseError:

    def logger(self):
        logging.basicConfig(level=logging.DEBUG,
                            filename='recognition.log',
                            filemode='a',
                            datefmt='%Y/%m/%d %H:%M:%S',
                            format='%(asctime)s - %(name)s - %(levelname)s - %(lineno)d - %(module)s - %(message)s')

        logger = logging.getLogger(__name__)

        return logger

    def __repr__(self):
        return 'RaiseError'


def rotate_image(frame1):
    """
    对拍照获取的合同图片进行旋转
    param frame: 拍照获得的图片的矩阵信息
    param degree: 旋转角度
    return: 旋转之后图片的矩阵信息
    """
    degree = Config.DEGREE
    frame = frame1
    height, width = frame.shape[:2]
    height_rotating = int(width * fabs(sin(radians(degree))) + height * fabs(cos(radians(degree))))
    width_rotating = int(height * fabs(sin(radians(degree))) + width * fabs(cos(radians(degree))))
    Rotation = cv2.getRotationMatrix2D((width / 2, height / 2), degree, 1)
    Rotation[0, 2] += (width_rotating - width) / 2
    Rotation[1, 2] += (height_rotating - height) / 2
    imgRotation = cv2.warpAffine(frame, Rotation, (width_rotating, height_rotating), borderValue=(255, 255, 255))

    # cv2.imwrite('/home/devin/Documents/qianhai_devin/stamp_machinery/stamp01/contract_r.jpg', imgRotation)

    return imgRotation


class Correct_Image:

    def __init__(self, image):
        self.image = image

    def degree_trans(self, theta):
        res = theta / np.pi * 180
        return res

    def rotate_image(self, degree):
        h, w = self.image.shape[: 2]
        RotateMatrix = cv2.getRotationMatrix2D((w/2.0, h/2.0), degree, 1)
        rotate = cv2.warpAffine(self.image, RotateMatrix, (w, h), borderValue=(255, 255, 255))
        return rotate

    def calc_degree(self):
        mid_image = cv2.cvtColor(self.image, cv2.COLOR_BGR2GRAY)
        dst_image = cv2.Canny(mid_image, 50, 200, 3)
        line_image = self.image.copy()
        lines = cv2.HoughLines(dst_image, 1, np.pi / 180, 200)

        sum = 0
        for i in range(len(lines)):
            for rho, theta in lines[i]:
                a = np.cos(theta)
                b = np.sin(theta)
                x0 = a * rho
                y0 = b * rho
                x1 = int(round(x0 + 1000 * (-b)))
                y1 = int(round(y0 + 1000 * a))
                x2 = int(round(x0 - 1000 * (-b)))
                y2 = int(round(y0 - 1000 * a))

                sum += theta

            cv2.line(line_image, (x1, y1), (x2, y2), (0, 0, 255), 1, cv2.LINE_AA)
        average = sum / len(lines)
        angle = self.degree_trans(average) - 90
        return angle

class Recognition:

    def __init__(self, img, save_path):
        self.img = img
        self.save_path = save_path
        self.key_lsit = Config.KEY_LIST

    @staticmethod
    def pick_out_text(lines, texts, top, left, width, height):
        """
        Pick out text from Ocr result
        param lines: the line number of each word
        param texts: each word
        param top: the top distance of each word
        param left: the left distance of each word
        param width: the width of each word
        param height: the height of each word
        return: The lists of all information about words
        """
        m = 0
        tops = []
        text = []
        lefts = []
        widths = []
        heights = []
        temporary_top = []
        temporary_left = []
        temporary_lines = []
        temporary_texts = []
        temporary_width = []
        temporary_height = []
        final_top_list = []
        final_left_list = []
        final_text_list = []
        final_width_list = []
        final_height_list = []
        if len(lines) == len(texts):
            for i in range(len(lines)):
                if lines[i] == 0:
                    top1 = top[m:i]
                    left1 = left[m:i]
                    list1 = lines[m:i]
                    text1 = texts[m:i]
                    width1 = width[m:i]
                    height1 = height[m:i]
                    m = i
                    temporary_top.append(top1)
                    temporary_left.append(left1)
                    temporary_lines.append(list1)
                    temporary_texts.append(text1)
                    temporary_width.append(width1)
                    temporary_height.append(height1)
            for x in range(len(temporary_lines)):
                if len(temporary_lines[x]) <= 2:
                    pass
                else:
                    list_top = temporary_top[x]
                    list_num = temporary_lines[x]
                    list_left = temporary_left[x]
                    list_text = temporary_texts[x]
                    list_width = temporary_width[x]
                    list_height = temporary_height[x]
                    for j in range(1, max(list_num) + 1):
                        line_num = j
                        line_text = []
                        line_top = []
                        line_left = []
                        line_width = []
                        line_height = []
                        for i in range(1, len(list_num)):
                            if list_num[i] == line_num:
                                line_top.append(list_top[i])
                                line_left.append(list_left[i])
                                line_text.append(list_text[i])
                                line_width.append(list_width[i])
                                line_height.append(list_height[i])
                            else:
                                pass
                        if len(line_text) <= 20:
                            final_top_list.append(line_top)
                            final_left_list.append(line_left)
                            final_text_list.append(line_text)
                            final_width_list.append(line_width)
                            final_height_list.append(line_height)
                        else:
                            pass
            tops = [y for x in final_top_list for y in x]
            text = [y for x in final_text_list for y in x]
            lefts = [y for x in final_left_list for y in x]
            widths = [y for x in final_width_list for y in x]
            heights = [y for x in final_height_list for y in x]
        else:
            print('Error')
        return text, tops, lefts, widths, heights

    def contract_recognition(self):
        """
        Process image, and get all information of text
        param img: image frame
        return: dict of processing result
        """
        img_gray = cv2.cvtColor(self.img, cv2.COLOR_RGB2GRAY)
        result = pytesseract.image_to_data(img_gray, lang='chi_sim', config='--psm 3',
                                           nice=0, output_type=pytesseract.Output.DICT)

        return result

    def get_key_words(self):
        """
        Pick out the words we need
        param img: image frame
        return: indexes: the index of every needed word
                strings: the string type of all words
                location: the dict of words' location
        """
        strings = ''
        indexes = []
        result = self.contract_recognition()
        top = list(result['top'])
        left = list(result['left'])
        texts = list(result['text'])
        width = list(result['width'])
        height = list(result['height'])
        lines = list(result['line_num'])
        text, tops, lefts, widths, heights = self.pick_out_text(lines, texts, top,
                                                                left, width, height)
        location = {'Top': tops, 'Left': lefts, 'Width': widths, 'Height': heights}
        for i in range(len(text)):
            if len(text[i]) == 1:
                string = text[i]
                strings = strings + string
                m = i
                indexes.append(m)
            else:
                pass

        return indexes, strings, location

    def locate_key_words(self):
        """
        Locate the location of every key word
        param img: image frame
        param key_list: the list of key words
        return: index_str: the index of strings
                indexes: the index of every needed word
                location: the dict of words' location
        """
        index_str = []
        key_list = self.key_lsit
        indexes, strings, location = self.get_key_words()
        for key in key_list:
            index_keys = strings.find(key)
            if index_keys >= 0:
                index_str.append(index_keys)
            else:
                pass

        return index_str, indexes, location

    def calculate_center(self):
        """
        Calculate circle center of the target
        param img: image frame
        param key_list: list of key words
        return: the coordinate of center
        """
        h, w, _ = self.img.shape
        index_str, indexes, location = self.locate_key_words()
        index_list = min(index_str)
        temp_index = indexes[index_list]
        x1 = location['Left'][temp_index]
        y1 = location['Top'][temp_index]
        x2 = x1 + location['Width'][temp_index]
        y2 = y1 + location['Height'][temp_index]
        center = (x2 + 5*location['Width'][temp_index], y2)
        ratio = 0.0
        if w > 0 and h > 0:
            ratio = (center[0]/w, center[1]/h)
        else:
            pass
        cv2.circle(self.img, center, 150, (255, 0, 0), thickness=5)
        cv2.imwrite(self.save_path, self.img)
        return center, ratio

    def __repr__(self):
        return 'Recognition'


def offset_of_image_and_a4(img, center):
    ih = img.shape[0]
    iw = img.shape[1]
    a4 = Config.A4
    deltas = [Config.DELTAS_HEIGHT, Config.DELTAS_WIDTH]
    isize = Config.IMAGE_REAL_SIZE
    dis_origin = Config.DISTANCE_TO_ORIGIN
    y_ = int((center[1] / ih) * isize[0] + deltas[0])
    x = int((center[0] / iw) * isize[1] + deltas[1])
    y = a4[0] - y_
    real_center = (int(dis_origin - x), y)
    region_width = Config.REGION_WIDTH
    region_ = 0
    if (y % region_width) > 0:
        region_ = int(y / region_width) + 1
    return real_center, region_


def draw_img(center, contract_path, draw_image_path):
    """
    绘制盖章图
    return:
    """
    x_click = Config.CLICK_WINDOW[0]
    y_click = Config.CLICK_WINDOW[1]
    img = cv2.imread(contract_path)
    if img.shape[0] == 0 or img.shape[1] == 0:
        raise IOError("Please put image!")
    else:
        center = (int(center[0] / (x_click / img.shape[1])), int(center[1] / (y_click / img.shape[0])))
    cv2.circle(img, center, radius=250, color=(255, 0, 0), thickness=5)
    cv2.imwrite(draw_image_path, img)
    center = center

    return center


def get_frame(frame, width, height):
    frame1 = cv2.resize(frame, (width, height))
    return frame1


def contract_detacting(path, save_path):
    img = cv2.imread(path)
    degree = Correct_Image(img).calc_degree()
    rotate = Correct_Image(img).rotate_image(degree)
    center, _ = Recognition(img, save_path).calculate_center()
    final_center, region = offset_of_image_and_a4(rotate, center)
    coordinate_and_region = {"final_center": final_center, "region": region}
    return coordinate_and_region


if __name__ == "__main__":
    camera = 2
    # path = "/home/devin/Documents/qianhai_devin/stamp_machinery/ocr/jpg/loan/3.jpg"
    # save_path = "/home/devin/Documents/qianhai_devin/stamp_machinery/stamp01/save.jpg"
    
    # location = contract_detacting(path, save_path)
    # print("location:", location)
