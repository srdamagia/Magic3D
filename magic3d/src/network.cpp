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

#include <magic3d/magic3d.h>

Magic3D::Network* Magic3D::Network::instance = NULL;


Magic3D::Network::Network()
{
    initialize();
}

Magic3D::Network::~Network()
{
    deinitialize();
}

bool Magic3D::Network::start()
{
    bool result = true;

    if (!instance)
    {
        instance = new Network();
    }    

    if (instance)
    {
        Log::log(eLOG_SUCCESS, "Network sucessfully started.");
    }
    else
    {
        result = false;
        Log::log(eLOG_FAILURE, "Network not started.");
    }

    return result;
}

bool Magic3D::Network::finish()
{
    if (instance)
    {
        delete instance;
        instance = NULL;
    }

    if (!instance)
    {
        Log::log(eLOG_SUCCESS, "Network sucessfully finished.");
    }
    else
    {
        Log::log(eLOG_FAILURE, "Network not finished.");
    }

    return !instance;
}

Magic3D::Network* Magic3D::Network::getInstance()
{
    return instance;
}

bool Magic3D::Network::initialize()
{    
    server = NULL;
    peer = NULL;

#if defined(_MGE_)
    showConsole = false;
#else
    showConsole = true;
#endif

    ip = Magic3D::getInstance()->getConfiguration().ADDRESS;
    port = Magic3D::getInstance()->getConfiguration().PORT;

    timeUpdate = 0.0f;

    bool result = true;
    int error = enet_initialize ();
    if (error != 0)
    {
        Log::logFormat(eLOG_FAILURE, "An error occurred while initializing ENet. %d", error);
        result = false;
    }
    else
    {
        prepareAddress(ip, port);

        server = enet_host_create(isServer() ? &address : NULL, Magic3D::getInstance()->getConfiguration().CLIENTS, 2, 0, 0);
        if (server == NULL)
        {
            Log::log(eLOG_FAILURE, "An error occurred while trying to create an ENet server host.");
        }
    }

    connect(ip, port);

    return result;
}

bool Magic3D::Network::deinitialize()
{
    disconnect(true);
    if (peer)
    {
        enet_peer_reset(peer);
        peer = NULL;
    }

    if (server)
    {
        enet_host_destroy(server);
        server = NULL;
    }
    enet_deinitialize();
    return true;
}

std::string Magic3D::Network::getObjectBaseName(std::string name)
{
    return name.substr(0, name.find_last_of('#'));
}

void Magic3D::Network::prepareAddress(std::string ip, int port)
{
    if (!ip.empty())
    {
        enet_address_set_host(&address, ip.c_str());
    }
    else
    {
        address.host = ENET_HOST_ANY;
    }
    address.port = port;
}

enet_uint32 Magic3D::Network::getID()
{
    if (isConnected())
    {
        return peer->connectID;
    }
    else
    {
        return 0;
    }
}

ENetAddress Magic3D::Network::getClient(enet_uint32 id)
{
    ENetAddress result;
    result.host = 0;
    result.port = 0;
    std::map<enet_uint32, ENetAddress>::const_iterator it_a = clients.find(id);
    if (it_a != clients.end())
    {
        result = (*it_a).second;
    }
    return result;
}

void Magic3D::Network::prepareHeader(NETWORK_PACKET type, byte* data)
{
    data[0] = (byte)type;
    enet_uint32 ip = getID();
    memcpy(&data[1], &ip, sizeof(enet_uint32));
}

void Magic3D::Network::connect(std::string ip, int port)
{
    this->ip = ip;
    this->port = port;
    if (!isConnected())
    {
        prepareAddress(this->ip, this->port);

        if (server && !isServer() && address.host != ENET_HOST_ANY)
        {
            peer = enet_host_connect(server, &address, 2, 0);

            if (peer == NULL)
            {
                log(eLOG_FAILURE, "No available peers for initializing an ENet connection.");
            }
            else
            {
                ENetEvent event;
                if (enet_host_service(server, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
                {
                    log(eLOG_SUCCESS, "Host connection %s successful!", Magic3D::getInstance()->getConfiguration().ADDRESS.c_str());
                }
                else
                {
                    log(eLOG_FAILURE, "Host connection failed!");
                    enet_peer_reset(peer);
                    peer = NULL;
                }
            }
        }
    }
}

void Magic3D::Network::disconnect(bool now)
{
    if (isConnected())
    {
        if (now)
        {
            enet_peer_disconnect_now(peer, 0);
        }
        else
        {
            enet_peer_disconnect(peer, 0);
        }
    }
}

bool Magic3D::Network::isConnected()
{
    return peer && peer->connectID > 0;
}

void Magic3D::Network::update()
{
    if (Physics::getInstance()->isPlaying())
    {
        timeUpdate += Magic3D::getInstance()->getElapsedTime();
        if (timeUpdate > 0.1f)
        {
            timeUpdate = 0.0f;
        }

        if (!showConsole)
        {
            connect(ip, port);
        }

        ENetEvent event;        
        if (server && enet_host_service(server, &event, 0) > 0)
        {
            switch(event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                {
                    char name[NETWORK_TEXT_SIZE];
                    enet_address_get_host_ip(&event.peer->address, name, 255);
                    log(eLOG_SUCCESS, "A new client connected from %s:%u.\n", name, event.peer->address.port);

                    if (getClient(event.peer->connectID).host == 0)
                    {
                        clients[event.peer->connectID] = event.peer->address;
                    }

                    std::map<std::string, enet_uint32>::const_iterator it_o = spawned.begin();
                    while (it_o != spawned.end())
                    {
                        spawnObject(getObjectBaseName((*it_o).first), (*it_o).second);
                        ++it_o;
                    }

                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE:
                {
                    if (isServer())
                    {
                        broadcastPacket(event.packet);
                    }
                    openPacket(event.packet);

                    break;
                }

                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    if (isServer())
                    {
                        enet_uint32 id = 0;
                        std::map<enet_uint32, ENetAddress>::const_iterator it_c = clients.begin();
                        while (it_c != clients.end())
                        {
                            ENetAddress peerAddress = (*it_c).second;
                            if (peerAddress.host == event.peer->address.host && peerAddress.port == event.peer->address.port)
                            {
                                id = (*it_c).first;
                                break;
                            }
                            ++it_c;
                        }
                        if (id != 0)
                        {
                            clients.erase(it_c);
                        }

                        std::map<std::string, enet_uint32>::const_iterator it_o = spawned.begin();
                        while (it_o != spawned.end())
                        {
                            enet_uint32 objID = (*it_o).second;
                            if (objID == id)
                            {
                                Object* object = ResourceManager::getObjects()->get((*it_o).first);
                                if (object)
                                {
                                    killObject((*it_o).first);
                                    Scene::getInstance()->removeObject(object->getLayer(), object);
                                    ResourceManager::getObjects()->remove((*it_o).first);
                                }
                                spawned.erase((*it_o).first);
                                it_o = spawned.begin();
                            }
                            else
                            {
                                ++it_o;
                            }
                        }
                    }

                    char name[NETWORK_TEXT_SIZE];
                    enet_address_get_host_ip(&event.peer->address, name, NETWORK_TEXT_SIZE - 1);
                    log(eLOG_PLAINTEXT, "%s disconnected %s:%u.\n", isServer() ? "Client" : "Host", name, event.peer->address.port);
                    event.peer->data = NULL;

                    if (peer)
                    {
                        enet_peer_reset(peer);
                        peer = NULL;
                    }
                    break;
                }

                default: break;
            }
        }
    }
}

void Magic3D::Network::render()
{
    if (showConsole)
    {
        console.draw("Network", &showConsole);
    }
}

int Magic3D::Network::getClientsCount()
{
    return clients.size();
}

bool Magic3D::Network::isServer()
{
    return Magic3D::getInstance()->getConfiguration().SERVER;
}

void Magic3D::Network::openPacket(ENetPacket* packet)
{
    if (packet)
    {
        enet_uint32 sender;
        memcpy(&sender, &packet->data[1], sizeof(enet_uint32));

        if (sender != getID())
        {
            switch (packet->data[0])
            {
                case eNETWORK_SPAWN:
                {
                    enet_uint32 peerID;
                    memcpy(&peerID, &packet->data[NETWORK_HEADER], sizeof(enet_uint32));
                    char name[NETWORK_TEXT_SIZE];
                    memcpy(&name[0], &packet->data[NETWORK_HEADER + sizeof(enet_uint32)], packet->dataLength - (NETWORK_HEADER + sizeof(enet_uint32)));

                    Object* object = ResourceManager::getObjects()->get(name);
                    if (object)
                    {
                        sprintf(&name[0], "%s#%d", object->getName().c_str(), peerID);

                        if (!ResourceManager::getObjects()->get(name))
                        {
                            object = object->spawn(name, object->getLayer()->getName(), true);
                            spawned[name] = peerID;
                            log(eLOG_SUCCESS, "Object: %s spawned.", name);
                        }
                        else if (peerID != getID())
                        {
                            log(eLOG_RENDERER, "Object: %s already exists.", name);
                        }
                    }

                    break;
                }
                case eNETWORK_KILL:
                {
                    char name[packet->dataLength - NETWORK_HEADER];
                    memcpy(&name[0], &packet->data[NETWORK_HEADER], packet->dataLength - NETWORK_HEADER);

                    Object* object = ResourceManager::getObjects()->get(name);
                    if (object)
                    {
                        std::map<std::string, enet_uint32>::const_iterator it_o = spawned.begin();
                        while (it_o != spawned.end())
                        {
                            if ((*it_o).first.compare(name) == 0)
                            {
                                enet_uint32 id = (*it_o).second;
                                Scene::getInstance()->removeObject(object->getLayer(), object);
                                ResourceManager::getObjects()->remove((*it_o).first);
                                spawned.erase((*it_o).first);
                                clients.erase(id);
                                break;
                            }
                            else
                            {
                                ++it_o;
                            }
                        }
                    }

                    break;
                }
                case eNETWORK_OBJECT:
                {
                    char name[NETWORK_TEXT_SIZE];
                    memcpy(&name[0], &packet->data[NETWORK_HEADER], NETWORK_TEXT_SIZE);
                    Object* object = ResourceManager::getObjects()->get(name);
                    if (object)
                    {
                        Vector4 pos;
                        Quaternion rot;
                        Vector4 scale;
                        int stride = 0;
                        memcpy(&pos[0], &packet->data[NETWORK_HEADER + NETWORK_TEXT_SIZE + stride], sizeof(Vector4));
                        stride += sizeof(Vector4);
                        memcpy(&rot[0], &packet->data[NETWORK_HEADER + NETWORK_TEXT_SIZE + stride], sizeof(Quaternion));
                        stride += sizeof(Quaternion);
                        memcpy(&scale[0], &packet->data[NETWORK_HEADER + NETWORK_TEXT_SIZE + stride], sizeof(Vector4));
                        object->setPosition(pos.getXYZ());
                        object->setRotation(rot);
                        object->setScale(scale.getXYZ());
                        object->resetPhysics();

                        if (object->getType() == eOBJECT_MODEL)
                        {
                            Model* model = static_cast<Model*>(object);
                            int index = (int)pos.getW();
                            int frame = (int)scale.getW();
                            if (model && model->getSkeleton() && model->getSkeleton()->getAnimation())
                            {
                                bool change = false;
                                /*if (model->getSkeleton()->getAnimation()->getCurrentSequenceIndex() != index)
                                {
                                    change = true;
                                }*/
                                model->playAnimation(model->getSkeleton()->getAnimation()->getSequenceName(index), 0.1f, false);
                                if (change)
                                {
                                    model->setAnimationFrame(frame);
                                }
                            }
                        }
                        //log(eLOG_SUCCESS, "Object: %s updated.", name);
                    }
                    else
                    {
                        log(eLOG_FAILURE, "Object: %s not found.", name);
                    }
                    break;
                }
                case eNETWORK_INPUT:
                {
                    int input;
                    int event;
                    Vector4 params;
                    int stride = 0;
                    memcpy(&input, &packet->data[NETWORK_HEADER + stride], sizeof(int));
                    stride += sizeof(int);
                    memcpy(&event, &packet->data[NETWORK_HEADER + stride], sizeof(int));
                    stride += sizeof(int);
                    memcpy(&params[0], &packet->data[NETWORK_HEADER + stride], sizeof(Vector4));

                    Input::getInstance()->dispatchEvent((INPUT)input, (EVENT)event, params.getX(), params.getY(), params.getZ(), params.getW());

                    break;
                }
                case eNETWORK_TEXT:
                {
                    char nick[NETWORK_TEXT_SIZE];
                    memcpy(&nick[0], &packet->data[NETWORK_HEADER], NETWORK_TEXT_SIZE);
                    log(eLOG_RENDERER, "%s: %s", nick, &packet->data[NETWORK_HEADER + NETWORK_TEXT_SIZE]);
                    break;
                }
            }
        }

        enet_packet_destroy(packet);
    }    
}

void Magic3D::Network::sendPacket(ENetPacket* packet)
{
    if (packet)
    {
        int channel = 0;
        if (packet->data[0] == eNETWORK_TEXT)
        {
            channel = 1;
        }
        if ((isServer() && server) || isConnected())
        {
            if (isServer())
            {
                enet_host_broadcast(server, channel, packet);
            }
            else
            {
                enet_peer_send(peer, channel, packet);
            }
            enet_host_flush(server);
        }        
    }
}

void Magic3D::Network::broadcastPacket(ENetPacket* packet)
{
    if (packet)
    {
        int channel = 0;
        if (packet->data[0] == eNETWORK_TEXT)
        {
            channel = 1;
        }
        ENetPacket* newPacket;
        if (packet->data[0] == eNETWORK_OBJECT || packet->data[0] == eNETWORK_INPUT)
        {
            newPacket = enet_packet_create(packet->data, packet->dataLength, ENET_PACKET_FLAG_UNSEQUENCED);
        }
        else
        {
            newPacket = enet_packet_create(packet->data, packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
        }
        enet_host_broadcast(server, channel, newPacket);
        enet_host_flush(server);
    }
}

Magic3D::Object* Magic3D::Network::spawnObject(std::string name)
{
    return spawnObject(name, getID());
}

Magic3D::Object* Magic3D::Network::spawnObject(std::string name, enet_uint32 id)
{
    Object* result = NULL;
    if (!name.empty())
    {
        if (isServer() || isConnected())
        {
            result = ResourceManager::getObjects()->get(name);
            if (result)
            {
                char spawnName[NETWORK_TEXT_SIZE];
                sprintf(&spawnName[0], "%s#%d", name.c_str(), getID());
                Object* tmp = ResourceManager::getObjects()->get(spawnName);
                if (!tmp)
                {
                    result = result->spawn(spawnName, result->getLayer()->getName(), true);
                }
                else
                {
                    result = tmp;
                }

                if (result)
                {
                    if (isServer())
                    {
                        spawned[spawnName] = 0;
                    }
                    result->setNetworkSpawn(true);
                    unsigned int size = NETWORK_HEADER + sizeof(enet_uint32) + name.size() + 1;
                    ENetPacket* packet = enet_packet_create(NULL, size, ENET_PACKET_FLAG_RELIABLE);
                    prepareHeader(eNETWORK_SPAWN, packet->data);
                    memcpy(&packet->data[NETWORK_HEADER], &id, sizeof(enet_uint32));
                    memcpy(&packet->data[NETWORK_HEADER + sizeof(enet_uint32)], name.c_str(), name.size());
                    packet->data[size - 1] = '\0';

                    sendPacket(packet);
                }
            }
        }
    }

    return result;
}

void Magic3D::Network::killObject(std::string name)
{
    if (!name.empty())
    {
        if (isServer() || isConnected())
        {
            unsigned int size = NETWORK_HEADER + name.size() + 1;
            ENetPacket* packet = enet_packet_create(NULL, size, ENET_PACKET_FLAG_RELIABLE);
            prepareHeader(eNETWORK_KILL, packet->data);
            memcpy(&packet->data[NETWORK_HEADER], name.c_str(), name.size());
            packet->data[size - 1] = '\0';

            sendPacket(packet);
        }
    }
}

void Magic3D::Network::sendObject(Object* object, bool now)
{
    if (object)
    {
        if ((isServer() || isConnected()) && (timeUpdate == 0.0f || now))
        {
            //Matrix4 matrix = object->getMatrix();
            Vector4 pos = Vector4(object->getPosition(), 1.0f);            
            Quaternion rot = object->getRotation();
            Vector4 scale = Vector4(object->getScale(), 1.0f);

            if (object->getType() == eOBJECT_MODEL)
            {
                pos.setW(static_cast<Model*>(object)->getSkeleton()->getAnimation()->getCurrentSequenceIndex());
                scale.setW(static_cast<Model*>(object)->getSkeleton()->getAnimation()->getCurrentFrame());
            }

            unsigned int size = NETWORK_HEADER + NETWORK_TEXT_SIZE + sizeof(Vector4) + sizeof(Quaternion) + sizeof(Vector4);
            ENetPacket* packet = enet_packet_create(NULL, size, ENET_PACKET_FLAG_UNSEQUENCED);
            prepareHeader(eNETWORK_OBJECT, packet->data);
            memcpy(&packet->data[NETWORK_HEADER], object->getName().c_str(), object->getName().size());
            packet->data[NETWORK_HEADER + object->getName().size()] = '\0';
            int stride = 0;
            memcpy(&packet->data[NETWORK_HEADER + NETWORK_TEXT_SIZE + stride], reinterpret_cast<float*>(&pos), sizeof(Vector4));
            stride += sizeof(Vector4);
            memcpy(&packet->data[NETWORK_HEADER + NETWORK_TEXT_SIZE + stride], reinterpret_cast<float*>(&rot), sizeof(Quaternion));
            stride += sizeof(Quaternion);
            memcpy(&packet->data[NETWORK_HEADER + NETWORK_TEXT_SIZE + stride], reinterpret_cast<float*>(&scale), sizeof(Vector4));

            sendPacket(packet);
        }
    }
}

void Magic3D::Network::sendInput(INPUT input, EVENT event, Vector4 params)
{
    if (isServer() || isConnected())
    {
        unsigned int size = NETWORK_HEADER + sizeof(int) + sizeof(int) + sizeof(Vector4);
        ENetPacket* packet = enet_packet_create(NULL, size, ENET_PACKET_FLAG_UNSEQUENCED);
        prepareHeader(eNETWORK_TEXT, packet->data);
        memcpy(&packet->data[NETWORK_HEADER], reinterpret_cast<int*>(&input), sizeof(int));
        memcpy(&packet->data[NETWORK_HEADER + sizeof(int)], reinterpret_cast<int*>(&event), sizeof(int));
        memcpy(&packet->data[NETWORK_HEADER + sizeof(int) + sizeof(int)], reinterpret_cast<float*>(&params), sizeof(Vector4));

        sendPacket(packet);
    }
}

void Magic3D::Network::sendText(std::string nick, std::string text)
{
    if (!text.empty())
    {
        if (isServer() || isConnected())
        {
            unsigned int size = NETWORK_HEADER + NETWORK_TEXT_SIZE + text.size() + 1;
            ENetPacket* packet = enet_packet_create(NULL, size, ENET_PACKET_FLAG_RELIABLE);
            prepareHeader(eNETWORK_TEXT, packet->data);
            memcpy(&packet->data[NETWORK_HEADER], nick.c_str(), nick.size());
            packet->data[NETWORK_HEADER + nick.size()] = '\0';
            memcpy(&packet->data[NETWORK_HEADER + NETWORK_TEXT_SIZE], text.c_str(), text.size());
            packet->data[size - 1] = '\0';

            sendPacket(packet);
            log(eLOG_PLAINTEXT, "%s: %s", nick.c_str(), text.c_str());
        }
    }
}

void Magic3D::Network::log(LOG type, const char* format, ...)
{
    char szParsedString[1024];
    va_list va;
    va_start(va, format);    
    vsprintf(szParsedString, format, va);
    va_end(va);

    if (showConsole)
    {
        console.addLog(szParsedString);
    }
    else
    {
        Log::logFormat(type, szParsedString);
    }
}

/*// unpack method for retrieving data in network byte,
//   big endian, order (MSB first)
// increments index i by the number of bytes unpacked
// usage:
//   int i = 0;
//   float x = unpackFloat(&buffer[i], &i);
//   float y = unpackFloat(&buffer[i], &i);
//   float z = unpackFloat(&buffer[i], &i);
float unpackFloat(const void *buf, int *i) {
    const unsigned char *b = (const unsigned char *)buf;
    uint32_t temp = 0;
    *i += 4;
    temp = ((b[0] << 24) |
            (b[1] << 16) |
            (b[2] <<  8) |
             b[3]);
    return *((float *) temp);
}

// pack method for storing data in network,
//   big endian, byte order (MSB first)
// returns number of bytes packed
// usage:
//   float x, y, z;
//   int i = 0;
//   i += packFloat(&buffer[i], x);
//   i += packFloat(&buffer[i], y);
//   i += packFloat(&buffer[i], z);
int packFloat(void *buf, float x) {
    unsigned char *b = (unsigned char *)buf;
    unsigned char *p = (unsigned char *) &x;
#if defined (_M_IX86) || (defined (CPU_FAMILY) && (CPU_FAMILY == I80X86))
    b[0] = p[3];
    b[1] = p[2];
    b[2] = p[1];
    b[3] = p[0];
#else
    b[0] = p[0];
    b[1] = p[1];
    b[2] = p[2];
    b[3] = p[3];
#endif
    return 4;
}*/
