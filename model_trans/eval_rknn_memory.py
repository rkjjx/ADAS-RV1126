from rknn.api import RKNN 
RKNN_PATH = '/home/alientek/ADAS/model_trans/model_data/mobilefacenet/mobilefacenet.rknn' 
#RKNN_PATH = '/home/alientek/ADAS/model_trans/model_data/ultralytics_yolov5/yolov5s_v4.0/best.pre.rknn'
#RKNN_PATH ='/home/alientek/ADAS/model_trans/model_data/face_keypoints/pfpld.pre.rknn'
def eval_mem_with_rk1808(): 
    # 创建RKNN对象 
    rknn = RKNN() 
    # 从当前目录加载RKNN模型resnet18 
    ret = rknn.load_rknn(path=RKNN_PATH) 
    if ret != 0: 
        print('Load Pytorch model failed!') 
        exit(ret) 
    # 初始化运行时环境 
    ## 设置eval_mem为True,进入内存评估模式 
    ret = rknn.init_runtime(target='rv1126', device_id='20230331RV1126FD2G0033', eval_mem=True) 
    if ret != 0: 
        print('Init runtime environment failed') 
        exit(ret) 
        # 调用eval_memory接口统计模型运行时内存使用情况 
    rknn.eval_memory() 
    # 释放RKNN对象 
    rknn.release() 
if __name__ == '__main__': 
    # 使用RK1808进行内存评估 
    eval_mem_with_rk1808()
