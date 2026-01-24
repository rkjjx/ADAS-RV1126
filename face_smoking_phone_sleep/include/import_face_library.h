#ifndef _IMPORT_FACE_LIBRARY_H
#define _IMPORT_FACE_LIBRARY_H


#include "face_database.h"
#include "rknn_api.h"
#define FEATURE_SIZE 512



int run_face_recognize(const char *name, unsigned char *in_image);

#endif
