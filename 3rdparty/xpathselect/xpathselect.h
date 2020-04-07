/*
* Copyright (C) 2012 Canonical Ltd
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 3 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef _XPATHSELECT_H
#define _XPATHSELECT_H

#include "node.h"

namespace xpathselect
{
    /// Search the node tree beginning with 'root' and return nodes that
    /// match 'query'.    
    #if !(defined(WIN32) | defined(WIN64))
    extern "C"
    #endif
    NodeVector SelectNodes(Node::Ptr const& root, std::string query);
}

#endif
