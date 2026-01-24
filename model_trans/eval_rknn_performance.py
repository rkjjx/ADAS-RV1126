from rknn.api import RKNN 
#RKNN_PATH = '/home/alientek/ADAS/model_trans/model_data/mobilefacenet/mobilefacenet.pre.rknn' 
#RKNN_PATH = '/home/alientek/ADAS/model_trans/model_data/ultralytics_yolov5/yolov5s_v4.0/best.pre.rknn' 
RKNN_PATH ='/home/alientek/ADAS/model_trans/model_data/face_keypoints/pfpld.rknn'
def eval_perf_with_simulator(): 
    # 创建RKNN对象 
    rknn = RKNN() 
    # 从当前目录加载RKNN模型resnet_18 
    ret = rknn.load_rknn(path=RKNN_PATH) 
    if ret != 0: 
        print('Load Pytorch model failed!') 
        exit(ret) 
    # 初始化运行时环境 
    ret = rknn.init_runtime() 
    if ret != 0: 
        print('Init runtime environment failed') 
        exit(ret) 
    # 调用eval_perf接口进行性能评估 
    rknn.eval_perf() 
    # 释放RKNN对象 
    rknn.release() 
def eval_perf_with_rk1808(): 
    # 创建RKNN对象 
    rknn = RKNN() 
    # 从当前目录加载RKNN模型resnet_18 
    ret = rknn.load_rknn(path=RKNN_PATH) 
    if ret != 0: 
        print('Load Pytorch model failed!') 
        exit(ret) 
	# 初始化运行时环境 
	## 默认perf_debug为False,如果要打印每一层耗时,设置该参数的值为True 
    ret = rknn.init_runtime(target='rv1126', device_id='20230331RV1126FD2G0033', perf_debug=True) 
    if ret != 0: 
        print('Init runtime environment failed') 
        exit(ret) 
    # 调用eval_perf接口统计模型运行耗时,loop_cnt指定循环次数,返回的是每帧的平均耗时 
    rknn.eval_perf(loop_cnt=100) 
    # 释放RKNN对象 
    rknn.release()
if __name__ == '__main__': 
    # 使用模拟器进行评估(只能在x86_64 Linux上使用)
    # eval_perf_with_simulator() 
    # 使用RK1808进行性能评估 
    eval_perf_with_rk1808()
