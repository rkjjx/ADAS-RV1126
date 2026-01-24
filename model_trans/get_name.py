# -*- coding:utf-8 -*-
import os
# 图片地址
data_base_dir = r"/home/alientek/ADAS/model_trans/dataset/face_dataset"
file_list = [] #建立列表，用于保存图片信息
# txt文件地址
write_file_name = r'/home/alientek/ADAS/model_trans/model_data/mobilefacenet/dataset.txt'
write_file = open(write_file_name, "a") #以只写方式打开write_file_name文件
start = 0
for file in os.listdir(data_base_dir): #file为current_dir当前目录下图片名
    if file.endswith(".jpg") and start < 2000: #如果file以jpg结尾
        write_name = file #图片路径 + 图片名 + 标签
        file_list.append(write_name) #将write_name添加到file_list列表最后
        sorted(file_list) #将列表中所有元素随机排列
        number_of_lines = len(file_list) #列表中元素个数
        start = start + 1
#将图片信息写入txt文件中
for current_line in range(number_of_lines):
    write_file.write(r"/home/alientek/ADAS/model_trans/dataset/face_dataset/" + file_list[current_line] + '\n')
#关闭文件
write_file.close()


"""
data_base_dir = r"/home/alientek/ADAS/model_trans/dataset/phone_smoking_dataset"
file_list = [] #建立列表，用于保存图片信息
# txt文件地址
write_file_name = r'/home/alientek/ADAS/model_trans/model_data/ultralytics_yolov5/yolov5m_v4.0/dataset.txt'
write_file = open(write_file_name, "a") #以只写方式打开write_file_name文件
start = 0
for file in os.listdir(data_base_dir): #file为current_dir当前目录下图片名
    if file.endswith(".jpg") and start < 2000: #如果file以jpg结尾
        write_name = file #图片路径 + 图片名 + 标签
        file_list.append(write_name) #将write_name添加到file_list列表最后
        sorted(file_list) #将列表中所有元素随机排列
        number_of_lines = len(file_list) #列表中元素个数
        start = start + 1
#将图片信息写入txt文件中
for current_line in range(number_of_lines):
    write_file.write(r"/home/alientek/ADAS/model_trans/dataset/phone_smoking_dataset/" + file_list[current_line] + '\n')
#关闭文件
write_file.close()
"""

