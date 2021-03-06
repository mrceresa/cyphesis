// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id$

#include "MemEntity.h"

static const bool debug_flag = false;

MemEntity::MemEntity(const std::string & id, long intId) :
           LocatedEntity(id, intId), m_lastSeen(0.)
{
}

MemEntity::~MemEntity()
{
}

void MemEntity::externalOperation(const Operation & op, Link &)
{
}

void MemEntity::operation(const Operation &, OpVector &)
{
}

void MemEntity::destroy()
{
     // Handling re-parenting is done very similarly to Entity::destroy,
     // but is slightly different as we tolerate LOC being null.
     LocatedEntity * ent_loc = this->m_location.m_loc;
     if (ent_loc != 0) {
         // Remove deleted entity from its parents contains
         assert(ent_loc->m_contains != 0);
         ent_loc->m_contains->erase(this);
     }
     // FIXME This is required until MemMap uses parent refcounting
     this->m_location.m_loc = 0;

     if (this->m_contains != 0) {
         // Add deleted entity's children into its parents contains
         LocatedEntitySet::const_iterator K = this->m_contains->begin();
         LocatedEntitySet::const_iterator Kend = this->m_contains->end();
         for (; K != Kend; ++K) {
             LocatedEntity * child_ent = *K;
             child_ent->m_location.m_loc = ent_loc;
             // FIXME adjust pos and:
             // FIXME take account of orientation
             if (ent_loc != 0) {
                 ent_loc->m_contains->insert(child_ent);
             }
         }
     }
}
