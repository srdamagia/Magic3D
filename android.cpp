#include <jni.h>
#include <android/log.h>
#include <magic3d/magic3d.h>

#include <SDL.h>
#include <SDL_main.h>

#define DEFAULT_SCENE "main"

#define log_v(TAG,...) __android_log_print(ANDROID_LOG_VERBOSE, TAG,__VA_ARGS__)
#define log_d(TAG,...) __android_log_print(ANDROID_LOG_DEBUG  , TAG,__VA_ARGS__)
#define log_i(TAG,...) __android_log_print(ANDROID_LOG_INFO   , TAG,__VA_ARGS__)
#define log_w(TAG,...) __android_log_print(ANDROID_LOG_WARN   , TAG,__VA_ARGS__)
#define log_e(TAG,...) __android_log_print(ANDROID_LOG_ERROR  , TAG,__VA_ARGS__)

static bool started = false;

static float accel[3] = {0.0f, 0.0f, 0.0f};
static bool accel_updated = false;

static float gyro[4] = {0.0f, 0.0f, 0.0f, 0.0f};
static bool gyro_updated = false;

#ifdef __cplusplus
extern "C" {
#endif
    JNIEXPORT void JNICALL Java_br_com_magictech_game_Game_onNativeAccel(JNIEnv* env, jclass jcls, jfloat x, jfloat y, jfloat z)
    {
        if (started)
        {
            accel[0] = x;
            accel[1] = y;
            accel[2] = z;

            accel_updated = true;
        }
    }

    JNIEXPORT void JNICALL Java_br_com_magictech_game_Game_onNativeGyro(JNIEnv* env, jclass jcls, jfloat x, jfloat y, jfloat z, jfloat w)
    {
        if (started)
        {
            gyro[0] = x;
            gyro[1] = y;
            gyro[2] = z;
            gyro[3] = w;

            gyro_updated = true;
        }
    }
#ifdef __cplusplus
}
#endif

class AndroidLogCallBack : public Magic3D::LogCallBack
{
public:
    AndroidLogCallBack() : Magic3D::LogCallBack(true)
    {

    }

    void log(Magic3D::LOG type, const char* text)
    {        
        const char* app = "Magic3D";
        switch (type)
        {
            case Magic3D::eLOG_SUCCESS:   log_v(app, "%s", text); break;
            case Magic3D::eLOG_FAILURE:   log_e(app, "%s", text); break;
            case Magic3D::eLOG_PLAINTEXT: log_i(app, "%s", text); break;
            case Magic3D::eLOG_RENDERER:  log_d(app, "%s", text); break;
        }
    }
};

int main(int argc, char *argv[])
{
    if (argc)
    {

    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        log_e("Magic3D", "SDL Error: %s\n", SDL_GetError());
        return false;
    }

    log_v("Magic3D", "%s", argv[0]);
    log_v("Magic3D", "%s", SDL_AndroidGetInternalStoragePath());

    AndroidLogCallBack logCallBack;

    std::string pathUser = SDL_AndroidGetInternalStoragePath();
    std::string log =  pathUser + "/magic3d.log.html";
    Magic3D::Log::setLogFile(log.c_str());    

    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, 0, &mode);

    Magic3D::Magic3DConfiguration configuration;

    configuration.WINDOW_WIDTH  = mode.w;
    configuration.WINDOW_HEIGHT = mode.h;
    configuration.FULLSCREEN    = true;
    configuration.CURSOR = false;
    configuration.VSYNC = false;

    configuration.NAME                 = "Android";
    configuration.TITLE                = "Game";
    configuration.FBO_MAP_SIZE         = 512;
    configuration.SHADOW_MAP_SIZE      = 1024;
    configuration.REFLECTION_MAP_SIZE  = 512;
    configuration.BLUR_MAP_SIZE        = 256;
    configuration.GLOW_MAP_SIZE        = 64;

    configuration.SHOW_FPS         = true;
    configuration.SHOW_INFORMATION = false;
    configuration.SHOW_COORDINATES = false;

    configuration.SHADOWS      = true;
    configuration.SOFT_SHADOWS = false;
    configuration.REFLECTIONS  = false;
    configuration.GLOW         = false;
    configuration.STEREOSCOPY  = false;

    configuration.CONSOLE = false;
    configuration.SERVER  = true;
    configuration.ADDRESS = "";
    configuration.PORT    = 31234;
    configuration.CLIENTS = 64;

    log_v("Magic3D", "Window width: %d - height: %d", configuration.WINDOW_WIDTH, configuration.WINDOW_HEIGHT);

    if (configuration.WINDOW_WIDTH > 1500 || configuration.WINDOW_HEIGHT > 1500)
    {
        configuration.SCREEN_FBO_SIZE  = 2048;
        configuration.SCREEN_VERTICES  = 64;
    }
    else if (configuration.WINDOW_WIDTH > 640 || configuration.WINDOW_HEIGHT > 640)
    {
        configuration.SCREEN_FBO_SIZE  = 1024;
        configuration.SCREEN_VERTICES  = 32;
    }
    else
    {
        configuration.SCREEN_FBO_SIZE  = 512;
        configuration.SCREEN_VERTICES  = 16;
    }

    if (Magic3D::Magic3D::start("", &configuration, &logCallBack) && Magic3D::Renderer::getInstance()->createWindow(false))
    {
        Magic3D::ResourceManager::getInstance()->setPath("", true);
        Magic3D::ResourceManager::getInstance()->setUserPath(pathUser);
        Magic3D::Magic3D::play();

        started = true;

        //Magic3D::Input::getInstance()->addEventListener(new TouchEvent(this));
        //Magic3D::Input::getInstance()->addEventListener(new AccelerometerEvent(this));

        Magic3D::Scene::getInstance()->load("main.mge", false);

        //Magic3D::Renderer::getInstance()->setRenderMode(Magic3D::eRENDER_MODE_DEPTH);

        while(Magic3D::Magic3D::getInstance()->run())
        {
            if (accel_updated)
            {
                accel_updated = false;
                Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_ACCELEROMETER, Magic3D::eEVENT_ACCELEROMETER, accel[0], accel[1], accel[2]);
            }

            if (gyro_updated)
            {
                gyro_updated = false;
                Magic3D::Input::getInstance()->dispatchEvent(Magic3D::eINPUT_GYROSCOPE, Magic3D::eEVENT_GYROSCOPE, gyro[0], gyro[1], gyro[2], gyro[3]);
            }
        }
    }

    return 0;
}
