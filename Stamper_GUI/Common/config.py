# -*- coding: UTF-8 -*-

class Config:
    """
    Parameters should be measured.
    DEGREE:              The degree need to be rotating
    A4:                  The height, and the width of A4 paper(mm)
    DELTAS_HEIGHT:       The distance between the left side of image and the left side of A4 paper
    DELTAS_WIDTH:        The distance between top side of image and top side of A4 paper
    IMAGE_REAL_SIZE:     The height, and the width of image(mm)
    DISTANCE_TO_ORIGIN:  The distance between the left side of A4 paper and the origin point.
    REGION_WIDTH:        The height of each region
    CLICK_WINDOW:        The size of click window
    KEY_LIST:            The key words need to be found out
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