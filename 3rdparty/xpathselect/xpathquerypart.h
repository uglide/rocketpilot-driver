/*
* Copyright (C) 2013 Canonical Ltd
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

#ifndef _XPATHQUERYPART_H
#define _XPATHQUERYPART_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include <boost/optional/optional.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>

#include "node.h"

namespace xpathselect
{
    // stores a parameter name, value pair.
    struct XPathQueryParam
    {
        typedef boost::variant<std::string,
                                bool,
                                int> ParamValueType;
        std::string param_name;
        ParamValueType param_value;
    };

    typedef std::vector<XPathQueryParam> ParamList;

    // Stores a part of an XPath query.
    struct XPathQueryPart
    {
    public:
        XPathQueryPart() {}
        XPathQueryPart(std::string node_name)
        : node_name_(node_name)
        {}

        enum class QueryPartType {Normal, Search, Parent};

        bool Matches(Node::Ptr const& node) const
        {
            bool matches = (node_name_ == "*" || node->GetName() == node_name_);
            if (!parameter.empty())
            {
                for (auto param : parameter)
                {
                    switch(param.param_value.which())
                    {
                        case 0:
                        {
                            matches &= node->MatchStringProperty(param.param_name, boost::get<std::string>(param.param_value));
                        }
                        break;
                        case 1:
                        {
                            matches &= node->MatchBooleanProperty(param.param_name, boost::get<bool>(param.param_value));
                        }
                        break;
                        case 2:
                        {
                            matches &= node->MatchIntegerProperty(param.param_name, boost::get<int>(param.param_value));
                        }
                        break;
                    }
                }
            }

            return matches;
        }

        QueryPartType Type() const
        {
            if (node_name_ == "")
                return QueryPartType::Search;
            else if (node_name_ == "..")
                return QueryPartType::Parent;
            else
                return QueryPartType::Normal;
        }

        void Dump() const
        {
            if (Type() == QueryPartType::Search)
                std::cout << "<search> ";
            else
                std::cout << "[" << node_name_ << "] ";
        }

        std::string node_name_;
        ParamList parameter;
    };



    typedef std::vector<XPathQueryPart> QueryList;

}

#endif
