package br.com.magictech.game;

import org.libsdl.app.SDLActivity;
import android.view.View;
import android.view.WindowManager;
import android.os.Bundle;

public class Game extends SDLActivity
{
    static {
        System.loadLibrary("magic3d");
    }

    public static native void onNativeAccel(float x, float y, float z);
    public static native void onNativeGyro(float x, float y, float z, float w);

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().getDecorView().setSystemUiVisibility( View.SYSTEM_UI_FLAG_LOW_PROFILE );
    }
}