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

    bool result = true;
    int error = enet_initialize ();
    if (error != 0)
    {
        Log::logFormat(eLOG_FAILURE, "An error occurred while initializing ENet. %d", error);
        result = false;
    }
    else
    {
        prepareAddress();

        server = enet_host_create(isServer() ? &address : NULL, Magic3D::getInstance()->getConfiguration().CLIENTS, 2, 0, 0);
        if (server == NULL)
        {
            Log::log(eLOG_FAILURE, "An error occurred while trying to create an ENet server host.");
        }

        connect();
    }
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

void Magic3D::Network::prepareAddress()
{
    if (!Magic3D::getInstance()->getConfiguration().ADDRESS.empty())
    {
        enet_address_set_host(&address, Magic3D::getInstance()->getConfiguration().ADDRESS.c_str());
    }
    else
    {
        address.host = ENET_HOST_ANY;
    }
    address.port = Magic3D::getInstance()->getConfiguration().PORT;
}

enet_uint32 Magic3D::Network::getAddress()
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

void Magic3D::Network::prepareHeader(NETWORK_PACKET type, byte* data)
{
    data[0] = (byte)type;
    enet_uint32 ip = getAddress();
    memcpy(&data[1], &ip, sizeof(enet_uint32));
}

void Magic3D::Network::connect()
{
    if (!isConnected())
    {
        prepareAddress();

        if (server && !isServer() && address.host != ENET_HOST_ANY)
        {
            peer = enet_host_connect(server, &address, 1, 0);
            if (peer == NULL)
            {
                Log::log(eLOG_FAILURE, "No available peers for initializing an ENet connection.");
            }
            else
            {
                ENetEvent event;
                if (enet_host_service(server, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
                {
                    Log::logFormat(eLOG_SUCCESS, "Host connection %s successful!", Magic3D::getInstance()->getConfiguration().ADDRESS.c_str());
                    char name[256];
                    ENetAddress tmp;
                    tmp.host = peer->connectID;
                    tmp.host = 0;
                    enet_address_get_host_ip(&tmp, name, 255);
                    Log::logFormat(eLOG_PLAINTEXT, "IP: %s:%u.\n", name, server->address.port);
                }
                else
                {
                    Log::log(eLOG_FAILURE, "Host connection failed!");
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
    connect();

    ENetEvent event;
    ENetPacket* packet;
    if (enet_host_service(server, &event, 0) > 0)
    {
        switch(event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                char name[256];
                enet_address_get_host_ip(&event.peer->address, name, 255);
                Log::logFormat(eLOG_SUCCESS, "A new client connected from %s:%u.\n", name, event.peer->address.port);
                clients.push_back(event.peer->address);

                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
            {
                Log::logFormat(eLOG_RENDERER, "A packet of length %u containing %s was received.", event.packet->dataLength, event.packet->data);

                if (isServer())
                {
                    packet = enet_packet_create(event.packet->data, event.packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
                    enet_host_broadcast(server, 0, packet);
                    enet_host_flush(server);                    
                }
                openPacket(event.packet);

                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                for (unsigned int i = 0; i < clients.size(); i++)
                {
                    if (clients[i].host == event.peer->address.host)
                    {
                        clients.erase(clients.begin() + i);
                    }
                }
                char name[256];
                enet_address_get_host_ip(&event.peer->address, name, 255);
                Log::logFormat(eLOG_PLAINTEXT, "Client disconnected %s:%u.\n", name, event.peer->address.port);
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

        if (sender != getAddress())
        {
            switch (packet->data[0])
            {
                case eNETWORK_OBJECT:
                {
                    unsigned char name[256];
                    memcpy(&name[0], &packet->data[NETWORK_HEADER], 256);
                    Log::logFormat(eLOG_RENDERER, "Object name: %s", name);

                    Matrix4 matrix;
                    memcpy(&matrix[0], &packet->data[NETWORK_HEADER + 256], sizeof(Matrix4));
                    for (int i = 0; i < 4; i++)
                    {
                        Log::logFormat(eLOG_RENDERER, "Object matrix row[%d]: %8.6f - %8.6f - %8.6f - %8.6f", i, matrix.getRow(i).getX(), matrix.getRow(i).getY(), matrix.getRow(i).getZ(), matrix.getRow(i).getW());
                    }
                    break;
                }
                case eNETWORK_INPUT:
                {
                    break;
                }
                case eNETWORK_TEXT:
                {
                    Log::logFormat(eLOG_RENDERER, "Network message: %s", &packet->data[NETWORK_HEADER]);
                    break;
                }
            }
        }
    }

    enet_packet_destroy(packet);
}

void Magic3D::Network::sendPacket(ENetPacket* packet)
{
    if (packet)
    {
        if (isServer() || isConnected())
        {
            if (isServer())
            {
                enet_host_broadcast(server, 0, packet);
            }
            else
            {
                enet_peer_send(peer, 0, packet);
            }
        }
    }
}

void Magic3D::Network::sendObject(Object* object)
{
    if (object)
    {
        if (isServer() || isConnected())
        {
            Matrix4 matrix = object->getMatrix();
            unsigned int size = NETWORK_HEADER + sizeof(byte) * 256 + sizeof(Matrix4);
            byte* data = new byte[size];
            prepareHeader(eNETWORK_OBJECT, data);
            memcpy(&data[NETWORK_HEADER], object->getName().c_str(), object->getName().size());
            data[NETWORK_HEADER + object->getName().size()] = '\0';
            memcpy(&data[NETWORK_HEADER + 256], reinterpret_cast<float*>(&matrix), sizeof(Matrix4));
            ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
            sendPacket(packet);
        }
    }
}

void Magic3D::Network::sendInput(INPUT input, EVENT event, float x, float y, float z, float w)
{

}

void Magic3D::Network::sendText(std::string text)
{
    if (!text.empty())
    {
        if (isServer() || isConnected())
        {
            unsigned int size = NETWORK_HEADER + text.size() + 1;
            byte* data = new byte[size];
            prepareHeader(eNETWORK_TEXT, data);
            memcpy(&data[NETWORK_HEADER], text.c_str(), text.size());
            data[size - 1] = '\0';
            ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
            sendPacket(packet);
        }
    }
}

// unpack method for retrieving data in network byte,
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
}
