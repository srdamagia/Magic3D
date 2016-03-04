/******************************************************************************
 @Copyright    Copyright (C) 2008 - 2015 by MagicTech.

 @Platform     ANSI compatible
******************************************************************************/
/*
Magic3D Engine
Copyright (c) 2008-2015
Thiago C. Moraes
http://www.magictech.com.br

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.
   If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef MAGIC3D_NETWORK_H
#define MAGIC3D_NETWORK_H

#include <magic3d/object.h>
#include <magic3d/input.h>
#include <magic3d/gui/imgui_window/imgui_window_network.h>

namespace Magic3D
{

/*
NETWORK Packets

NETWORK_SPAWN  byte               type
               enet_uint32        clientID
               enet_uint32        peerID
               unsigned char[256] nick
               unsigned char[256] name

NETWORK_KILL   byte               type
               enet_uint32        clientID
               unsigned char[256] name

NETWORK_OBJECT byte               type
               enet_uint32        clientID
               unsigned char[256] name
               Matrix4            matrix

NETWORK_INPUT  byte               type
               enet_uint32        clientID
               int                INPUT
               int                EVENT
               Vector4            x, y, z ,w

NETWORK_TEXT   byte               type
               enet_uint32        clientID
               unsigned char*     nick
               unsigned char*     text
*/

#define NETWORK_HEADER (sizeof(byte) + sizeof(enet_uint32))
#define NETWORK_TEXT_SIZE 256

#define NETWORK_COMMAND_SPAWN      "SPAWN"
#define NETWORK_COMMAND_KILL       "KILL"
#define NETWORK_COMMAND_TEXT       "TEXT"
#define NETWORK_COMMAND_DISCONNECT "DISCONNECT"

enum NETWORK_PACKET
{
    eNETWORK_SPAWN,
    eNETWORK_KILL,
    eNETWORK_OBJECT,
    eNETWORK_INPUT,
    eNETWORK_TEXT,
    eNETWORK_COMMAND,
    eNETWORK_DISCONNECT
};

struct NetworkClient
{
    ENetAddress address;
    std::string nick;
};

class Network
{
private:
    static Network* instance;

    ENetAddress address;
    ENetHost* server;
    ENetPeer* peer;

    std::map<enet_uint32, NetworkClient> clients;
    std::map<std::string, enet_uint32> spawned;

    std::string nick;
    std::string ip;
    int port;
    bool hosting;

    float timeUpdate;

    GUINetwork console;
    bool showConsole;

    Network();
    virtual ~Network();

    void clear();
    void prepareAddress(std::string ip, int port);
    enet_uint32 getID();    

    void prepareHeader(NETWORK_PACKET type, byte* data);

    void openPacket(ENetPacket* packet);
    void sendPacket(ENetPacket* packet);
    void broadcastPacket(ENetPacket* packet);

    Object* spawnObject(std::string name, enet_uint32 id);

    void log(LOG type, const char* format, ...);

public:
    static bool start();
    static bool finish();

    static Network* getInstance();

    bool initialize();
    bool deinitialize();

    void connect(std::string ip, int port);
    void disconnect(bool now);
    bool isConnected();

    void setNick(std::string nick);
    const std::string& getNick();

    void update();
    void render();

    std::string getObjectBaseName(std::string name);
    enet_uint32 getObjectClientID(std::string name);
    ENetAddress getClient(enet_uint32 id);
    std::string getClientNick(enet_uint32 id);
    int getClientsCount();

    bool isServer();

    Object* spawnObject(std::string name);
    void killObject(std::string name);
    void sendObject(Object* object, bool now);
    void sendInput(INPUT input, EVENT event, Vector4 params);
    void sendText(std::string text);
    void sendCommand(std::string command, std::string value);
    void sendDisconnect(enet_uint32 id);
};

}
#endif // MAGIC3D_NETWORK_H
