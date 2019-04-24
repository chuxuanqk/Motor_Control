import os

# 获取当前文件路径
current_path = os.path.abspath(__file__)
# 获取当前文件的父目录
father_path = os.path.abspath(os.path.dirname(current_path) + os.path.sep)


# 图片资源
welcome = './Resource/img/welcome.png'
face_img = './Resource/img/face_img'
back_img = './Resource/img/矩形背景2.png'
wait_img = './Resource/img/wait_lab.png'

# QSS资源
Face_QSS = './Resource/QSS/Face_style.qss'
Serial_QSS = './Resource/QSS/Serial_style.qss'
Preview_QSS = './Resource/QSS/preview_style.qss'
Hand_QSS = './Resource/QSS/hand_style.qss'

# Temp_img资源
contract_path = './Resource/Temp_img/contract.jpg'
drawn_img_path = './Resource/Temp_img/drawn_img.jpg'

# 摄像头
face_camera = 0
camera = 0


# 数据库
# Database