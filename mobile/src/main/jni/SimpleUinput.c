#include "SimpleUinput.h"

#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>

static void throw_exception(JNIEnv * env, const char * msg)
{
    jclass jc = (*env)->FindClass(env,  "java/io/IOException");
    (*env)->ThrowNew(env, jc, msg);
}

JNIEXPORT jint JNICALL Java_com_android_hremoteservice_SimpleUinput_openUinput (JNIEnv *env, jobject obj)
{
    int fd;
    struct uinput_user_dev uidev;

    fd = open("dev/uinput",  O_WRONLY | O_NONBLOCK);
    if (fd < 0)
        throw_exception(env, "can't open /dev/uinput");

    if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        throw_exception(env, "error: ioctl EV_KEY");

    if(ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
        throw_exception(env, "error: ioctl EV_LEFT");

    if(ioctl(fd, UI_SET_EVBIT, EV_ABS) < 0)
        throw_exception(env, "error: ioctl EV_ABS");

    if(ioctl(fd, UI_SET_ABSBIT, REL_X) < 0)
        throw_exception(env, "error: ioctl ABS_X");

    if(ioctl(fd, UI_SET_ABSBIT, REL_Y) < 0)
        throw_exception(env, "error: ioctl ABS_Y");

    if(ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
        throw_exception(env, "error: ioctl EV_REL");

    if(ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
        throw_exception(env, "error: ioctl REL_X");

    if(ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
        throw_exception(env, "error: ioctl REL_Y");

    memset(&uidev, 0, sizeof(uidev));

    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-hremoteservice");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    if (write(fd, &uidev, sizeof(uidev)) < 0)
        throw_exception(env, "error: write uidev");

    if (ioctl(fd, UI_DEV_CREATE) < 0)
        throw_exception(env, "error: ioctl UI_DEV_CREATE");

    return (jint) fd;
}

JNIEXPORT void JNICALL Java_om_android_hremoteservice_SimpleUinput_closeUinput(JNIEnv *env, jobject obj, jint fd)
{
    if (fd < 0)
        throw_exception(env, "invalid fd");
    close(fd);
}

JNIEXPORT void JNICALL Java_om_android_hremoteservice_SimpleUinput_send(JNIEnv * env, jobject obj, jint fd, jshort type, jshort code, jint value)
{
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