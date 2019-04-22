# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '11/12/18 下午2:42'


import os
import numpy as np
import sqlite3


base_path_1 = os.path.dirname(__file__)
db_name = base_path_1 +"/Stamper.db"

#########################################
#
#      印章信息查询、插入、更新、删除操作
#
#########################################
def select_stamper_info(stamper_number):
    """
    根据印章编号,获取印章信息
    :param stamper_name:
    :return: 印章坐标，和印章名字
    """

    stamper_list = []

    conn = sqlite3.connect(db_name)
    c = conn.cursor()
    sql = "select * from stamper_info where stamper_number={}".format(stamper_number)
    cursor = c.execute(sql)

    for row in cursor:
        ret = {}
        ret['stamper_name'] = row[0]
        ret['coordinate'] = row[1]
        stamper_list.append(ret)

    conn.commit()
    conn.close()

    return stamper_list


def insert_stamper_info(stamper_dic):
    """
    插入数据
    :param stamper_dic:
    :return:
    """
    stamper_name_1 = stamper_dic['stamper_name']
    stamper_cod_info_1 = stamper_dic['stamper_cod_info']
    stamper_number_1 = stamper_dic['stamper_number']
    datetime_1 = stamper_dic['datetime']

    conn = sqlite3.connect(db_name)
    c = conn.cursor()
    sql = "INSERT INTO  stamper_info (stamper_name, stamper_cod_info, stamper_number, datetime) \
    VALUES  ('{}', '{}', {}, '{}')".format(stamper_name_1, stamper_cod_info_1, stamper_number_1, datetime_1)

    c.execute(sql)
    conn.commit()
    conn.close()


def update_stamper_info(update_info):
    """
    更新数据
    :param update_info:
    :return:
    """
    stamper_name = update_info["stamper_name"]
    datetime = update_info["datetime"]

    conn = sqlite3.connect(db_name)
    c = conn.cursor()
    sql = "UPDATE stamper_info SET stamper_name = '{}',datetime = '{}' WHERE stamper_number = {}".format(stamper_name, \
            datetime)

    c.execute(sql)
    conn.commit()
    conn.close()


def delete_stamper_info(stamper_number):
    """
    删除印章信息
    :param stamper_number:
    :return:
    """
    conn = sqlite3.connect(db_name)
    c = conn.cursor()
    sql = "delete from stamper_info where stamper_number={}".format(stamper_number)

    cursor = c.execute(sql)

    conn.commit()
    conn.close()


#########################################
#
#      用户历史操作查询、插入、更新、删除操作
#
#########################################
def select_user_operation():
    """
    查询用户操作的信息
    :return:
    """

    conn = sqlite3.connect(db_name)
    c = conn.cursor()
    sql = "select * from user_operation"
    cursor = c.execute(sql)
    results = cursor.fetchall()

    history_list = []

    for row in results:
        history_info = {}
        history_info['id'] = row[0]
        history_info['user_name'] = row[1]
        history_info['stamper_name'] = row[2]
        history_info['history_img_path'] = row[3]
        history_info['datetime'] = row[4]
        history_list.append(history_info)

    conn.commit()
    conn.close()

    return history_list


def insert_user_operation(history_info):
    """
    用户操作信息插入
    :param history_info:
    :return:
    """
    conn = sqlite3.connect(db_name)
    c = conn.cursor()
    sql = "INSERT INTO user_operation (user_name, stamper_name, history_img_path, datetime) \
            VALUES ('{}', '{}', '{}', '{}')".format(history_info['user_name'], ['stamper_name'], history_info['history_img_path'], history_info['datetime'])
    cursor = c.execute(sql)

    conn.commit()
    conn.close()


#########################################
#
#      用户操作查询、插入、更新、删除操作
#
#########################################
def select_user_info(user_name=None):
    """
    用户信息查询
    :return:
    """
    user_list = []

    conn = sqlite3.connect(db_name)
    c = conn.cursor()
    if user_name == None:
        sql = "select * from user_info"
    else:
        sql = "select * from user_info where user_name ='{}'".format(user_name)

    cursor = c.execute(sql)

    for row in cursor:
        user_info = {}
        user_info['user_id'] = row[0]
        user_info['user_name'] = row[1]
        user_info['user_face_info'] = row[2]
        user_info['user_face_path'] = row[3]
        user_info['datetime'] = row[4]
        user_list.append(user_info)

    conn.commit()
    conn.close()

    return user_list


def insert_user_info(user_info):
    """
    用户信息插入
    :param user_ifno:
    :return:
    """
    conn = sqlite3.connect(db_name)
    c = conn.cursor()
    sql = "INSERT INTO user_info (user_name,user_face_info, user_face_path, datetime) \
          VALUES ('{}', '{}', '{}', '{}')".format(user_info['user_name'],\
                                                        user_info['user_face_info'],\
                                                        user_info['user_face_path'],\
                                                        user_info['datetime']
                                                  )
    curosr = c.execute(sql)

    conn.commit()
    conn.close()


def delete_user_info(user_name):
    """
    删除印章信息
    :param user_name:
    :return:
    """
    conn = sqlite3.connect(db_name)
    c = conn.cursor()
    sql = "delete from user_info where user_name='{}'".format(user_name)

    try:
        cursor = c.execute(sql)
        cursor.commit()
    except Exception as e:
        return -1
    conn.close()

    return 0

# stamper_dic = {}
# stamper_dic['stamper_name'] = "hello"
# stamper_dic['stamper_cod_info'] = "200300"
# stamper_dic['stamper_number'] = 1
# stamper_dic['datetime'] = "33333"

# insert_stamper_info(stamper_dic)
# update_stamper_info(stamper_dic)

# ret = select_stamper_info()
# print(ret)

# try:
#     delete_stamper_info(1)
# except Exception as e:
#     print(str(e))


def history_img():
    """
    返回所有history信息
    :return:
    """
    results = select_user_operation()
    for row in results:
        history_info = {}
        history_info['id'] = row[0]
        history_info['user_name'] = row[1]
        history_info['stamper_name'] = row[2]
        history_info['history_img_path'] = row[3]
        history_info['datetime'] = row[4]
        yield history_info


def Get_img_path(page_num):
    """
    获取图片信息
    :return:
    """
    start = 8*(page_num-1)
    end = 8*page_num
    history_list = select_user_operation()
    try:
        for i in range(start, end):
            seq = i%8
            history_info = history_list[i]
            # print(history_info['history_img_path'])
            # self.img_path['lab{}'.format(seq)] = history_info['history_img_path']
            # print(history_info)
    except Exception as e:
        print(str(e))


if __name__ == "__main__":
    pass
    ret = select_user_info()

    known_face_encodings = [i['user_face_info'] for i in ret]
    known_face_names = [i['user_name'] for i in ret]

    print([np.array(eval(','.join(i.split()))) for i in known_face_encodings])
    print(known_face_names)




