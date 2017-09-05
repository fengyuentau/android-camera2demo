#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Camera2Demo", __VA_ARGS__)

//converting Y Plane from YUV_420_888 to RGBA


//converting YUV_420_888 to RGBA
/*
void YUVtoRGBA(jbyte* Y_jb, jbyte* U_jb, jbyte* V_jb, jint width, jint height, jint* rgb)
{
    int i;
    int j;

    int Y;
    int U = 0;
    int V = 0;

    int R = 0;
    int G = 0;
    int B = 0;

    int w = width;
    int h = height;
    int Y_size = w * h;

    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {

        }
    }

}
*/

extern "C" {
JNIEXPORT jstring JNICALL Java_tau_camera2demo_JNIUtils_display(
        JNIEnv *env,
        jobject obj,
        jint srcWidth,
        jint srcHeight,
        jint rowStride,
        jobject srcBuffer,
        jobject surface) {

    uint8_t *srcLumaPtr = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(srcBuffer));
    /*
    if (srcLumaPtr == nullptr) {
        LOGE("srcLumaPtr null ERROR!");
        return NULL;
    }
    */

    ANativeWindow * window = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_acquire(window);
    ANativeWindow_Buffer buffer;
    //set output size and format
    //only 3 formats are available:
    //WINDOW_FORMAT_RGBA_8888(DEFAULT), WINDOW_FORMAT_RGBX_8888, WINDOW_FORMAT_RGB_565
    ANativeWindow_setBuffersGeometry(window, 0, 0, WINDOW_FORMAT_RGBA_8888);
    if (int32_t err = ANativeWindow_lock(window, &buffer, NULL)) {
        LOGE("ANativeWindow_lock failed with error code: %d\n", err);
        ANativeWindow_release(window);
        return NULL;
    }

    //to display grayscale, first convert the Y plane from YUV_420_888 to RGBA
    //ANativeWindow_Buffer buffer;
    uint8_t * outPtr = reinterpret_cast<uint8_t *>(buffer.bits);
    for (size_t y = 0; y < srcHeight; y++)
    {
        uint8_t * rowPtr = srcLumaPtr + y * rowStride;
        for (size_t x = 0; x < srcWidth; x++)
        {
            *(outPtr++) = *rowPtr;
            *(outPtr++) = *rowPtr;
            *(outPtr++) = *rowPtr;
            *(outPtr++) = 255; // gamma for RGBA_8888
            ++rowPtr;
        }
    }


    //memcpy(buffer.bits, srcLumaPtr, srcWidth * srcHeight * 1.5);


    ANativeWindow_unlockAndPost(window);
    ANativeWindow_release(window);

    return NULL;
}


JNIEXPORT jstring JNICALL Java_tau_camera2demo_JNIUtils_display2(
        JNIEnv *env,
        jobject obj,
        jint srcWidth,
        jint srcHeight,
        jbyteArray Y,
        jbyteArray U,
        jbyteArray V,
        jobject surface) {

/*
    uint8_t *srcLumaPtr = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(srcData));

    if (srcData) {
        LOGE("srcLumaPtr null ERROR!");
        return NULL;
    }
    jbyte* y = (jbyte*) (*env)->GetPrimitiveArrayCritical(env, Y, 0);
    jbyte* u = (jbyte*) (*env)->GetPrimitiveArrayCritical(env, U, 0);
    jbyte* v = (jbyte*) (*env)->GetPrimitiveArrayCritical(env, V, 0);
*/

    ANativeWindow * window = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_acquire(window);

    ANativeWindow_Buffer buffer;

    ANativeWindow_setBuffersGeometry(window, srcWidth, srcHeight, 0/* format unchanged */);

    if (int32_t err = ANativeWindow_lock(window, &buffer, NULL)) {
        LOGE("ANativeWindow_lock failed with error code: %d\n", err);
        ANativeWindow_release(window);
        return NULL;
    }

    LOGE("srcWidth: %d\n", srcWidth);
    LOGE("srcHeight: %d\n", srcHeight);

    memcpy(buffer.bits, Y, srcWidth * srcHeight);


    ANativeWindow_unlockAndPost(window);
    ANativeWindow_release(window);

    return NULL;
}
}

