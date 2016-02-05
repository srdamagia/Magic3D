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
        bool isServer = Magic3D::getInstance()->getConfiguration().SERVER;
        if (!Magic3D::getInstance()->getConfiguration().ADDRESS.empty())
        {
            enet_address_set_host(&address, Magic3D::getInstance()->getConfiguration().ADDRESS.c_str());
        }
        else
        {
            address.host = ENET_HOST_ANY;
        }

        address.port = Magic3D::getInstance()->getConfiguration().PORT;

        server = enet_host_create (isServer ? &address : NULL, Magic3D::getInstance()->getConfiguration().CLIENTS, 2, 0, 0);
        if (server == NULL)
        {
            Log::log(eLOG_FAILURE, "An error occurred while trying to create an ENet server host.");
        }
        else if (!isServer && address.host != ENET_HOST_ANY)
        {            
            peer = enet_host_connect(server, &address, 1, 0);
            if(peer == NULL)
            {
                Log::log(eLOG_FAILURE, "No available peers for initializing an ENet connection.");
            }
            else
            {
                ENetEvent event;
                if(enet_host_service(server, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
                {
                    Log::logFormat(eLOG_SUCCESS, "Host connection %s successful!", Magic3D::getInstance()->getConfiguration().ADDRESS.c_str());
                }
                else
                {
                    Log::log(eLOG_FAILURE, "Host connection failed!");
                    enet_peer_reset(peer);
                }
            }
        }
    }
    return result;
}

bool Magic3D::Network::deinitialize()
{
    if (server)
    {
        enet_host_destroy(server);
    }
    enet_deinitialize();
    return true;
}

void Magic3D::Network::update()
{
    ENetEvent event;
    ENetPacket* packet;
    if (enet_host_service(server, &event, 0) > 0)
    {
        switch(event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                Log::logFormat(eLOG_SUCCESS, "A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
                clients.push_back(event.peer->address);

                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
            {
                /*printf ("A packet of length %u containing %s was received.\n",
                        event.packet->dataLength,
                        event.packet->data);

                packet = enet_packet_create(event.packet->data, event.packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
                enet_host_broadcast(server, 0, packet);
                enet_host_flush(server);

                enet_packet_destroy(event.packet);*/
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
                Log::logFormat(eLOG_PLAINTEXT, "Client disconnected %x:%u.\n", event.peer->address.host, event.peer->address.port);
                event.peer->data = NULL;
                break;
            }
        }
    }
}

int Magic3D::Network::getClientsCount()
{
    return clients.size();
}
