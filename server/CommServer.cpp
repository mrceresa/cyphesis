// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommServer.h"

#include "CommClient.h"
#include "CommMetaClient.h"
#include "ServerRouting_methods.h"
#include "protocol_instructions.h"

#include "common/log.h"
#include "common/debug.h"

#include <iostream>

static const bool debug_flag = false;

CommServer::CommServer(ServerRouting & svr) : server(svr)
{
}

CommServer::~CommServer()
{
    comm_set_t::const_iterator I = m_sockets.begin();
    for(; I != m_sockets.end(); I++) {
        delete *I;
    }
}

inline bool CommServer::idle()
{
    // Update the time, and get the core server object to process
    // stuff.
    time_t ctime = time(NULL);
    commi_set_t::const_iterator I;
    for(I = m_idleSockets.begin(); I != m_idleSockets.end(); ++I) {
        (*I)->idle(ctime);
    }
    // server.idle() is inlined, and simply calls the world idle method,
    // which is not directly accessible from here.
    return server.idle();
}

void CommServer::loop()
{
    // This is the main code loop.
    // Classic select code for checking incoming data or client connections.
    // It may be beneficial to re-write this code to use the poll(2) system
    // call.
    bool busy = idle();

    fd_set sock_fds;
    int highest = 0;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = (busy ? 0 : 100000);

    FD_ZERO(&sock_fds);

    bool pendingConnections = false;
    comm_set_t::const_iterator I = m_sockets.begin();
    for(; I != m_sockets.end(); I++) {
       if (!(*I)->isOpen()) {
           pendingConnections = true;
           continue;
       }
       int client_fd = (*I)->getFd();
       FD_SET(client_fd, &sock_fds);
       if (client_fd > highest) {
           highest = client_fd;
       }
    }
    highest++;
    int rval = ::select(highest, &sock_fds, NULL, NULL, &tv);

    if (rval < 0) {
        if (errno != EINTR) {
            perror("select");
            log(ERROR, "Error caused by select() in main loop");
        }
        return;
    }

    if ((rval == 0) && !pendingConnections) {
        return;
    }
    
    std::set<CommSocket *> obsoleteConnections;
    for(I = m_sockets.begin(); I != m_sockets.end(); I++) {
       CommSocket * client = *I;
       if (!client->isOpen()) {
           obsoleteConnections.insert(client);
           continue;
       }
       if (FD_ISSET(client->getFd(), &sock_fds)) {
           if (!client->eof()) {
               if (client->read()) {
                   debug(std::cout << "Removing client due to failed negotiation or timeout" << std::endl << std::flush;);
                   obsoleteConnections.insert(client);
               }
               client->dispatch();
           } else {
               // It is not clear why but on some implementation/circumstances
               // client->eof() is true, and sometimes it isn't.
               // Either way, the stream is now done, and we should remove it
               obsoleteConnections.insert(client);
           }
       }
    }
    std::set<CommSocket *>::const_iterator J = obsoleteConnections.begin();
    for(; J != obsoleteConnections.end(); ++J) {
        removeSocket(*J);
    }
}

inline void CommServer::removeSocket(CommSocket * client, char * error_msg)
{
    // FIXME This code needs to be moved into CommClient
    // Atlas::Message::Element::MapType err;
    // err["message"] = error_msg;
    // Atlas::Message::Element::ListType eargs(1,err);

    // Error e;

    // e.setArgs(eargs);

    // if (client->online() && client->isOpen()) {
        // client->send(e);
    // }
    m_sockets.erase(client);
    delete client;
}

void CommServer::removeSocket(CommSocket * client)
{
    removeSocket(client,"You caused exception. Connection closed");
}
