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

#ifndef RULESETS_HANDLER_PROPERTY_IMPL_H
#define RULESETS_HANDLER_PROPERTY_IMPL_H

#include "HandlerProperty.h"

#include "Entity.h"

template <typename T>
HandlerProperty<T>::HandlerProperty(int op, Handler handler) :
                                    m_operationClassNo(op),
                                    m_handler(handler)
{
}

template <typename T>
HandlerProperty<T> * HandlerProperty<T>::copy() const
{
    return new HandlerProperty<T>(*this);
}

template <typename T>
void HandlerProperty<T>::install(LocatedEntity * ent)
{
    ent->installHandler(m_operationClassNo, m_handler);
}

#endif // RULESETS_HANDLER_PROPERTY_IMPL_H
