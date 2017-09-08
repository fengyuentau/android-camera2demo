package tau.camera2demo;

import android.media.Image;
import android.view.Surface;

import java.nio.ByteBuffer;


public class JNIUtils {
    // TAG for JNIUtils class
    private static final String TAG = "JNIUtils";

    // Load native library.
    static {
        System.loadLibrary("native-lib");
    }

    public static native void GrayscaleDisplay(int srcWidth, int srcHeight, int rowStride, ByteBuffer srcBuffer, Surface surface);
    public static native void RGBADisplay(int srcWidth, int srcHeight, int Y_rowStride, ByteBuffer Y_Buffer, int UV_rowStride, ByteBuffer U_Buffer, ByteBuffer V_Buffer, Surface surface);
    public static native void RGBADisplay2(int srcWidth, int srcHeight, int Y_rowStride, ByteBuffer Y_Buffer, ByteBuffer U_Buffer, ByteBuffer V_Buffer, Surface surface);
}
