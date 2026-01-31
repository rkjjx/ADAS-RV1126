#!/bin/bash
cp -p /home/alientek/ADAS/face_smoking_phone_sleep/build/libatk_face_recognition.so /opt/atk-dlrv1126-toolchain/arm-buildroot-linux-gnueabihf/sysroot/usr/lib/libatk_face_recognition.so
adb push /home/alientek/ADAS/face_smoking_phone_sleep/build/libatk_face_recognition.so /usr/lib/
adb push /home/alientek/ADAS/face_smoking_phone_sleep/build/import_face_library /demo/bin


