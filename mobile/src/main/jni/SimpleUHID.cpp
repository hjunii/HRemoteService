#include "SimpleUHID.h"

#include <android/log.h>
#include <stdio.h>
#include <linux/input.h>
#include <linux/uhid.h>
#include <fcntl.h>

#define  LOGV(...) \
           __android_log_print(ANDROID_LOG_VERBOSE, "NativeSimpleUHID", __VA_ARGS__)

/* HID Report Desciptor
 * We emulate a basic 3 button mouse with wheel. This is the report-descriptor
 * as the kernel will parse it:
 *
 * INPUT[INPUT]
 *   Field(0)
 *     Physical(GenericDesktop.Pointer)
 *     Application(GenericDesktop.Mouse)
 *     Usage(3)
 *       Button.0001
 *       Button.0002
 *       Button.0003
 *     Logical Minimum(0)
 *     Logical Maximum(1)
 *     Report Size(1)
 *     Report Count(3)
 *     Report Offset(0)
 *     Flags( Variable Absolute )
 *   Field(1)
 *     Physical(GenericDesktop.Pointer)
 *     Application(GenericDesktop.Mouse)
 *     Usage(3)
 *       GenericDesktop.X
 *       GenericDesktop.Y
 *       GenericDesktop.Wheel
 *     Logical Minimum(-128)
 *     Logical Maximum(127)
 *     Report Size(8)
 *     Report Count(3)
 *     Report Offset(8)
 *     Flags( Variable Relative )
 *
 * This is the mapping that we expect:
 *   Button.0001 ---> Key.LeftBtn
 *   Button.0002 ---> Key.RightBtn
 *   Button.0003 ---> Key.MiddleBtn
 *   GenericDesktop.X ---> Relative.X
 *   GenericDesktop.Y ---> Relative.Y
 *   GenericDesktop.Wheel ---> Relative.Wheel
 *
 * This information can be verified by reading /sys/kernel/debug/hid/<dev>/rdesc
 * This file should print the same information as showed above.
 */

static unsigned char rdesc[] = {
        0x05, 0x01, 0x09, 0x02, 0xa1, 0x01, 0x09, 0x01,
        0xa1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29, 0x03,
        0x15, 0x00, 0x25, 0x01, 0x95, 0x03, 0x75, 0x01,
        0x81, 0x02, 0x95, 0x01, 0x75, 0x05, 0x81, 0x01,
        0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x09, 0x38,
        0x15, 0x80, 0x25, 0x7f, 0x75, 0x08, 0x95, 0x03,
        0x81, 0x06, 0xc0, 0xc0,
};

static void throw_exception(JNIEnv * env, const char * msg)
{
    jclass jc = env->FindClass( "java/io/IOException");
    env->ThrowNew(jc, msg);
    env->DeleteLocalRef(jc);
}

static int uhid_write(int fd, const struct uhid_event *ev)
{
    ssize_t ret;

    ret = write(fd, ev, sizeof(*ev));
    if (ret < 0) {
        return -errno;
    } else if (ret != sizeof(*ev)) {
        return -EFAULT;
    } else {
        return 0;
    }
}

JNIEXPORT jint JNICALL Java_com_android_hremoteservice_SimpleUHID_open(JNIEnv *env, jobject obj)
{
    int fd = -1;
    struct uhid_event ev;;
    char err_msg[255];

    fd = open("/dev/uhid", O_RDWR | O_CLOEXEC);
    if (fd < 0)
    {
        sprintf(err_msg, "can't open /dev/uhid - %s", strerror(errno));
        throw_exception(env, err_msg);
        return -1;
    }

    memset(&ev, 0, sizeof(ev));
    ev.type = UHID_CREATE;
    strcpy((char*)ev.u.create.name, "uhid-hremoteservice");
    ev.u.create.rd_data = rdesc;
    ev.u.create.rd_size = sizeof(rdesc);
    ev.u.create.bus = BUS_USB;
    ev.u.create.vendor = 0x15d9;
    ev.u.create.product = 0x0a37;
    ev.u.create.version = 0;
    ev.u.create.country = 0;

    if (uhid_write(fd, &ev))
    {
        sprintf(err_msg, "open: can't write /dev/uhid - %s", strerror(errno));
        throw_exception(env, err_msg);
        return -1;
    }

    return (jint) fd;
}

JNIEXPORT void JNICALL Java_com_android_hremoteservice_SimpleUHID_close(JNIEnv *env, jobject obj, jint fd)
{
    char err_msg[255];
    struct uhid_event ev;

    if (fd < 0)
        return;

    memset(&ev, 0, sizeof(ev));
    ev.type = UHID_DESTROY;

    if (uhid_write(fd, &ev))
    {
        sprintf(err_msg, "close: can't write /dev/uhid - %s", strerror(errno));
        throw_exception(env, err_msg);;
    }
    close(fd);
}

JNIEXPORT void JNICALL Java_com_android_hremoteservice_SimpleUHID_sendEvent(JNIEnv * env, jobject obj, jint fd, jboolean btn1Down, jboolean btn2Down, jboolean btn3Down, jshort absX, jshort absY, jshort wheel)
{
    char err_msg[255];
    struct uhid_event ev;

    if (fd >= 0)
    {
        memset(&ev, 0, sizeof(ev));

        ev.type = UHID_INPUT;
        ev.u.input.size = 4;

        if (btn1Down)
            ev.u.input.data[0] |= 0x1;
        if (btn2Down)
            ev.u.input.data[0] |= 0x2;
        if (btn3Down)
            ev.u.input.data[0] |= 0x4;

        ev.u.input.data[1] = absX;
        ev.u.input.data[2] = absY;
        ev.u.input.data[3] = wheel;

        if (uhid_write(fd, &ev))
        {
            sprintf(err_msg, "sendEvent: can't write /dev/uhid - %s - %s", strerror(errno));
            throw_exception(env, err_msg);;
        }
    }
}