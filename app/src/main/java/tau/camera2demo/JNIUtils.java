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

    public static native void display(int srcWidth, int srcHeight, int rowStride, ByteBuffer srcBuffer, Surface surface);
    public static native void display2(int srcWidth, int srcHeight, byte[] Y, byte[] U, byte[] V, Surface surface);
}
