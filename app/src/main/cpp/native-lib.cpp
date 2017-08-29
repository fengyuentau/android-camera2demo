#include <jni.h>
#include <string>
#include <android/log.h>
//#include <android/bitmap.h>
#include <android/native_window_jni.h>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Camera2Demo", __VA_ARGS__)

extern "C" {
JNIEXPORT jstring JNICALL Java_tau_camera2demo_JNIUtils_display(
        JNIEnv *env,
        jobject obj,
        jint srcWidth,
        jint srcHeight,
        jobject srcBuffer,
        jobject surface) {
    /*
    uint8_t *srcLumaPtr = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(srcBuffer));

    if (srcLumaPtr == nullptr) {
        LOGE("srcLumaPtr null ERROR!");
        return NULL;
    }
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

    //uint8_t * dstLumaPtr = reinterpret_cast<uint8_t *>(buffer.bits);
    memcpy(buffer.bits, srcBuffer,  srcWidth * srcHeight * 4);


    ANativeWindow_unlockAndPost(window);
    ANativeWindow_release(window);

    return NULL;
}
}
