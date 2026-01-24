import cv2
import torch 
import numpy as np
from rknn.api import RKNN 
import onnxruntime as ort
ONNX_PATH = '/home/alientek/ADAS/model_trans/model_data/mobilefacenet/mobilefacenet.onnx' 
RKNN_PATH = '/home/alientek/ADAS/model_trans/model_data/mobilefacenet/mobilefacenet.pre.rknn'

#ONNX_PATH = '/home/alientek/ADAS/model_trans/model_data/ultralytics_yolov5/yolov5s_v4.0/best.onnx' 
#RKNN_PATH = '/home/alientek/ADAS/model_trans/model_data/ultralytics_yolov5/yolov5s_v4.0/best.rknn' 

MEANS = [0, 0, 0] 
STDS = [255, 255, 255] 
def inference_with_rknn(target, device_id, inputs): 
    # (保持原有的 RKNN 推理代码不变，除了修正原来代码中的语法错误)
    rknn = RKNN() 
    # 注意：你原代码这里有语法错误 "resnet_18 ret =", 下面是修正后的
    ret = rknn.load_rknn(path=RKNN_PATH) 
    if ret != 0: 
        print('Load RKNN model failed!') 
        exit(ret) 
    ret = rknn.init_runtime(target=target, device_id=device_id) 
    if ret != 0: 
        print('Init runtime environment failed') 
        exit(ret) 
    
    outputs = rknn.inference(inputs=inputs) 
    rknn.release() 
    return outputs
def inference_with_onnx(img, onnx_path):
    # --- 预处理 (保持与之前一致逻辑) ---
    img = (img - MEANS) / STDS
    # 调整维度: HWC (224,224,3) -> NCHW (1,3,224,224)
    img = img.transpose((2, 0, 1)) # 先变成 CHW
    img = img.reshape((1, 3, 112, 112)) # 再增加 Batch 维度
    img = img.astype(np.float32) # 确保是 float32
    
    # --- 推理核心 ---
    # 创建推理会话
    session = ort.InferenceSession(onnx_path)
    
    # 获取输入节点的名称 (自动获取，防止名字写死报错)
    input_name = session.get_inputs()[0].name
    
    # 运行推理 (输入是字典形式 {input_name: numpy_array})
    outputs = session.run(None, {input_name: img})
    
    # ONNX 输出是一个列表，通常取第一个
    return outputs
def compute_cos_sim(x, y):
    print(f"x shape (before flatten): {x.shape}")
    print(f"y shape (before flatten): {y.shape}")

    x = x.flatten()
    y = y.flatten()

    cos_dist = np.dot(x, y) / (np.linalg.norm(x) * np.linalg.norm(y))
    return cos_dist

if __name__ == '__main__': 
    
    img_1 = cv2.imread('/home/alientek/ADAS/model_trans/img/1.jpg') 
    img_1 = cv2.cvtColor(img_1, cv2.COLOR_BGR2RGB) 
    img_1 = cv2.resize(img_1, (112,112), interpolation=cv2.INTER_LINEAR)
    # 1. RKNN 推理
    rknn_outs_1 = inference_with_rknn(target='rv1126', device_id='20230331RV1126FD2G0033', inputs=[img_1]) 
    
    img_2 = cv2.imread('/home/alientek/ADAS/model_trans/img/2.jpg') 
    img_2 = cv2.cvtColor(img_2, cv2.COLOR_BGR2RGB) 
    img_2 = cv2.resize(img_2, (112,112), interpolation=cv2.INTER_LINEAR)
    # 1. RKNN 推理
    rknn_outs_2 = inference_with_rknn(target='rv1126', device_id='20230331RV1126FD2G0033', inputs=[img_2]) 
    


    # 2. ONNX 推理 (替代原来的 Torch 推理)
    #onnx_outs_1 = inference_with_onnx(img=img_1, onnx_path=ONNX_PATH) 
    #onnx_outs_2 = inference_with_onnx(img=img_2, onnx_path=ONNX_PATH) 
    # 3. 对比相似度
    # rknn_outs[0] 是 numpy 数组，onnx_outs 也是 numpy 数组，直接计算
    cos_dis = compute_cos_sim(rknn_outs_1[0], rknn_outs_2[0]) 
    print("Cosine Similarity of RKNN output[0] and ONNX output[0]: {}".format(cos_dis))
    #cos_dis = compute_cos_sim(rknn_outs[1], onnx_outs[1]) 
    #print("Cosine Similarity of RKNN output[1] and ONNX output[1]: {}".format(cos_dis))
    #cos_dis = compute_cos_sim(rknn_outs[2], onnx_outs[2]) 
    #print("Cosine Similarity of RKNN output[2] and ONNX output[2]: {}".format(cos_dis))
