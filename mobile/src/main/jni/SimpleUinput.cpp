#include "SimpleUinput.h"

#include <android/log.h>
#include <stdio.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>

#define X_AXIS_MAX             768
#define X_AXIS_MIN              0
#define Y_AXIS_MAX             1280
#define Y_AXIS_MIN              0
#define PRESSURE_MIN         0
#define PRESSURE_MAX        15000

#define  LOGV(...) \
           __android_log_print(ANDROID_LOG_VERBOSE, "NativeSimpleUinput", __VA_ARGS__)

static void throw_exception(JNIEnv * env, const char * msg)
{
    jclass jc = env->FindClass( "java/io/IOException");
    env->ThrowNew(jc, msg);
    env->DeleteLocalRef(jc);
}

JNIEXPORT jint JNICALL Java_com_android_hremoteservice_SimpleUinput_openUinput(JNIEnv *env, jobject obj)
{
    int fd = -1;
    struct uinput_user_dev uidev;
    char err_msg[255];

    fd = open("/dev/uinput",  O_WRONLY | O_NONBLOCK);
    LOGV("openUinput0 fd = %d", fd);
    if (fd < 0)
    {
        sprintf(err_msg, "can't open /dev/uinput - %s", strerror(errno));
        LOGV("%s", err_msg);
        throw_exception(env, err_msg);
        return -1;
    }

    LOGV("openUinput fd = %d", fd);

    memset(&uidev, 0, sizeof(uidev));

    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-hremoteservice");
    uidev.id.bustype = BUS_VIRTUAL;
    uidev.id.vendor  = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    uidev.absmax[ABS_X] = X_AXIS_MAX;
    uidev.absmin[ABS_X] = X_AXIS_MIN;
    uidev.absmax[ABS_Y] = Y_AXIS_MAX;
    uidev.absmin[ABS_Y] = Y_AXIS_MIN;
    //uidev.absmax[ABS_PRESSURE] = PRESSURE_MAX;
    //uidev.absmin[ABS_PRESSURE] = PRESSURE_MIN;

    if (write(fd, &uidev, sizeof(uidev)) < 0)
        throw_exception(env, "error: write uidev");

    if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        throw_exception(env, "error: ioctl EV_KEY");

    if (ioctl(fd, UI_SET_EVBIT, EV_ABS) < 0)
        throw_exception(env, "error: ioctl EV_ABS");

    if (ioctl(fd, UI_SET_EVBIT, EV_SYN) < 0)
        throw_exception(env, "error: ioctl EV_SYN");

    if (ioctl(fd, UI_SET_ABSBIT, ABS_X) < 0)
        throw_exception(env, "error: ioctl ABS_X");

    if (ioctl(fd, UI_SET_ABSBIT, ABS_Y) < 0)
        throw_exception(env, "error: ioctl ABS_Y");

#if 0
    if (ioctl(fd, UI_SET_ABSBIT, ABS_PRESSURE) < 0)
        throw_exception(env, "error: ioctl ABS_PRESSURE");

    if (ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
        throw_exception(env, "error: ioctl EV_LEFT");

    if (ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT) < 0)
        throw_exception(env, "error: ioctl EV_RIGHT");
#endif

//    if(ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
//        throw_exception(env, "error: ioctl EV_REL");

//    if(ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
//        throw_exception(env, "error: ioctl REL_X");

//    if(ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
//        throw_exception(env, "error: ioctl REL_Y");

    LOGV("openUinput2 fd = %d", fd);

    if (ioctl(fd, UI_DEV_CREATE) < 0)
    {
        sprintf(err_msg, "error: ioctl UI_DEV_CREATE - %s", strerror(errno));
        throw_exception(env, err_msg);;
    }

    return (jint) fd;
}

JNIEXPORT void JNICALL Java_com_android_hremoteservice_SimpleUinput_closeUinput(JNIEnv *env, jobject obj, jint fd)
{
    LOGV("closeUinput");
    if (fd < 0)
        throw_exception(env, "invalid fd");
    close(fd);
}

JNIEXPORT void JNICALL Java_com_android_hremoteservice_SimpleUinput_send(JNIEnv * env, jobject obj, jint fd, jshort type, jshort code, jint value)
{
    LOGV("send");
    if (fd >= 0)
    {
        struct input_event event;
        memset(&event, 0, sizeof(event));
        gettimeofday(&event.time, 0);
        event.type = type;
        event.code = code;
        event.value = value;

        ssize_t bytes;
        bytes = write(fd, &event, sizeof(struct input_event));
        if (bytes != sizeof(struct input_event))
            throw_exception(env, "can't write /udev/uinput");
    }
}