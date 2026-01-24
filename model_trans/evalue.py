import cv2
import time
import torch
import torch.nn as nn
import numpy as np
from PIL import Image,ImageDraw,ImageFont
from rknn.api import RKNN

from yolo import YOLO
from nets.yolo import YoloBody
from utils.utils import (cvtColor, get_anchors, get_classes, preprocess_input,resize_image, show_config)
from utils.utils_bbox import DecodeBox, DecodeBoxNP    
             
PT_PATH = 'model_data/yolov5_s.pth'
RKNN_PATH = 'model_data/models_10.rknn'
MEANS = [123.675, 116.28, 103.53]
STDS = [58.395, 58.395, 58.395]
video_path      = 0
video_save_path = "video_out/output_2.avi"
video_fps       = 25.0
if __name__ == '__main__':
    # 读取测试图像
    image = Image.open("img/street.jpg")
    
    # 使用 torch 进行推理
    yolo = YOLO()
    # 创建 RKNN 对象
    rknn = RKNN()
    # 从当前目录加载 RKNN 模型 resnet_18
    ret = rknn.load_rknn(path='model_data/models_10.rknn')
    if ret != 0:
        print('Load Pytorch model failed!')
        exit(ret)
    # 初始化运行时环境
    ret = rknn.init_runtime(target='rv1126', device_id='939c4ee9505c48aa')
    if ret != 0:
        print('Init runtime environment failed')
        exit(ret)
    r1_image = yolo.detect_image(image)
    r1_image.save("img_out/res1.jpg")
    r2_image = yolo.detect_image_rknn(image, rknn)
    r2_image.save("img_out/res2.jpg")
    capture = cv2.VideoCapture(video_path)
    if video_save_path!="":
        fourcc  = cv2.VideoWriter_fourcc(*'XVID')
        size    = (int(capture.get(cv2.CAP_PROP_FRAME_WIDTH)), int(capture.get(cv2.CAP_PROP_FRAME_HEIGHT)))
        out     = cv2.VideoWriter(video_save_path, fourcc, video_fps, size)

    ref, frame = capture.read()
    if not ref:
        raise ValueError("未能正确读取摄像头（视频），请注意是否正确安装摄像头（是否正确填写视频路径）。")

    fps = 0.0
    while(True):
        t1 = time.time()
        # 读取某一帧
        ref, frame = capture.read()
        if not ref:
            break
        # 格式转变，BGRtoRGB
        frame = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
        # 转变成Image
        frame = Image.fromarray(np.uint8(frame))
        # 进行检测
        frame = np.array(yolo.detect_image_rknn(frame, rknn))
        # RGBtoBGR满足opencv显示格式
        frame = cv2.cvtColor(frame,cv2.COLOR_RGB2BGR)
            
        fps  = ( fps + (1./(time.time()-t1)) ) / 2
        print("fps= %.2f"%(fps))
        frame = cv2.putText(frame, "fps= %.2f"%(fps), (0, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
            
        cv2.imshow("video",frame)
        c= cv2.waitKey(1) & 0xff 
        if video_save_path!="":
            out.write(frame)

        if c==27:
            capture.release()
            break
    print("Video Detection Done!")
    capture.release()
    if video_save_path!="":
        print("Save processed video to the path :" + video_save_path)
        out.release()
    cv2.destroyAllWindows()
    
