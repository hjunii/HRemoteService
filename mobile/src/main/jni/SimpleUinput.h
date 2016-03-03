#include <jni.h>

#ifndef HREMOTESERVICE_SIMPLEUINPUT_H
#define HREMOTESERVICE_SIMPLEUINPUT_H
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL Java_com_android_hremoteservice_SimpleUinput_openUinput(JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_com_android_hremoteservice_SimpleUinput_closeUinput(JNIEnv*, jobject, jint);

JNIEXPORT void JNICALL Java_com_android_hremoteservice_SimpleUinput_send(JNIEnv*, jobject, jint, jshort, jshort, jint);

#ifdef __cplusplus
}
#endif;

#endif //HREMOTESERVICE_SIMPLEUINPUT_H
