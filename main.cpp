#include <magic3d/magic3d.h>

#define DEFAULT_SCENE "main"
static int stopLoading = true;

class Event : public Magic3D::EventListener_Keyboard
{
public:
    Event() : Magic3D::EventListener_Keyboard()
    {

    }

    ~Event()
    {

    }

    void down(int key)
    {
    if (key == 27)
    {
        if (!stopLoading)
            stopLoading = true;
        else
            Magic3D::Magic3D::stop();
    }
    }
};

int main(int argc, char *argv[])
{
    std::string applicationPath = argv[0];
    size_t pos = applicationPath.find_last_of('/');
    if (pos == std::string::npos)
    {
        pos = applicationPath.find_last_of('\\');
    }
    if (pos == std::string::npos)
    {
        applicationPath.clear();
    }
    else
    {
        applicationPath = applicationPath.substr(0, pos + 1);
        pos = applicationPath.find('\\');
        while (pos != applicationPath.npos)
        {
            applicationPath.replace(pos, 1, "/");
            pos = applicationPath.find('\\');
        }
    }
    if (Magic3D::Magic3D::start(applicationPath))
    {        
        Magic3D::Input::getInstance()->addEventListener(new Event());
        //Magic3D::Renderer::getInstance()->createWindow(false, true, 1280, 1024);
        Magic3D::Renderer::getInstance()->createWindow();

        //Magic3D::ResourceManager::getInstance()->setPath(applicationPath + "data", false);
        Magic3D::ResourceManager::getInstance()->setPath(applicationPath + "data.magic3d", true);
        Magic3D::ResourceManager::getInstance()->setUserPath(applicationPath);

        Magic3D::Magic3D::play();

        Magic3D::Scene::getInstance()->load(DEFAULT_SCENE, false);

        int cycles = 0;
        int count = 0;
        int total = 1;
        while (Magic3D::Magic3D::run())
        {
            if (!stopLoading && !Magic3D::Scene::getInstance()->isLoading())
            {
                count++;
                if (count > 0 && cycles++ > 500)
                {
                    total++;
                    count = 0;
                    cycles = 0;
                    Magic3D::Scene::getInstance()->clear(true);
                    Magic3D::Scene::getInstance()->load(DEFAULT_SCENE, false);

                    if (total == 1000)
                    {
                        stopLoading = true;
                    }
                }
            }
            else
            {
                if (!stopLoading && cycles++ > 500)
                {
                     Magic3D::Magic3D::stop();
                }
            }
        }

        if (Magic3D::Script::getInstance()->isPlaying())
        {
            std::vector<Magic3D::ScriptHelpItems> list;
            Magic3D::Script::getInstance()->help(&list);

            Magic3D::Script::getInstance()->dumpStack();
        }

        Magic3D::Log::logFormat( Magic3D::eLOG_SUCCESS, "%d scenes loaded.", total);
        Magic3D::Magic3D::finish();
    }
    return 0;
}
