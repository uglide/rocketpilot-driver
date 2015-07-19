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

#ifndef _NODE_H
#define _NODE_H

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <cstdint>

namespace xpathselect
{
    /// Represents a node in the object tree. Provide an implementation of
    /// this class in your own code.
    class Node
    {
    public:
        typedef std::shared_ptr<const Node> Ptr;

        /// Get the node's name.
        virtual std::string GetName() const =0;

        /// Get the node's full path
        virtual std::string GetPath() const =0;

        /// Get this node's ID.
        virtual int32_t GetId() const =0;

        virtual bool MatchBooleanProperty(const std::string& name, bool value) const =0;
        virtual bool MatchIntegerProperty(const std::string& name, int32_t value) const =0;
        virtual bool MatchStringProperty(const std::string& name, const std::string& value) const =0;

        /// Return a list of the children of this node.
        virtual std::vector<Node::Ptr> Children() const =0;

        /// Return a pointer to the parent class.
        virtual Node::Ptr GetParent() const =0;
    };

    /// NodeList is how we return lists of nodes.
    typedef std::vector<Node::Ptr> NodeVector;
    typedef std::list<Node::Ptr> NodeList;
}

#endif
