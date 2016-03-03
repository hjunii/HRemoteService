#include "SimpleUinput.h"

JNIEXPORT jint JNICALL Java_com_android_hremoteservice_SimpleUinput_openUinput (JNIEnv *env, jobject obj)
{
    return 0;
}

JNIEXPORT void JNICALL Java_om_android_hremoteservice_SimpleUinput_closeUinput(JNIEnv *env, jobject obj, jint fd)
{
}

JNIEXPORT void JNICALL Java_om_android_hremoteservice_SimpleUinput_send(JNIEnv * env, jobject obj, jint fd, jshort type, jshort code, jint value)
{
}
