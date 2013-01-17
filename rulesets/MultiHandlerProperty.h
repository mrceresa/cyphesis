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

#ifndef RULESETS_MULTI_HANDLER_PROPERTY_H
#define RULESETS_MULTI_HANDLER_PROPERTY_H

#include "common/Property.h"

#include <map>

typedef HandlerResult (*Handler)(LocatedEntity *,
                                 const Operation &,
                                 OpVector &);

typedef std::map<int, Handler> HandlerMap;

/// \brief Class to handle a property that triggers a many handlers.
/// \ingroup PropertyClasses
template <typename T>
class MultiHandlerProperty : public Property<T> {
  protected:
    HandlerMap m_handlers;
  public:
    explicit MultiHandlerProperty(const HandlerMap & handlers);

    virtual MultiHandlerProperty<T> * copy() const;

    virtual void install(LocatedEntity *);
};

#endif // RULESETS_MULTI_HANDLER_PROPERTY_H
