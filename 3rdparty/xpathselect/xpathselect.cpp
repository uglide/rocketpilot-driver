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

#include <vector>
#include <queue>


#include "xpathselect.h"
#include "xpathquerypart.h"
#include "parser.h"

namespace xpathselect
{
    // anonymous namespace for internal-only utility class:
    namespace
    {
        QueryList GetQueryPartsFromQuery(std::string const& query)
        {
            xpathselect::parser::xpath_grammar<std::string::const_iterator> grammar;
            QueryList query_parts;

            auto begin = query.cbegin();
            auto end = query.cend();
            if (boost::spirit::qi::parse(begin, end, grammar, query_parts) && (begin == end))
            {
#ifdef DEBUG
                std::cout << "Query parts are: ";
                for (auto n : query_parts)
                    n.Dump();
                std::cout << std::endl;
#endif
                return query_parts;
            }
#ifdef DEBUG
            std::cout << "Query failed." << std::endl;
#endif
            return QueryList();
        }

        // Starting at each node listed in 'start_points', search the tree for nodes that match
        // 'next_match'. next_match *must* be a normal query part object, not a search token.
        NodeList SearchTreeForNode(NodeList const& start_points, XPathQueryPart const& next_match)
        {
            NodeList matches;
            for (auto root: start_points)
            {
                // non-recursive BFS traversal to find starting points:
                std::queue<Node::Ptr> queue;
                queue.push(root);
                while (!queue.empty())
                {
                    Node::Ptr node = queue.front();
                    queue.pop();
                    if (next_match.Matches(node))
                    {
                        // found one. We keep going deeper, as there may be another node beneath this one
                        // with the same node name.
                        matches.push_back(node);
                    }
                    // Add all children of current node to queue.
                    for(Node::Ptr child : node->Children())
                    {
                        queue.push(child);
                    }
                }
            }
            return matches;
        }
    } // end of anonymous namespace

    NodeVector SelectNodes(Node::Ptr const& root, std::string query)
    {
        // allow users to be lazy when specifying tree root:
        if (query == "" || query == "/" || query == "//")
        {
            query = "/" + root->GetName();
        }

        QueryList query_parts = GetQueryPartsFromQuery(query);
        if (query_parts.empty())
            return NodeVector();
        auto query_part = query_parts.cbegin();
        NodeList start_nodes { root };
        while (query_part != query_parts.cend())
        {
            // If the current query piece is a recursive search token ('//')...
            if (query_part->Type() == XPathQueryPart::QueryPartType::Search)
            {
                // advance to look at the next piece.
                ++query_part;
                // do some sanity checking...
                if (query_part->Type() == XPathQueryPart::QueryPartType::Search)
                    // invalid query - cannot specify multiple search sequences in a row.
                    return NodeVector();
                // then find all the nodes that match the new query part, and store them as
                // the new start nodes. We pass in 'start_nodes' rather than 'root' since
                // there's a chance we'll be doing more than one search in different parts of the tree.
                start_nodes = SearchTreeForNode(start_nodes, *query_part);
            }
            else if (query_part->Type() == XPathQueryPart::QueryPartType::Parent)
            {
                // This part of the query selects the parent node. If the current node has no
                // parent (i.e.- we're already at the root of the tree) then this is a no-op:
                NodeList new_start_nodes;
                for (auto n: start_nodes)
                {
                    auto parent = n->GetParent();
                    new_start_nodes.push_back(parent ? parent : n);
                }
                start_nodes = new_start_nodes;
            }
            else
            {
                // this isn't a search token. Look at each node in the start_nodes list,
                // and discard any that don't match the current query part.
                // C++11 is the shit:
                start_nodes.erase(
                    std::remove_if(
                        start_nodes.begin(),
                        start_nodes.end(),
                        [query_part](Node::Ptr n) -> bool {
                            return ! query_part->Matches(n);
                        }
                        ),
                    start_nodes.end()
                    );
            }
            // then replace each node still in the list with all it's children.
            // ... but only if we're not on the last query part, and only if the
            // next query part is not a parent node...
            auto next_query_part = query_part + 1;
            if (next_query_part != query_parts.cend()
                && next_query_part->Type() != XPathQueryPart::QueryPartType::Parent)
            {
                NodeList new_start_nodes;
                for (auto node: start_nodes)
                {
                    auto children = node->Children();
                    if (children.size())
                    {
                        new_start_nodes.insert(
                            new_start_nodes.end(),
                            children.begin(),
                            children.end());
                    }
                }
            start_nodes = new_start_nodes;
            }
            ++query_part;
        }
        // remove duplicate nodes by sorting & unique'ing:
        // we could probably do this better, but since start_nodes is
        // typically very small at this stage, I'm not sure it's worth it:
        start_nodes.sort([](Node::Ptr a, Node::Ptr b) -> bool {
            return a->GetId() < b->GetId();
        });
        start_nodes.unique([](Node::Ptr a, Node::Ptr b) -> bool {
            return a->GetId() == b->GetId();
        });

        return NodeVector(start_nodes.begin(), start_nodes.end());
    }
}
