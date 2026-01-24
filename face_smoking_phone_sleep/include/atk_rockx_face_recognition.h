#ifndef _ATK_ROCKX_FACE_RECOGNITION_H
#define _ATK_ROCKX_FACE_RECOGNITION_H
#include <math.h> 
#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <unordered_map>

#include "im2d.h"
#include "rga.h"
#include "rockx.h"
#include "rkmedia_api.h"
#include "sample_common.h"
#include "face_database.h"
#include "rga_func.h"
#include "drm_func.h"
#include "opencv2/opencv.hpp"
#include "opencv2/freetype.hpp"
#include "rknn_api.h"
#include "postprocess.h"
#include "nap_test.h"
#include "my_utils.h"

#define True 1
#define False 0

extern bool atk_face_recognition_quit;
extern int face_size;
extern int face_xstart;
extern int face_ystart;
extern RK_U32 video_width;
extern RK_U32 video_height;

int atk_recognition_init();
void *rkmedia_vi_face_thread(void *args);
static void printRKNNTensor(rknn_tensor_attr *attr);
static unsigned char *load_model(const char *filename, int *model_size);
#endif
