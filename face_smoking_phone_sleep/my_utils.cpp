// Copyright (c) 2021 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "./include/my_utils.h"
#include "atk_rockx_face_recognition.h"




float cosine_similarity(const float* vec1, const float* vec2, int size) {
    float magnitude1 = 0.0f, magnitude2 = 0.0f;
    float dot_product = 0.0f;

    // 计算点积和模
    for (int i = 0; i < size; ++i) {
        dot_product += vec1[i] * vec2[i];
        magnitude1 += vec1[i] * vec1[i];
        magnitude2 += vec2[i] * vec2[i];
    }

    magnitude1 = sqrtf(magnitude1);
    magnitude2 = sqrtf(magnitude2);

    const float epsilon = 1e-10f;
    if (magnitude1 < epsilon || magnitude2 < epsilon) {
        return 0.0f;  // 至少一个向量接近零，无法计算相似度
    }

    // 返回余弦相似度
    return dot_product / (magnitude1 * magnitude2);
}

FaceRecognitionFrame *GetFace() {
    MEDIA_BUFFER src_mb = NULL;
    src_mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_RGA, 1, -1);
    if (!src_mb) {
        printf("RK_MPI_SYS_GetMediaBuffer get null buffer!\n");
        return NULL;
    }

    FaceRecognitionFrame *frame = (FaceRecognitionFrame*)malloc(sizeof(FaceRecognitionFrame));
    if (!frame) {
        RK_MPI_MB_ReleaseBuffer(src_mb);
        return NULL;
    }

    unsigned char *face_buf = (unsigned char *)malloc(112 * 112 * 3);
    if (!face_buf) {
        RK_MPI_MB_ReleaseBuffer(src_mb);
        free(frame);
        return NULL;
    }

    int ret = rgb24_resize((unsigned char *)RK_MPI_MB_GetPtr(src_mb),{face_xstart, face_ystart, face_size, face_size},disp_width,disp_height,
                    (unsigned char *)face_buf,{0, 0, 112, 112},112,112);
    RK_MPI_MB_ReleaseBuffer(src_mb);
    if (ret != 0) {
        free(face_buf);
        free(frame);
        return NULL;
    }

    frame->file = face_buf;
    frame->size = 112 * 112 * 3;

    return frame;
}

void filter_largest_face_only(detect_result_group_t *group)
{
    if (group->count <= 0) return;

    int max_face_idx = -1;
    long max_area = -1;

    // ---------------------------------------------------------
    // 第一步：寻找面积最大的 "face"
    // ---------------------------------------------------------
    for (int i = 0; i < group->count; i++)
    {
        // 检查是否为 "face"
        if (strcmp(group->results[i].name, "face") == 0)
        {
            // 计算面积：(right - left) * (bottom - top)
            int width = group->results[i].box.right - group->results[i].box.left;
            int height = group->results[i].box.bottom - group->results[i].box.top;

            // 防止负数
            if (width < 0) width = 0;
            if (height < 0) height = 0;

            long current_area = width * height;

            // 更新最大值
            if (current_area > max_area)
            {
                max_area = current_area;
                max_face_idx = i;
            }
        }
    }

    // 如果没有找到任何人脸，直接返回，不需要修改
    if (max_face_idx == -1)
    {
        return;
    }

    // ---------------------------------------------------------
    // 第二步：数组紧缩 (移除多余的 face)
    // ---------------------------------------------------------
    int keep_count = 0; // 写入指针

    for (int i = 0; i < group->count; i++) // 读取指针
    {
        int is_face = (strcmp(group->results[i].name, "face") == 0);

        // 保留条件的逻辑：
        // 1. 如果它不是 face -> 保留
        // 2. 如果它是 face，且它的索引等于我们找到的最大索引 -> 保留
        if (!is_face || (is_face && i == max_face_idx))
        {
            // 如果读取位置和写入位置不同，说明前面有被删除的元素，需要移动数据
            if (i != keep_count)
            {
                group->results[keep_count] = group->results[i];
            }
            keep_count++;
        }
        // else: 是 face 但不是最大的 -> 此时 keep_count 不增加，数据将在下一次循环被覆盖
    }

    // 更新最终的有效数量
    group->count = keep_count;
}

int get_aligned_crop_rect(BOX_RECT obj_box, int img_w, int img_h, int align, im_rect *out_rect)
{
    // 1. 计算原始宽高
    int w = obj_box.right - obj_box.left;
    int h = obj_box.bottom - obj_box.top;

    // 2. 确定基准边长 (取长边并扩充1.1倍)
    int size = (int)(MAX(w, h) * 1.1f);

    // 4. 安全检查：如果扩充后比原图还大，只能被迫缩小 (向下取整)
    if (size > MIN(img_w, img_h)) {
        size = (MIN(img_w, img_h) / align) * align;
    }

    if (size == 0) return 0; // 失败

    // 5. 计算中心点
    int cx = (obj_box.left + obj_box.right) / 2;
    int cy = (obj_box.top + obj_box.bottom) / 2;

    // 6. 算出左上角 (暂不考虑边界)
    int x = cx - size / 2;
    int y = cy - size / 2;

    // 7. 平移逻辑 (Shift)：把框推回图像内
    // 先限制右下角，防止溢出
    if (x + size > img_w) x = img_w - size;
    if (y + size > img_h) y = img_h - size;

    // 再限制左上角，防止负数 (优先级更高，因为size已经保证小于img了，这里只是为了兜底)
    x = MAX(x, 0); // 或者你的业务要求 x > 2
    y = MAX(y, 0);

    // 赋值输出
    out_rect->x = x;
    out_rect->y = y;
    out_rect->width = size;
    out_rect->height = size;

    return 1;
}


int rgb24_resize(unsigned char *input_rgb,im_rect src_rect,int input_width,int input_height,
                       unsigned char *output_rgb,im_rect dst_rect,int output_width,int output_height)
{
  rga_buffer_t src = wrapbuffer_virtualaddr(input_rgb, input_width, input_height, RK_FORMAT_RGB_888);
  rga_buffer_t dst = wrapbuffer_virtualaddr(output_rgb, output_width, output_height, RK_FORMAT_RGB_888);
  rga_buffer_t pat = {0};
  im_rect pat_rect = {0};
  IM_STATUS STATUS = improcess(src, dst, pat, src_rect, dst_rect, pat_rect, 0);
  if (STATUS != IM_STATUS_SUCCESS)
  {
    printf("rga imcrop and resize failed: %s\n", imStrError(STATUS));
    return -1;
  }
  return 0;
}

FaceRecognitionFrame *GetFaceRecognitionMediaBuffer() {
    MEDIA_BUFFER src_mb = NULL;
    src_mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_RGA, 0, -1);
    if (!src_mb) {
        printf("RK_MPI_SYS_GetMediaBuffer get null buffer!\n");
        return NULL;
    }

    FaceRecognitionFrame *frame = (FaceRecognitionFrame*)malloc(sizeof(FaceRecognitionFrame));
    if (!frame) {
        RK_MPI_MB_ReleaseBuffer(src_mb);
        return NULL;
    }

    frame->size = RK_MPI_MB_GetSize(src_mb);
    frame->file = malloc(frame->size);
    if (!frame->file) {
        RK_MPI_MB_ReleaseBuffer(src_mb);
        free(frame);
        return NULL;
    }

    memcpy(frame->file, RK_MPI_MB_GetPtr(src_mb), frame->size);
    RK_MPI_MB_ReleaseBuffer(src_mb);
    return frame;
}






































