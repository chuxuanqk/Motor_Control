# -*- coding: UTF-8 -*-

class Config:
    """
    需要手动测量的参数.
    DEGREE:              拍照后图片需要旋转的角度（通常为90度，可根据视野中图片的方向调整）
    A4:                  A4纸的宽高，宽为：210mm，高为：297mm
    DELTAS_HEIGHT:       图片的左边缘离A4纸的左边缘的实际距离（单位：mm）
    DELTAS_WIDTH:        图片的上边缘离A4纸的上边缘的实际距离（单位：mm）
    IMAGE_REAL_SIZE:     图片的宽高（实际测量值，单位：mm）
    DISTANCE_TO_ORIGIN:  A4纸的左边缘距离坐标原点的距离.
    REGION_WIDTH:        将A4纸分区后，对应每个区域的高度
    CLICK_WINDOW:        手动点击的桌面窗口的大小
    KEY_LIST:            需要从合同中找出的关键字
    """
    DEGREE = 90
    A4 = [297, 210]
    DELTAS_HEIGHT = 14
    DELTAS_WIDTH = 20
    IMAGE_REAL_SIZE = [242, 157]
    DISTANCE_TO_ORIGIN = 246
    REGION_WIDTH = 59
    CLICK_WINDOW = [600, 420]
    KEY_LIST = ['借款方', '贷款方', '出借方', '甲方', '借款人', '盖章']