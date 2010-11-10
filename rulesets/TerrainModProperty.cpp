// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#include "TerrainModProperty.h"

#include "Entity.h"
#include "TerrainMod.h"
#include "TerrainProperty.h"

#include "common/compose.hpp"
#include "common/log.h"
#include "common/debug.h"

#include "modules/Location.h"
#include "modules/TerrainContext.h"

#include <Mercator/TerrainMod.h>

#include <sstream>

#include <cassert>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

/// \brief TerrainModProperty constructor
///
TerrainModProperty::TerrainModProperty(const HandlerMap & handlers) :
                    m_modptr(0), m_handlers(handlers), m_innerMod(0)
{
}

TerrainModProperty::~TerrainModProperty()
{
	// TODO remove the mod from the terrain
    // This is already covered from the Delete op handler when
    // the entity is deleted
}

bool TerrainModProperty::get(Element & ent) const
{
	///NOTE: what does this do? /erik
    MapType & mod = (ent = MapType()).Map();
    mod = m_data;
    return true;
}

void TerrainModProperty::set(const Element & ent)
{
    if (ent.isMap()) {
        const MapType & mod = ent.Map();
        m_data = mod;
    }

}

// void TerrainModProperty::setPos(const Point3D & newPos)
// {
//     if (m_owner != NULL) {
// 
//         TerrainProperty * terr = NULL;
//         // Search for an entity with the terrain property
//         terr = getTerrain();
// 
//         if (terr != NULL) {
// 
//             // If we're updating an existing mod, remove it from the terrain first
//             if (m_modptr != NULL) {
//                 terr->removeMod(m_modptr);
//             }
// 
//             // Parse the Atlas data for our mod, using the new position
//             Mercator::TerrainMod *newMod = parseModData(m_terrainmods);
// 
//             if (newMod != NULL) {
//                 // Apply the new mod to the terrain; retain the returned pointer
//                 m_modptr = terr->setMod(newMod);
//             } else {
//                 m_modptr = NULL;
//                 log(ERROR, "Terrain Modifier could not be parsed!");
//             }
//         }
//     }
// }

const TerrainProperty * TerrainModProperty::getTerrain(Entity * owner)
{
    const PropertyBase * terr;
    LocatedEntity * ent = owner;

    while ( (terr = ent->getProperty("terrain")) == NULL) {
        ent = ent->m_location.m_loc;
        if (ent == NULL) {
            return NULL;
        }
    }

    const TerrainProperty * tp = dynamic_cast<const TerrainProperty*>(terr);
    return tp;
}

void TerrainModProperty::add(const std::string & s, MapType & ent) const
{
    get(ent[s]);
}

void TerrainModProperty::install(Entity * owner)
{
    HandlerMap::const_iterator I = m_handlers.begin();
    HandlerMap::const_iterator Iend = m_handlers.end();
    for (; I != Iend; ++I) {
        owner->installHandler(I->first, I->second);
    }
}

void TerrainModProperty::apply(Entity * owner)
{
    // Find the terrain
    const TerrainProperty * terrain = NULL;
    terrain = getTerrain(owner);

    if (terrain == NULL) {
        log(ERROR, "Terrain Modifier could not find terrain");
        return;
    }

    // If we're updating an existing mod, remove it from the terrain first
    remove(owner);

    // Parse the Atlas data for our mod
    m_modptr = parseModData(owner, m_data);

    if (m_modptr != NULL) {
        // Apply the new mod to the terrain; retain the returned pointer
        terrain->addMod(m_modptr);
        m_modptr->setContext(new TerrainContext(owner));
        m_modptr->context()->setId(owner->getId());
        log(ERROR, "Terrain Modifier could not be parsed!");
    }
}

void TerrainModProperty::move(Entity* owner, const Point3D & newPos)
{
    remove(owner);
    const TerrainProperty* terrain = getTerrain(owner);
    if (terrain) {
        Mercator::TerrainMod* modifier = parseModData(owner, m_data);
        if (modifier) {
            terrain->addMod(modifier);
        }
    }
    
}

void TerrainModProperty::remove(Entity * owner)
{
    if (m_modptr) {
        const TerrainProperty* terrain = getTerrain(owner);
        if (terrain) {
            terrain->removeMod(m_modptr);
        }
    }
    m_modptr = 0;
    if (m_innerMod) {
        delete m_innerMod;
        m_innerMod = 0;
    }
}

Mercator::TerrainMod * TerrainModProperty::parseModData(Entity * owner,
                                                        const MapType & modMap)
{
    // Get modifier type
    MapType::const_iterator I = modMap.find("type");
    if (I == modMap.end() || !I->second.isString()) {
        log(WARNING, "Terrain mod data has no type");
        return 0;
    }
    const std::string& modType = I->second.String();

    if (m_innerMod != 0) {
        log(INFO, String::compose("Checking type of existing mod %1, %2",
                                  m_innerMod->getTypename(), modType));
        if (m_innerMod->getTypename() != modType) {
            log(WARNING, "Terrain mod type has changed");
            delete m_innerMod;
            m_innerMod = 0;
        }
    }

    if (m_innerMod == 0) {
        // TODO(alriddoch, 2010-10-19) m_innerMod is being leaked)
        m_innerMod = new InnerTerrainMod(modType);
    }

    if (m_innerMod->parseData(owner->m_location.pos(),
                              owner->m_location.orientation(), modMap)) {
        return m_innerMod->getModifier();
    }

    return NULL;
}

int TerrainModProperty::getAttr(const std::string & name,
                                Element & val)
{
    MapType::const_iterator I = m_data.find(name);
    if (I != m_data.end()) {
        val = I->second;
        return 0;
    }
    return -1;
}

void TerrainModProperty::setAttr(const std::string & name,
                                 const Element & val)
{
    m_data[name] = val;
}

