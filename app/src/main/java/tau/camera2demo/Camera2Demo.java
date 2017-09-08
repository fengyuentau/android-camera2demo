package tau.camera2demo;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.util.Size;
import android.view.Surface;
import android.view.TextureView;

public class Camera2Demo extends Activity implements
        TextureView.SurfaceTextureListener {

    private CameraDevice mCamera;
    private String mCameraID = "1";

    private TextureView mPreviewView;
    private Size mPreviewSize;
    private CaptureRequest.Builder mPreviewBuilder;
    private ImageReader mImageReader;

    private Handler mHandler;
    private HandlerThread mThreadHandler;

    // size of images captured in ImageReader Callback
    private int mImageWidth = 1920; //1920
    private int mImageHeight = 1080; //1080

    // Log tag
    private static final String TAG = "Camera2Demo";


    //surface
    private Surface surface;
    private static final int REQUEST_CAMERA_PERMISSION = 1;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.camera);

        initView();
        initLooper();

    }

    // subroutine to run camera
    private void initLooper() {
        mThreadHandler = new HandlerThread("CAMERA2");
        mThreadHandler.start();
        mHandler = new Handler(mThreadHandler.getLooper());
    }

    /*
     *  step 2: using TextureView to display PREVIEW
     */
    private void initView() {
        mPreviewView = (TextureView) findViewById(R.id.textureview);
        mPreviewView.setSurfaceTextureListener(this);
    }

    private void requestCameraPermission() {
        if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.CAMERA)) {
            //new ConfirmationDialog().show(getChildFragmentManager(), FRAGMENT_DIALOG);
        } else {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA},
                    REQUEST_CAMERA_PERMISSION);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        switch (requestCode){
            case REQUEST_CAMERA_PERMISSION: {
                //If request is cancelled, the result arrays are empty.
                if (grantResults.length != 1 || grantResults[0] != PackageManager.PERMISSION_GRANTED) {
                    //permission was granted, do the contacts-related task you want to do.
                } else {
                    //permission was denied, disable the functionality that depends on thhis permission.
                    super.onRequestPermissionsResult(requestCode, permissions, grantResults);
                }
            }
            //other 'case' lines to check for other permissions this app might request
        }
    }

    /*
     *  step 3: to set features of camera and open camera
     */
    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width,
                                          int height) {
        try {
            // to get the manager of all cameras
            CameraManager cameraManager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
            // to get features of the selected camera
            CameraCharacteristics characteristics = cameraManager.getCameraCharacteristics(mCameraID);
            // to get stream configuration from features
            StreamConfigurationMap map = characteristics
                    .get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
            // to get the size that the camera supports
            mPreviewSize = map.getOutputSizes(SurfaceTexture.class)[0];

            // open camera
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                requestCameraPermission();
                return;
            }
            cameraManager.openCamera(mCameraID, mCameraDeviceStateCallback, mHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width,
                                            int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        return false;
    }

    // note that the following method will be called every time a frame's ready
    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {

    }

    private CameraDevice.StateCallback mCameraDeviceStateCallback = new CameraDevice.StateCallback() {

        @Override
        public void onOpened(CameraDevice camera) {
            try {
                mCamera = camera;
                startPreview(mCamera);
            } catch (CameraAccessException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onDisconnected(CameraDevice camera) {

        }

        @Override
        public void onError(CameraDevice camera, int error) {

        }
    };

    /*
     *  step 4: to start PREVIEW
     */
    private void startPreview(CameraDevice camera) throws CameraAccessException {
        SurfaceTexture texture = mPreviewView.getSurfaceTexture();

        // to set PREVIEW size
        texture.setDefaultBufferSize(mPreviewSize.getWidth(),mPreviewSize.getHeight());
        surface = new Surface(texture);
        try {
            // to set request for PREVIEW
            mPreviewBuilder = camera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }

        // to set the format of captured images and the maximum number of images that can be accessed in mImageReader
        mImageReader = ImageReader.newInstance(mImageWidth, mImageHeight, ImageFormat.YUV_420_888, 2);

        mImageReader.setOnImageAvailableListener(mOnImageAvailableListener,mHandler);

        // the first added target surface is for camera PREVIEW display
        // the second added target mImageReader.getSurface() is for ImageReader Callback where we can access EACH frame
        //mPreviewBuilder.addTarget(surface);
        mPreviewBuilder.addTarget(mImageReader.getSurface());

        //output Surface
        List<Surface> outputSurfaces = new ArrayList<>();
        outputSurfaces.add(mImageReader.getSurface());

        /*camera.createCaptureSession(
                Arrays.asList(surface, mImageReader.getSurface()),
                mSessionStateCallback, mHandler);
                */
        camera.createCaptureSession(outputSurfaces, mSessionStateCallback, mHandler);
    }


    private CameraCaptureSession.StateCallback mSessionStateCallback = new CameraCaptureSession.StateCallback() {

        @Override
        public void onConfigured(CameraCaptureSession session) {
            try {
                updatePreview(session);
            } catch (CameraAccessException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onConfigureFailed(CameraCaptureSession session) {

        }
    };

    private void updatePreview(CameraCaptureSession session)
            throws CameraAccessException {
        mPreviewBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_AUTO);

        session.setRepeatingRequest(mPreviewBuilder.build(), null, mHandler);
    }


    /*
     *  step 5: to implement listener and access each frame
     */
    private ImageReader.OnImageAvailableListener mOnImageAvailableListener = new ImageReader.OnImageAvailableListener() {

        /*
         *  The following method will be called every time an image is ready
         *  be sure to use method acquireNextImage() and then close(), otherwise, the display may STOP
         */
        @Override
        public void onImageAvailable(ImageReader reader) {
            // get the newest frame
            Image image = reader.acquireNextImage();

            if (image == null) {
                return;
            }

            // HERE to call jni methods
            //grayscale output
//            JNIUtils.GrayscaleDisplay(image.getWidth(), image.getHeight(), image.getPlanes()[0].getRowStride(), image.getPlanes()[0].getBuffer(), surface);

            //RGBA output
            Image.Plane Y_plane = image.getPlanes()[0];
            int Y_rowStride = Y_plane.getRowStride();
            Image.Plane U_plane = image.getPlanes()[1];
            int UV_rowStride = U_plane.getRowStride();  //in particular, uPlane.getRowStride() == vPlane.getRowStride()
            Image.Plane V_plane = image.getPlanes()[2];
            JNIUtils.RGBADisplay(image.getWidth(), image.getHeight(), Y_rowStride, Y_plane.getBuffer(), UV_rowStride, U_plane.getBuffer(), V_plane.getBuffer(), surface);

//            JNIUtils.RGBADisplay2(image.getWidth(), image.getHeight(), Y_rowStride, Y_plane.getBuffer(), U_plane.getBuffer(), V_plane.getBuffer(), surface);

//            Log.d(TAG, "Y plane pixel stride: " + Y_plane.getPixelStride());
//            Log.d(TAG, "U plane pixel stride: " + U_plane.getPixelStride());
//            Log.d(TAG, "V plane pixel stride: " + V_plane.getPixelStride());

//            Log.d(TAG, "Y plane length: " + Y_plane.getBuffer().remaining());
//            Log.d(TAG, "U plane length: " + U_plane.getBuffer().remaining());
//            Log.d(TAG, "V plane length: " + V_plane.getBuffer().remaining());

//            Log.d(TAG, "Y plane rowStride: " + Y_rowStride);
//            Log.d(TAG, "U plane rowStride: " + U_rowStride);
//            Log.d(TAG, "V plane rowStride: " + V_rowStride);


            image.close();
        }
    };


    protected void onPause() {
        if (null != mCamera) {
            mCamera.close();
            mCamera = null;
        }
        if (null != mImageReader) {
            mImageReader.close();
            mImageReader = null;
        }
        super.onPause();
    }
}