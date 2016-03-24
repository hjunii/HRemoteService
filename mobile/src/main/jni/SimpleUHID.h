#include <jni.h>

#ifndef HREMOTESERVICE_SIMPLEUHID_H
#define HREMOTESERVICE_SIMPLEUHID_H
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL Java_com_android_hremoteservice_SimpleUHID_open(JNIEnv *env, jobject obj);

JNIEXPORT void JNICALL Java_com_android_hremoteservice_SimpleUHID_close(JNIEnv *env, jobject obj, jint fd);

JNIEXPORT void JNICALL Java_com_android_hremoteservice_SimpleUHID_sendEvent(JNIEnv * env, jobject obj, jint fd, jboolean btn1Down, jboolean btn2Down, jboolean btn3Down, jshort absX, jshort absY, jshort wheel);

#ifdef __cplusplus
}
#endif

#endif //HREMOTESERVICE_SIMPLEUHID_H
