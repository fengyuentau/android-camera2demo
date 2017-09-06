#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Camera2Demo", __VA_ARGS__)

const uint8_t NUM_128 = 128;
const uint8_t NUM_255 = 255;

//convert Y Plane from YUV_420_888 to RGBA and display
extern "C" {
JNIEXPORT void JNICALL Java_tau_camera2demo_JNIUtils_GrayscaleDisplay(
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
    }

    //to display grayscale, first convert the Y plane from YUV_420_888 to RGBA
    //ANativeWindow_Buffer buffer;
    uint8_t * outPtr = reinterpret_cast<uint8_t *>(buffer.bits);
    for (size_t y = 0; y < srcHeight; y++)
    {
        uint8_t * rowPtr = srcLumaPtr + y * rowStride;
        for (size_t x = 0; x < srcWidth; x++)
        {
            //for grayscale output, just duplicate the Y channel into R, G, B channels
            *(outPtr++) = *rowPtr; //R
            *(outPtr++) = *rowPtr; //G
            *(outPtr++) = *rowPtr; //B
            *(outPtr++) = 255; // gamma for RGBA_8888
            ++rowPtr;
        }
    }

    ANativeWindow_unlockAndPost(window);
    ANativeWindow_release(window);
}


JNIEXPORT void JNICALL Java_tau_camera2demo_JNIUtils_RGBADisplay(
        JNIEnv *env,
        jobject obj,
        jint srcWidth,
        jint srcHeight,
        jint Y_rowStride,
        jobject Y_Buffer,
        jint U_rowStride,
        jobject U_Buffer,
        jint V_rowStride,
        jobject V_Buffer,
        jobject surface) {


    uint8_t *srcYPtr = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(Y_Buffer));
    uint8_t *srcUPtr = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(U_Buffer));
    uint8_t *srcVPtr = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(V_Buffer));
/*
    if (srcYPtr == nullptr)
    {
        LOGE("srcYPtr null ERROR!");
        return;
    }
    else if (srcUPtr == nullptr)
    {
        LOGE("srcUPtr null ERROR!");
        return;
    }
    else if (srcVPtr == nullptr)
    {
        LOGE("srcVPtr null ERROR!");
        return;
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
    }

    //convert YUV_420_888 to RGBA_888 and display
    uint8_t * outPtr = reinterpret_cast<uint8_t *>(buffer.bits);
    for (size_t y = 0; y < srcHeight; y++)
    {
        uint8_t * Y_rowPtr = srcYPtr + y * Y_rowStride;
        uint8_t * U_rowPtr = srcUPtr + (y >> 1) * U_rowStride;
        uint8_t * V_rowPtr = srcVPtr + (y >> 1) * V_rowStride;
        for (size_t x = 0; x < srcWidth; x++)
        {
            //from Wikipedia article YUV:
            //Integer operation of ITU-R standard for YCbCr(8 bits per channel) to RGB888
            //Y-Y, U-Cb, V-Cr
            //R = Y + V + (V >> 2) + (V >> 3) + (V >> 5);
            //G = Y - ((U >> 2) + (U >> 4) + (U >> 5)) - ((V >> 1) + (V >> 3) + (V >> 4) + (V >> 5));
            //B = Y + U + (U >> 1) + (U >> 2) + (U >> 6);
            uint8_t Y = Y_rowPtr[x];
            uint8_t U = U_rowPtr[(x >> 1)] - NUM_128;
            uint8_t V = V_rowPtr[(x >> 1)] - NUM_128;
            *(outPtr++) = Y + V + (V >> 2) + (V >> 3) + (V >> 5); //R
            *(outPtr++) = Y - ((U >> 2) + (U >> 4) + (U >> 5)) - ((V >> 1) + (V >> 3) + (V >> 4) + (V >> 5)); //G
            *(outPtr++) = Y + U + (U >> 1) + (U >> 2) + (U >> 6); //B
            *(outPtr++) = NUM_255; // gamma for RGBA_8888
        }
    }

    ANativeWindow_unlockAndPost(window);
    ANativeWindow_release(window);
}
}

