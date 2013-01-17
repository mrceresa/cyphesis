// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef SERVER_TELEPORT_AUTHENTICATOR_H
#define SERVER_TELEPORT_AUTHENTICATOR_H

#include <string>
#include <map>

class LocatedEntity;
class PendingTeleport;

/// \brief Map of teleported entity IDs and their PendingState objects
typedef std::map<std::string, PendingTeleport *> PendingTeleportMap;

/// \brief A class that stores and authenticates teleport requests
class TeleportAuthenticator
{
    /// \brief An instance pointer for singleton behaviour
    static TeleportAuthenticator * m_instance;
    /// \brief Map of teleport requests
    PendingTeleportMap m_teleports;

    public:

    static void init() {
        if(m_instance == 0) {
            m_instance = new TeleportAuthenticator();
        }
    }
    static TeleportAuthenticator * instance() {
        return m_instance;
    }
    static void del() {
        if (m_instance != 0) {
            delete m_instance;
            m_instance = 0;
        }
    }

    /// \brief Checks if there is a pending teleport on an account
    bool isPending(const std::string &);

    /// \brief Add a teleport authentication entry
    int addTeleport(const std::string &, const std::string &);

    /// \brief Remove a teleport authentications entry. Typically after a
    ///        successful authentication
    int removeTeleport(const std::string &);

    /// \brief Remove a teleport authentications entry. Typically after a
    ///        successful authentication
    int removeTeleport(PendingTeleportMap::iterator I);

    /// \brief Authenticate a teleport request
    LocatedEntity *authenticateTeleport(const std::string &,
                                        const std::string &);

    friend class TeleportAuthenticatortest;
};

#endif // SERVER_TELEPORT_AUTHENTICATOR_H
