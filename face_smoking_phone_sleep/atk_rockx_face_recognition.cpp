/****************************************************************************
*
*    Copyright (c) 2017 - 2019 by Rockchip Corp.  All rights reserved.
*
*    The material in this file is confidential and contains trade secrets
*    of Rockchip Corporation. This is proprietary information owned by
*    Rockchip Corporation. No part of this work may be disclosed,
*    reproduced, copied, transmitted, or used in any way for any purpose,
*    without the express written permission of Rockchip Corporation.
*
*****************************************************************************/
#include "atk_rockx_face_recognition.h"
#include <cmath>  
#include <math.h>

bool atk_face_recognition_quit = false;
int face_size,face_xstart,face_ystart;

RK_U32 video_width = 1280;
RK_U32 video_height = 720;
int disp_width = 720;
int disp_height = 1280;
int atk_recognition_init() 
{
  RK_CHAR *pDeviceName = "rkispp_scale0";
  RK_CHAR *pcDevNode = "/dev/dri/card0";
  char *iq_file_dir = "/etc/iqfiles";
  RK_S32 s32CamId = 0;
  RK_U32 u32BufCnt = 3;
  RK_U32 fps = 10;
  int ret;
  pthread_t rkmedia_vi_face_tidp;
  RK_BOOL bMultictx = RK_FALSE;
  
  printf("\n###############################################\n");
  printf("VI CameraIdx: %d\npDeviceName: %s\nResolution: %dx%d\n\n",
          s32CamId,pDeviceName,video_width,video_height);
  printf("VO pcDevNode: %s\nResolution: %dx%d\n",
          pcDevNode,disp_height,disp_width);
  printf("###############################################\n\n");

  if (iq_file_dir) 
  {
#ifdef RKAIQ
    printf("#Rkaiq XML DirPath: %s\n", iq_file_dir);
    printf("#bMultictx: %d\n\n", bMultictx);
    rk_aiq_working_mode_t hdr_mode = RK_AIQ_WORKING_MODE_NORMAL;
    SAMPLE_COMM_ISP_Init(s32CamId, hdr_mode, bMultictx, iq_file_dir);
    SAMPLE_COMM_ISP_Run(s32CamId);
    SAMPLE_COMM_ISP_SetFrameRate(s32CamId, fps);
#endif
  }

  open_db();
  
  RK_MPI_SYS_Init();
  VI_CHN_ATTR_S vi_chn_attr;
  vi_chn_attr.pcVideoNode = pDeviceName;
  vi_chn_attr.u32BufCnt = u32BufCnt;
  vi_chn_attr.u32Width = video_width;
  vi_chn_attr.u32Height = video_height;
  vi_chn_attr.enPixFmt = IMAGE_TYPE_NV12;
  vi_chn_attr.enBufType = VI_CHN_BUF_TYPE_MMAP;
  vi_chn_attr.enWorkMode = VI_WORK_MODE_NORMAL;
  ret = RK_MPI_VI_SetChnAttr(s32CamId, 1, &vi_chn_attr);
  ret |= RK_MPI_VI_EnableChn(s32CamId, 1);
  if (ret) 
  {
    printf("ERROR: create VI[0:1] error! ret=%d\n", ret);
    return -1;
  }
  RGA_ATTR_S stRgaAttr1;
  memset(&stRgaAttr1, 0, sizeof(stRgaAttr1));
  stRgaAttr1.bEnBufPool = RK_TRUE;
  stRgaAttr1.u16BufPoolCnt = 3;
  stRgaAttr1.u16Rotaion = 270;
  stRgaAttr1.stImgIn.u32X = 0;
  stRgaAttr1.stImgIn.u32Y = 0;
  stRgaAttr1.stImgIn.imgType = IMAGE_TYPE_NV12;
  stRgaAttr1.stImgIn.u32Width = video_width;
  stRgaAttr1.stImgIn.u32Height = video_height;
  stRgaAttr1.stImgIn.u32HorStride = video_width;
  stRgaAttr1.stImgIn.u32VirStride = video_height;
  stRgaAttr1.stImgOut.u32X = 0;
  stRgaAttr1.stImgOut.u32Y = 0;
  stRgaAttr1.stImgOut.imgType = IMAGE_TYPE_BGR888;
  stRgaAttr1.stImgOut.u32Width = disp_width;
  stRgaAttr1.stImgOut.u32Height = disp_height;
  stRgaAttr1.stImgOut.u32HorStride = disp_width;
  stRgaAttr1.stImgOut.u32VirStride = disp_height;
  ret = RK_MPI_RGA_CreateChn(1, &stRgaAttr1);
  if (ret) {
      printf("ERROR: create RGA[0:1] falied! ret=%d\n", ret);
      return -1;
  }
  RGA_ATTR_S stRgaAttr;
  memset(&stRgaAttr, 0, sizeof(stRgaAttr));
  stRgaAttr.bEnBufPool = RK_TRUE;
  stRgaAttr.u16BufPoolCnt = 3;
  stRgaAttr.u16Rotaion = 0;
  stRgaAttr.stImgIn.u32X = 0;
  stRgaAttr.stImgIn.u32Y = 0;
  stRgaAttr.stImgIn.imgType = IMAGE_TYPE_BGR888;
  stRgaAttr.stImgIn.u32Width = disp_width;
  stRgaAttr.stImgIn.u32Height = disp_height;
  stRgaAttr.stImgIn.u32HorStride = disp_width;
  stRgaAttr.stImgIn.u32VirStride = disp_height;
  stRgaAttr.stImgOut.u32X = 0;
  stRgaAttr.stImgOut.u32Y = 0;
  stRgaAttr.stImgOut.imgType = IMAGE_TYPE_BGR888;
  stRgaAttr.stImgOut.u32Width = disp_width;
  stRgaAttr.stImgOut.u32Height = disp_height;
  stRgaAttr.stImgOut.u32HorStride = disp_width;
  stRgaAttr.stImgOut.u32VirStride = disp_height;
  ret = RK_MPI_RGA_CreateChn(0, &stRgaAttr);
  if (ret) {
      printf("ERROR: create RGA[0:0] falied! ret=%d\n", ret);
      return -1;
  }
  /*
  VO_CHN_ATTR_S stVoAttr = {0};
  stVoAttr.pcDevNode = "/dev/dri/card0";
  stVoAttr.emPlaneType = VO_PLANE_PRIMARY;
  stVoAttr.enImgType = IMAGE_TYPE_BGR888;
  stVoAttr.u16Zpos = 0;
  stVoAttr.stImgRect.s32X = 0;
  stVoAttr.stImgRect.s32Y = 0;
  stVoAttr.stImgRect.u32Width = disp_width;
  stVoAttr.stImgRect.u32Height = disp_height;
  stVoAttr.stDispRect.s32X = 0;
  stVoAttr.stDispRect.s32Y = 0;
  stVoAttr.stDispRect.u32Width = disp_width;
  stVoAttr.stDispRect.u32Height = disp_height;
  ret = RK_MPI_VO_CreateChn(0, &stVoAttr);
  if (ret)
  {
    printf("ERROR: create VO[0:0] failed! ret=%d\n", ret);
    return -1;
  }
  */
  MPP_CHN_S stSrcChn;
  MPP_CHN_S stDestChn;
  printf("Bind VI[0:1] to RGA[0:1]....\n");
  stSrcChn.enModId = RK_ID_VI;
  stSrcChn.s32DevId = s32CamId;
  stSrcChn.s32ChnId = 1;
  stDestChn.enModId = RK_ID_RGA;
  stDestChn.s32DevId = s32CamId;
  stDestChn.s32ChnId = 1;
  ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
  if (ret) 
  {
    printf("ERROR: Bind VI[0:1] to RGA[0:1] failed! ret=%d\n", ret);
    return -1;
  }
  pthread_create(&rkmedia_vi_face_tidp, NULL, rkmedia_vi_face_thread, NULL);
  printf("%s initial finish\n", __func__);
  
  
  
  while (!atk_face_recognition_quit) 
  {
    usleep(500000);//0.5s
  }

  printf("%s exit!\n", __func__);
  printf("Unbind VI[0:1] to RGA[0:1]....\n");
  stSrcChn.enModId = RK_ID_VI;
  stSrcChn.s32DevId = s32CamId;
  stSrcChn.s32ChnId = 1;
  stDestChn.enModId = RK_ID_RGA;
  stSrcChn.s32DevId = s32CamId;
  stDestChn.s32ChnId = 1;
  ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
  if (ret) 
  {
    printf("ERROR: unbind VI[0:1] to RGA[0:1] failed! ret=%d\n", ret);
    return -1;
  }
  //RK_MPI_VO_DestroyChn(0);
  RK_MPI_RGA_DestroyChn(0);
  RK_MPI_RGA_DestroyChn(1);
  printf("Destroy VI[0:1] channel\n");
  ret = RK_MPI_VI_DisableChn(s32CamId, 1);
  if (ret) 
  {
    printf("ERROR: destroy VI[0:1] error! ret=%d\n", ret);
    return -1;
  }

  if (iq_file_dir) 
  {
#if RKAIQ
    SAMPLE_COMM_ISP_Stop(s32CamId);
#endif
  }
  return 0;
}


void *rkmedia_vi_face_thread(void *args) 
{
  pthread_detach(pthread_self());//将线程状态改为unjoinable状态，确保资源的释放
  map<string, rockx_face_feature_t> database_face_map = FaceFeature();
  map<string, rockx_face_feature_t>::iterator database_iter;
  int ret;
  
  rknn_context yolov5s_ctx;
  int yolov5s_len = 0;
  unsigned char *yolov5s;
  static char *yolov5s_path = "/demo/bin/best.pre.rknn";

  rknn_context pfpld_ctx;
  int pfpld_len = 0;
  unsigned char *pfpld;
  static char *pfpld_path = "/demo/bin/pfpld.pre.rknn";
   
  rknn_context mobilefacenet_ctx;
  int mobilefacenet_len = 0;
  unsigned char *mobilefacenet;
  static char *mobilefacenet_path = "/demo/bin/mobilefacenet.pre.rknn";
 
  float res_landmk[98][2];
  float res_angle[3];
  int frame_cnt = 0;
  int frame_cnt_eye = 0;
  int nap_count = 0;

  string face_name = "";
  
  struct timespec start, end;
  int fps;
  

  // Load yolov5s RKNN Model
  printf("Loading yolov5s model ...\n");  
  yolov5s = load_model(yolov5s_path, &yolov5s_len);
  ret = rknn_init(&yolov5s_ctx, yolov5s, yolov5s_len, 0);
  if (ret < 0)
  {
    printf("yolov5s rknn_init fail! ret=%d\n", ret);
    return NULL;
  }
  
  // Get yolov5s Input Output Info
  rknn_input_output_num yolov5s_io_num;
  ret = rknn_query(yolov5s_ctx, RKNN_QUERY_IN_OUT_NUM, &yolov5s_io_num, sizeof(yolov5s_io_num));
  if (ret != RKNN_SUCC)
  {
    printf("yolov5s rknn_query fail! ret=%d\n", ret);
    return NULL;
  }
  printf("yolov5s model input num: %d, output num: %d\n", yolov5s_io_num.n_input,yolov5s_io_num.n_output);

  printf("yolov5s input tensors:\n");
  rknn_tensor_attr yolov5s_input_attrs[yolov5s_io_num.n_input];
  memset(yolov5s_input_attrs, 0, sizeof(yolov5s_input_attrs));
  for (unsigned int i = 0; i < yolov5s_io_num.n_input; i++)
  {
    yolov5s_input_attrs[i].index = i;
    ret = rknn_query(yolov5s_ctx, RKNN_QUERY_INPUT_ATTR, &(yolov5s_input_attrs[i]),sizeof(rknn_tensor_attr));
    if (ret != RKNN_SUCC)
    {
      printf("yolov5s rknn_query fail! ret=%d\n", ret);
      return NULL;
    }
    printRKNNTensor(&(yolov5s_input_attrs[i]));
  }

  printf("yolov5s_output tensors:\n");
  rknn_tensor_attr yolov5s_output_attrs[yolov5s_io_num.n_output];
  memset(yolov5s_output_attrs, 0, sizeof(yolov5s_output_attrs));
  for (unsigned int i = 0; i < yolov5s_io_num.n_output; i++)
  {
    yolov5s_output_attrs[i].index = i;
    ret = rknn_query(yolov5s_ctx, RKNN_QUERY_OUTPUT_ATTR, &(yolov5s_output_attrs[i]),sizeof(rknn_tensor_attr));
    if (ret != RKNN_SUCC)
    {
      printf("rknn_query fail! ret=%d\n", ret);
      return NULL;
    }
    printRKNNTensor(&(yolov5s_output_attrs[i]));
  }

  // get yolov5s model's input image width and height
  int yolov5s_channel = 3;
  int yolov5s_width = 0;
  int yolov5s_height = 0;
  if (yolov5s_input_attrs[0].fmt == RKNN_TENSOR_NCHW)
  {
      printf("yolov5s model is NCHW input fmt\n");
      yolov5s_width = yolov5s_input_attrs[0].dims[0];
      yolov5s_height = yolov5s_input_attrs[0].dims[1];
  }
  else
  {
      printf("yolov5s model is NHWC input fmt\n");
      yolov5s_width = yolov5s_input_attrs[0].dims[1];
      yolov5s_height = yolov5s_input_attrs[0].dims[2];
  }
  printf("yolov5s input height=%d, width=%d, channel=%d\n", yolov5s_height, yolov5s_width, yolov5s_channel);
    // Load Pfpld RKNN Model
  printf("Loading pfpld.pre.rknn model ...\n");            
  pfpld = load_model(pfpld_path, &pfpld_len);
  ret = rknn_init(&pfpld_ctx, pfpld, pfpld_len, 0);
  if (ret < 0)
  {
    printf("pfpld rknn_init fail! ret=%d\n", ret);
    return NULL;
  }
  
  // Get Pfpld Input Output Info
  rknn_input_output_num pfpld_io_num;
  ret = rknn_query(pfpld_ctx, RKNN_QUERY_IN_OUT_NUM, &pfpld_io_num, sizeof(pfpld_io_num));
  if (ret != RKNN_SUCC)
  {
    printf("pfpld rknn_query fail! ret=%d\n", ret);
    return NULL;
  }
  printf("pfpld model input num: %d, output num: %d\n", pfpld_io_num.n_input,pfpld_io_num.n_output);

  // print pfpld input tensor
  printf("pfpld input tensors:\n");
  rknn_tensor_attr pfpld_input_attrs[pfpld_io_num.n_input];
  memset(pfpld_input_attrs, 0, sizeof(pfpld_input_attrs));
  for (unsigned int i = 0; i < pfpld_io_num.n_input; i++)
  {
    pfpld_input_attrs[i].index = i;
    ret = rknn_query(pfpld_ctx, RKNN_QUERY_INPUT_ATTR, &(pfpld_input_attrs[i]),sizeof(rknn_tensor_attr));
    if (ret != RKNN_SUCC)
    {
      printf("pfpld rknn_query fail! ret=%d\n", ret);
      return NULL;
    }
    printRKNNTensor(&(pfpld_input_attrs[i]));
  }

  // print pfpld output tensor
  printf("pfpld output tensors:\n");
  rknn_tensor_attr pfpld_output_attrs[pfpld_io_num.n_output];
  memset(pfpld_output_attrs, 0, sizeof(pfpld_output_attrs));
  for (unsigned int i = 0; i < pfpld_io_num.n_output; i++)
  {
    pfpld_output_attrs[i].index = i;
    ret = rknn_query(pfpld_ctx, RKNN_QUERY_OUTPUT_ATTR, &(pfpld_output_attrs[i]),sizeof(rknn_tensor_attr));
    if (ret != RKNN_SUCC)
    {
      printf("pfpld rknn_query fail! ret=%d\n", ret);
      return NULL;
    }
    printRKNNTensor(&(pfpld_output_attrs[i]));
  }

  // get pfpld model's input image width and height
  int pfpld_channel = 3;
  int pfpld_width = 0;
  int pfpld_height = 0;
  if (pfpld_input_attrs[0].fmt == RKNN_TENSOR_NCHW)
  {
      printf("pfpld model is NCHW input fmt\n");
      pfpld_width = pfpld_input_attrs[0].dims[0];
      pfpld_height = pfpld_input_attrs[0].dims[1];
  }
  else
  {
      printf("pfpld model is NHWC input fmt\n");
      pfpld_width = pfpld_input_attrs[0].dims[1];
      pfpld_height = pfpld_input_attrs[0].dims[2];
  }
  printf("pfpld model input height=%d, width=%d, channel=%d\n", pfpld_height, pfpld_width, pfpld_channel);
  
  // Load mobilefacenet RKNN Model
  printf("Loading mobilefacenet.rknn model ...\n");            
  mobilefacenet = load_model(mobilefacenet_path, &mobilefacenet_len);
  printf("rknn_init...\n");
  ret = rknn_init(&mobilefacenet_ctx, mobilefacenet, mobilefacenet_len, 0);
  if (ret < 0)
  {
    printf("mobilefacenet rknn_init fail! ret=%d\n", ret);
    return NULL;
  }
  // Get mobilefacenet Input Output Info
  printf("mobilefacenet_io_num...\n");
  rknn_input_output_num mobilefacenet_io_num;
  ret = rknn_query(mobilefacenet_ctx, RKNN_QUERY_IN_OUT_NUM, &mobilefacenet_io_num, sizeof(mobilefacenet_io_num));
  if (ret != RKNN_SUCC)
  {
    printf("mobilefacenet rknn_query fail! ret=%d\n", ret);
    return NULL;
  }
  printf("mobilefacenet model input num: %d, output num: %d\n", mobilefacenet_io_num.n_input,mobilefacenet_io_num.n_output);

  // print mobilefacenet input tensor
  printf("mobilefacenet input tensors:\n");
  rknn_tensor_attr mobilefacenet_input_attrs[mobilefacenet_io_num.n_input];
  memset(mobilefacenet_input_attrs, 0, sizeof(mobilefacenet_input_attrs));
  for (unsigned int i = 0; i < mobilefacenet_io_num.n_input; i++)
  {
    mobilefacenet_input_attrs[i].index = i;
    ret = rknn_query(mobilefacenet_ctx, RKNN_QUERY_INPUT_ATTR, &(mobilefacenet_input_attrs[i]),sizeof(rknn_tensor_attr));
    if (ret != RKNN_SUCC)
    {
      printf("mobilefacenet rknn_query fail! ret=%d\n", ret);
      return NULL;
    }
    printRKNNTensor(&(mobilefacenet_input_attrs[i]));
  }

  // print mobilefacenet output tensor
  printf("mobilefacenet output tensors:\n");
  rknn_tensor_attr mobilefacenet_output_attrs[mobilefacenet_io_num.n_output];
  memset(mobilefacenet_output_attrs, 0, sizeof(mobilefacenet_output_attrs));
  for (unsigned int i = 0; i < mobilefacenet_io_num.n_output; i++)
  {
    mobilefacenet_output_attrs[i].index = i;
    ret = rknn_query(mobilefacenet_ctx, RKNN_QUERY_OUTPUT_ATTR, &(mobilefacenet_output_attrs[i]),sizeof(rknn_tensor_attr));
    if (ret != RKNN_SUCC)
    {
      printf("mobilefacenet rknn_query fail! ret=%d\n", ret);
      return NULL;
    }
    printRKNNTensor(&(mobilefacenet_output_attrs[i]));
  }

  // get mobilefacenet model's input image width and height
  int mobilefacenet_channel = 3;
  int mobilefacenet_width = 0;
  int mobilefacenet_height = 0;
  if (mobilefacenet_input_attrs[0].fmt == RKNN_TENSOR_NCHW)
  {
      printf("mobilefacenet model is NCHW input fmt\n");
      mobilefacenet_width = mobilefacenet_input_attrs[0].dims[0];
      mobilefacenet_height = mobilefacenet_input_attrs[0].dims[1];
  }
  else
  {
      printf("mobilefacenet model is NHWC input fmt\n");
      mobilefacenet_width = mobilefacenet_input_attrs[0].dims[1];
      mobilefacenet_height = mobilefacenet_input_attrs[0].dims[2];
  }
  printf("mobilefacenet model input height=%d, width=%d, channel=%d\n", mobilefacenet_height, mobilefacenet_width, mobilefacenet_channel);

  rga_context rga_ctx;
  drm_context drm_ctx;
  memset(&rga_ctx, 0, sizeof(rga_context));
  memset(&drm_ctx, 0, sizeof(drm_context));
  // DRM alloc buffer
  //int drm_fd = -1;
  //int buf_fd = -1; // converted from buffer handle
  //unsigned int handle;
  //size_t actual_size = 0;
  //void *drm_buf = NULL;
  //drm_fd = drm_init(&drm_ctx);//返回drm设备文件描述符
  //drm_buf = drm_buf_alloc(&drm_ctx, drm_fd, disp_width, disp_height, 3 * 8, &buf_fd, &handle, &actual_size);
  void *yolov5s_input_buf = malloc(yolov5s_height * yolov5s_width * yolov5s_channel);
  // init rga context
  RGA_init(&rga_ctx);

  std::vector<float> out_scales;
  std::vector<uint8_t> out_zps;
  for (int i = 0; i < yolov5s_io_num.n_output; ++i)
  {
    out_scales.push_back(yolov5s_output_attrs[i].scale);
    out_zps.push_back(yolov5s_output_attrs[i].zp);
  }
	//malloc分配pfpld输入数据
  void *pfpld_input_buf = malloc(pfpld_height * pfpld_width * pfpld_channel);
  while (!atk_face_recognition_quit) 
  {
    clock_gettime(CLOCK_MONOTONIC, &start);  
    MEDIA_BUFFER src_mb = NULL;
    frame_cnt += 1;
    frame_cnt_eye += 1;
    src_mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_RGA, 1, -1);//阻塞等待，返回缓冲区指针
    if (!src_mb) 
    {
      printf("ERROR: RK_MPI_SYS_GetMediaBuffer get null buffer!\n");
      break;
    }
    //memcpy(drm_buf, (uint8_t *)RK_MPI_MB_GetPtr(src_mb) , disp_width * disp_height * 3);
    img_resize_slow(&rga_ctx, (uint8_t *)RK_MPI_MB_GetPtr(src_mb), disp_width, disp_height, yolov5s_input_buf, yolov5s_width, yolov5s_height);

    // Set Input Data
    rknn_input yolov5s_inputs[1];
    memset(yolov5s_inputs, 0, sizeof(yolov5s_inputs));
    yolov5s_inputs[0].index = 0;
    yolov5s_inputs[0].type = RKNN_TENSOR_UINT8;
    yolov5s_inputs[0].size = yolov5s_height*yolov5s_width*yolov5s_channel;
    yolov5s_inputs[0].fmt = RKNN_TENSOR_NHWC;
    yolov5s_inputs[0].buf = yolov5s_input_buf;
    ret = rknn_inputs_set(yolov5s_ctx, yolov5s_io_num.n_input, yolov5s_inputs);
    if (ret < 0)
    {
      printf("ERROR: yolov5s rknn_inputs_set fail! ret=%d\n", ret);
      continue;
    }
    // yolov5s Run
    ret = rknn_run(yolov5s_ctx, nullptr);
    if (ret < 0)
    {
      printf("ERROR: yolov5s rknn_run fail! ret=%d\n", ret);
      continue;
    }

    // Get Output
    rknn_output yolov5s_outputs[yolov5s_io_num.n_output];
    memset(yolov5s_outputs, 0, sizeof(yolov5s_outputs));
    for (int i = 0; i < yolov5s_io_num.n_output; i++)
    {
      yolov5s_outputs[i].want_float = 0;
    }
    ret = rknn_outputs_get(yolov5s_ctx, yolov5s_io_num.n_output, yolov5s_outputs, NULL);
    if (ret < 0)
    {
      printf("ERROR: rknn_outputs_get fail! ret=%d\n", ret);
      continue;
    }

    const float vis_threshold = 0.2;
    const float nms_threshold = 0.3;
    const float conf_threshold = 0.4;
    float scale_w = (float)yolov5s_width / disp_width;
    float scale_h = (float)yolov5s_height / disp_height;
    detect_result_group_t detect_result_group;
    memset(&detect_result_group, 0, sizeof(detect_result_group));
    post_process((uint8_t *)yolov5s_outputs[0].buf, (uint8_t *)yolov5s_outputs[1].buf, (uint8_t *)yolov5s_outputs[2].buf, yolov5s_height, yolov5s_width,
                 conf_threshold, nms_threshold, vis_threshold, scale_w, scale_h, out_zps, out_scales, &detect_result_group);
    filter_largest_face_only(&detect_result_group);
    for (int i = 0; i < detect_result_group.count; i++)
    {
      detect_result_t *det_result = &(detect_result_group.results[i]);
      int left = det_result->box.left;
      int top = det_result->box.top;
      int right = det_result->box.right;
      int bottom = det_result->box.bottom;
//***********************************************************************************************************************************************************
      if(strcmp(det_result->name, "face") == 0)
      {
        
        //设置源数据裁剪区域
		    im_rect video_buf_rect;
		    get_aligned_crop_rect(det_result->box, disp_width, disp_height, pfpld_width, &video_buf_rect);
		
		    face_size=video_buf_rect.width;
		    face_xstart=video_buf_rect.x;
		    face_ystart=video_buf_rect.y;
		
		    //设置pfpld输入数据区域
		    im_rect pfpld_input_buf_rect = {0, 0, pfpld_width, pfpld_height};

        rgb24_resize((unsigned char *)RK_MPI_MB_GetPtr(src_mb),video_buf_rect,disp_width,disp_height,
                    (unsigned char *)pfpld_input_buf,pfpld_input_buf_rect,pfpld_width,pfpld_height);
		
        //set_input
        rknn_input pfpld_inputs[1];
        memset(pfpld_inputs, 0, sizeof(pfpld_inputs));
        pfpld_inputs[0].index = 0;
        pfpld_inputs[0].type = RKNN_TENSOR_UINT8;
        pfpld_inputs[0].size = pfpld_width * pfpld_height * pfpld_channel;
        pfpld_inputs[0].fmt = RKNN_TENSOR_NHWC;
        pfpld_inputs[0].buf = pfpld_input_buf;
        ret = rknn_inputs_set(pfpld_ctx, pfpld_io_num.n_input, pfpld_inputs);
        if(ret < 0)
        {
          printf("ERROR: rknn_inputs_set fail! ret=%d\n", ret);
          continue;
        }
        
        // Run
        ret = rknn_run(pfpld_ctx, nullptr);
        if (ret < 0)
        {
          printf("ERROR: rknn_run fail! ret=%d\n", ret);
          continue;
        }
        // Get Output
        rknn_output pfpld_outputs[pfpld_io_num.n_output];
        memset(pfpld_outputs, 0, sizeof(pfpld_outputs));
        for(int i = 0; i < pfpld_io_num.n_output; i++)
        {
          pfpld_outputs[i].want_float = 1;
        }
        ret = rknn_outputs_get(pfpld_ctx, pfpld_io_num.n_output, pfpld_outputs, NULL);
        if(ret < 0)
        {
          printf("ERROR: rknn_outputs_get fail! ret=%d\n", ret);
          continue;
        }
        get_result_hcb((float*)pfpld_outputs[1].buf, &res_landmk, (float*)pfpld_outputs[0].buf, &res_angle);
        nap_count = nap_test(res_landmk, res_angle, &frame_cnt_eye, &frame_cnt);
		
        // Set Input Data
        rknn_input mobilefacenet_inputs[1];
        memset(mobilefacenet_inputs, 0, sizeof(mobilefacenet_inputs));
        mobilefacenet_inputs[0].index = 0;
        mobilefacenet_inputs[0].type = RKNN_TENSOR_UINT8;
        mobilefacenet_inputs[0].size = mobilefacenet_width * mobilefacenet_height * mobilefacenet_channel;
        mobilefacenet_inputs[0].fmt = RKNN_TENSOR_NHWC;
        mobilefacenet_inputs[0].buf = pfpld_input_buf;
        ret = rknn_inputs_set(mobilefacenet_ctx, mobilefacenet_io_num.n_input, mobilefacenet_inputs);
        if(ret < 0)
        {
          printf("ERROR: mobilefacenet rknn_inputs_set fail! ret=%d\n", ret);
          continue;
        }
        
        ret = rknn_run(mobilefacenet_ctx, nullptr);
        if (ret < 0)
        {
          printf("ERROR: mobilefacenet rknn_run fail! ret=%d\n", ret);
          continue;
        }
  
        // Get Output
        rknn_output mobilefacenet_outputs[1];
        memset(mobilefacenet_outputs, 0, sizeof(mobilefacenet_outputs));
        for(int i = 0; i < mobilefacenet_io_num.n_output; i++)
        {
          mobilefacenet_outputs[i].want_float = 1;
        }
        ret = rknn_outputs_get(mobilefacenet_ctx, mobilefacenet_io_num.n_output, mobilefacenet_outputs, NULL);
        if(ret < 0)
        {
          printf("ERROR: mobilefacenet rknn_outputs_get fail! ret=%d\n", ret);
          continue;
        }
        float similarity = 0.0;
        float max_similarity = 0.0;
        std::string best_match = ""; // 用于存储相似度最高的 name

        for(database_iter = database_face_map.begin();database_iter != database_face_map.end();database_iter++) 
        {
          // 计算相似度
          similarity = cosine_similarity(database_iter->second.feature, (float*)mobilefacenet_outputs[0].buf, 512);
          //printf("%s simple_value = %f\n", database_iter->first.c_str(), similarity); // 使用 c_str() 将 std::string 转为 C 风格字符串
          // 如果当前相似度大于最大相似度，则更新最大相似度和对应的 name
          if(similarity > max_similarity) 
          {
            max_similarity = similarity;
            best_match = database_iter->first;
          }
        }
        // 遍历完成后，判断最大相似度是否符合条件
        if(max_similarity >= 0.6) 
        {
          face_name = best_match;
        }
        else
        {
          face_name = "";
          //printf("No match found with similarity >= 0.5\n");
        }
        rknn_outputs_release(pfpld_ctx, pfpld_io_num.n_output, pfpld_outputs);
        rknn_outputs_release(mobilefacenet_ctx, mobilefacenet_io_num.n_output, mobilefacenet_outputs);
        
      }//end "if(strcmp(det_result->name, "face") == 0)"
//***********************************************************************************************************************************************************
      using namespace cv;
      Mat orig_img = Mat(disp_height, disp_width, CV_8UC3, RK_MPI_MB_GetPtr(src_mb));//黑白灰图案
      // 采用opencv来绘制矩形框,颜色格式是B、G、R
      cv::rectangle(orig_img,cv::Point(left, top),cv::Point(right, bottom),cv::Scalar(0,255,255),5,8,0);
      putText(orig_img, detect_result_group.results[i].name, Point(left, top-16), FONT_HERSHEY_TRIPLEX, 1, Scalar(0,0,255),2,8,0); 
    }//end "for (int i = 0; i < detect_result_group.count; i++)"

    rknn_outputs_release(yolov5s_ctx, yolov5s_io_num.n_output, yolov5s_outputs);
	  clock_gettime(CLOCK_MONOTONIC, &end);
    fps = 1/((end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1e9);
    using namespace cv;
    Mat orig_img = Mat(disp_height, disp_width, CV_8UC3, RK_MPI_MB_GetPtr(src_mb));//黑白灰图案
    // 采用opencv来绘制矩形框,颜色格式是B、G、R
	  putText(orig_img, "fps:", cv::Point(50, 1050), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 4, 8, false);
    putText(orig_img, std::to_string(fps), cv::Point(150, 1050), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 4, 8, false);
	  putText(orig_img, "state:", cv::Point(50, 1100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 4, 8, false);
    putText(orig_img, std::to_string(nap_count), cv::Point(150, 1100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 4, 8, false);
  	putText(orig_img, "name:", cv::Point(50, 1150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 4, 8, false);
    putText(orig_img,  face_name, cv::Point(150, 1150), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 4, 8, false);   
	  
    //RK_MPI_SYS_SendMediaBuffer(RK_ID_VO, 0, src_mb);
    RK_MPI_SYS_SendMediaBuffer(RK_ID_RGA, 0, src_mb);
    RK_MPI_MB_ReleaseBuffer(src_mb);
  }//end "while (!atk_face_recognition_quit)"

  free(yolov5s_input_buf);
  free(pfpld_input_buf);
  //drm_buf_destroy(&drm_ctx, drm_fd, buf_fd, handle, drm_buf, actual_size);
  drm_deinit(&drm_ctx, drm_fd);
  RGA_deinit(&rga_ctx);
  if(yolov5s) 
   {
     free(yolov5s);
     yolov5s = NULL;
   }
  if(pfpld) 
  {
     free(pfpld);
     pfpld = NULL;
  }
  if(mobilefacenet) 
  {
     free(mobilefacenet);
     mobilefacenet = NULL;
  }
  rknn_destroy(yolov5s_ctx);
  rknn_destroy(pfpld_ctx);
  rknn_destroy(mobilefacenet_ctx);  
  return NULL;
}

static void printRKNNTensor(rknn_tensor_attr *attr)
{
  printf("index=%d name=%s n_dims=%d dims=[%d %d %d %d] n_elems=%d size=%d "
         "fmt=%d type=%d qnt_type=%d fl=%d zp=%d scale=%f\n",
         attr->index, attr->name, attr->n_dims, attr->dims[3], attr->dims[2],
         attr->dims[1], attr->dims[0], attr->n_elems, attr->size, 0, attr->type,
         attr->qnt_type, attr->fl, attr->zp, attr->scale);
}


static unsigned char *load_model(const char *filename, int *model_size)
{
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
  {
    printf("fopen %s fail!\n", filename);
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  unsigned int model_len = ftell(fp);
  unsigned char *model = (unsigned char *)malloc(model_len);
  fseek(fp, 0, SEEK_SET);

  if (model_len != fread(model, 1, model_len, fp))
  {
    printf("fread %s fail!\n", filename);
    free(model);
    return NULL;
  }
  *model_size = model_len;

  if (fp)
  {
    fclose(fp);
  }
  return model;
}
