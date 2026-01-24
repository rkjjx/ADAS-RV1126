#ifndef __MY_UTILS_H__
#define __MY_UTILS_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include "im2d.h"
#include "postprocess.h"
typedef struct {
  void *file;
  size_t size;
}FaceRecognitionFrame;


extern int disp_width;
extern int disp_height;

float cosine_similarity(const float* vec1, const float* vec2, int size);
FaceRecognitionFrame *GetFace();
void filter_largest_face_only(detect_result_group_t *group);
int get_aligned_crop_rect(BOX_RECT obj_box, int img_w, int img_h, int align, im_rect *out_rect);
int rgb24_resize(unsigned char *input_rgb,im_rect src_rect,int input_width,int input_height,
						   unsigned char *output_rgb,im_rect dst_rect,int output_width,int output_height);
FaceRecognitionFrame *GetFaceRecognitionMediaBuffer();


#endif /*__MY_UTILS_H__*/
