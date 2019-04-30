# -*- coding: UTF-8 -*-
"""
智能盖章机：
实现功能：通过相机对放在入纸托盘中的合同进行拍照，对照片作降噪矫正处理后，进行Ocr文字识别
        从识别的文本信息中挑选出特定的目标关键字，并获取关键字的位置信息，宽高信息。将位
        置信息转化到实际物理世界中A4纸上面的相对坐标，再将位置信息返回。
RaiseError: 该类主要用于识别模块的错误捕捉，并将错误报告按照给定的格式写入到log日志文件
            便于调试过程使用。
CorrectImage: 该类主要解决放入合同时，有一定的倾斜，导致拍照后的图片中文字也以倾斜的样式
              呈现，为了不影响，识别的精度，在每一次识别前，对图片作矫正是很有必要的。
Recognition: 识别主类，实现对图片文本识别转化，对识别后的文本信息进行二次过滤，再从过滤
             后的文本中找到目标关键字，并定位关键字在图片中的位置坐标，将位置坐标作一定
             修正后传出，便于后续盖章动作使用。
全局函数：
    offset_of_image_and_a4：图片上坐标和对应的A4纸合同上的相应位置进行转换，得到物理
        世界的真实坐标，传给盖章抓手；
    draw_img：对定位到的盖章位置进行可视化显示，以圆形代表章，圆形为修正后的盖章位置坐标；
    get_frame：对摄像头获取窗口的分辨率进行固化设置；
    contract_detecting：实现了对识别过程接口函数的封装，便于外部调用；
    image_saver：对最终要盖章的图片进行记录保存便于后期校对核查。
"""

import cv2
import time
from math import *
import logging
import numpy as np
import pytesseract
from .config import Config


class RaiseError:
    """
    Log日志文件，将识别模块的错误，以及警告日志，全部写入到recognition.log文件中
    """

    def logger(self):
        """
        定义logger函数，明确错误等级，写入文件的名称，时间，以及写入日至的内容
        """
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
    rotation = cv2.getRotationMatrix2D((width / 2, height / 2), degree, 1)
    rotation[0, 2] += (width_rotating - width) / 2
    rotation[1, 2] += (height_rotating - height) / 2
    img_rotation = cv2.warpAffine(frame, rotation, (width_rotating, height_rotating), borderValue=(255, 255, 255))

    return img_rotation


class CorrectImage:
    """
    用于矫正拍照后图片上文字倾斜
    """

    def __init__(self, image):
        self.image = image

    def degree_trans(self, theta):
        """
        角度转换函数
        param theta: 角度数
        return: 转换后的角度
        """
        res = theta / np.pi * 180
        return res

    def rotate_image(self, degree):
        """
        对图片内倾斜的文字内容，进行倾斜矫正
        param degree: 倾斜的角度
        return:
        """
        h, w = self.image.shape[: 2]
        rotate_matrix = cv2.getRotationMatrix2D((w/2.0, h/2.0), degree, 1)
        rotate = cv2.warpAffine(self.image, rotate_matrix, (w, h), borderValue=(255, 255, 255))
        return rotate

    def calc_degree(self):
        """
        计算文字倾斜的角度数
        return: 倾斜的角度
        """
        mid_image = cv2.cvtColor(self.image, cv2.COLOR_BGR2GRAY)
        dst_image = cv2.Canny(mid_image, 50, 200, 3)
        line_image = self.image.copy()
        lines = cv2.HoughLines(dst_image, 1, np.pi / 180, 200)

        sum = 0
        x1 = 0
        x2 = 0
        y1 = 0
        y2 = 0
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
        从OCR识别结果中，挑选出目标文本
        param lines: 每个字所对应的行号
        param texts: 单个文字
        param top: 每个文字的top距离
        param left: 每个文字的left距离
        param width: 每个文字的宽度
        param height: 每个文字的高度
        return: 包含以上全部文字信息的lists
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
        处理并识别图片，获取文本相关的所有信息
        param img: 图片矩阵
        return: 识别后的结果放入字典中
        """
        img_gray = cv2.cvtColor(self.img, cv2.COLOR_RGB2GRAY)
        result = pytesseract.image_to_data(img_gray, lang='chi_sim', config='--psm 3',
                                           nice=0, output_type=pytesseract.Output.DICT)

        return result

    def get_key_words(self):
        """
        挑选出所需要的文本
        param img: 图片像素矩阵
        return: indexes: 每一个所需的文字的索引
                strings: 所有文字的字符串集
                location: 记录文字对应在图片中的位置信息
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
        定位每一个文字的位置信息
        param img: 图片像素矩阵
        param key_list: 包含关键字的数组
        return: index_str: 文字字符串的索引
                indexes: 每一个所需的索引
                location: 记录文字对应在图片中的位置信息
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
        计算盖章位置圆形的圆心坐标
        param img: 图片像素矩阵
        param key_list: 包含关键字的数组
        return: 盖章位置圆心坐标
        """
        h, w, _ = self.img.shape
        index_str, indexes, location = self.locate_key_words()
        print("location:", location)
        # print("index_str:", index_str)
        # print("indexes:", indexes)
        try:
            index_list = min(index_str)
            temp_index = indexes[index_list]
            x1 = location['Left'][temp_index]
            y1 = location['Top'][temp_index]
            x2 = x1 + location['Width'][temp_index]
            y2 = y1 + location['Height'][temp_index]
            center = (x2 + 5*location['Width'][temp_index], y2)
            print("center:", center)
            ratio = 0.0
            if w > 0 and h > 0:
                ratio = (center[0]/w, center[1]/h)
            else:
                pass
            cv2.circle(self.img, center, 150, (255, 0, 0), thickness=5)
            cv2.imwrite(self.save_path, self.img)
            print("center", center)
            # print("ratio:", ratio)
            return center, ratio

        except Exception as e:
            RaiseError().logger()
            logging.exception(e)

    def __repr__(self):
        return 'Recognition'


def offset_of_image_and_a4(img, center):
    """
    换算图片和A4纸的偏移量，将图片上的圆心坐标换算到A4纸上，并计算所在的区域。
    区域划分规则： 按照A4纸的高（297mm）等分为五个区域（1～5），区域1～5从
                 纸张的底部开始到顶部依次增大。
    param img: 图片像素矩阵
    param center: 图片上盖章位置的圆心坐标
    return: 换算到A4纸上的坐标，以及该坐标所在的区域
    """
    ih = img.shape[0]
    iw = img.shape[1]
    a4 = Config.A4
    deltas = [Config.DELTAS_HEIGHT, Config.DELTAS_WIDTH]
    i_size = Config.IMAGE_REAL_SIZE
    dis_origin = Config.DISTANCE_TO_ORIGIN
    y_ = int((center[1] / ih) * i_size[0] + deltas[0])
    x = int((center[0] / iw) * i_size[1] + deltas[1])
    y = a4[0] - y_
    real_center = (int(dis_origin - x), y)
    region_width = Config.REGION_WIDTH
    region_ = 0
    if (y % region_width) > 0:
        region_ = int(y / region_width) + 1
    return real_center, region_


def draw_img(center, contract_path, draw_image_path):
    """
    通过OpenCV画圆显示，盖章位置
    param center: 所画圆形的圆心坐标（相对显示窗口而言，鼠标的点击位置坐标）
    param contract_path: 拍照后合同图片的路径
    param draw_image_path: 画完圆后的图片的保存路径
    return: 圆形在图片上的圆心坐标（相对于图片而言）
    """
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
    """
    对相机所获取的frame流，进行resize，得到想要的固定宽高比
    param frame: 相机的frame
    param width: 图片的宽
    param height: 图片的高
    return: 经过resize后的，frame流
    """
    frame1 = cv2.resize(frame, (width, height))
    return frame1


def contract_detecting(path, save_path):
    """
    识别主函数
    param path: 待识别图片的路径
    :param save_path: 识别后的保存路径
    :return: 包含A4纸上的圆心坐标，和圆心所在的区域的字典
    """
    img = cv2.imread(path)
    degree = CorrectImage(img).calc_degree()
    rotate = CorrectImage(img).rotate_image(degree)
    center, _ = Recognition(img, save_path).calculate_center()
    final_center, region = offset_of_image_and_a4(rotate, center)
    coordinate_and_region = {"final_center": final_center, "region": region}
    print("final_center:", final_center)
    return coordinate_and_region


def image_saver(img_path, save_path):
    """
    识别后，或手动选择盖章位置后的图片按照时间命名保存留底
    param img_path: 画完盖章圆形的图片的路径
    param save_path: 最终保存图片的位置路径
    """
    img = cv2.imread(img_path)
    date = time.strftime("%Y%m%d%H%M%S")
    cv2.imwrite(save_path + date + '.jpg', img)


if __name__ == "__main__":
    camera = 2
    path = "/home/devin/Documents/qianhai_devin/stamp_machinery/ocr/jpg/loan/3.jpg"
    # path = '/home/devin/Documents/qianhai_devin/stamp_machinery/Saber/Stamper_GUI/Resource/Temp_img/contract.jpg'
    save_path = "/home/devin/Documents/qianhai_devin/stamp_machinery/Saber/Stamper_GUI/Resource/Temp_img/drawn_img.jpg"

    location = contract_detecting(path, save_path)
    print("location:", location)
    # img = cv2.imread(path)
    # image_saver(img, save_path)

