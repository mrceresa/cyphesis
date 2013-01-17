// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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

#ifndef RULESETS_MULTI_HANDLER_PROPERTY_IMPL_H
#define RULESETS_MULTI_HANDLER_PROPERTY_IMPL_H

#include "MultiHandlerProperty.h"

#include "Entity.h"

template <typename T>
MultiHandlerProperty<T>::MultiHandlerProperty(const HandlerMap & handlers) :
                                    m_handlers(handlers)
{
}

template <typename T>
MultiHandlerProperty<T> * MultiHandlerProperty<T>::copy() const
{
    return new MultiHandlerProperty<T>(*this);
}

template <typename T>
void MultiHandlerProperty<T>::install(LocatedEntity * ent)
{
    HandlerMap::const_iterator I = m_handlers.begin();
    HandlerMap::const_iterator Iend = m_handlers.end();
    for (; I != Iend; ++I) {
        ent->installHandler(I->first, I->second);
    }
}

#endif // RULESETS_MULTI_HANDLER_PROPERTY_IMPL_H
