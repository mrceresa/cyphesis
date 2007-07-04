// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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

// $Id: Delve.h,v 1.11 2007-07-04 21:20:40 alriddoch Exp $

#error This file has been removed from the build
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef COMMON_DELVE_H
#define COMMON_DELVE_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int DELVE_NO;

/// \brief An affect operation used to gather rock resources from the terrain.
/// \ingroup CustomOperations
class Delve : public Generic
{
  public:
    Delve() {
        (*this)->setType("delve", DELVE_NO);
    }
};

} } }

#endif // COMMON_DELVE_H
