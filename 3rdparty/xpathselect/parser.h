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
#ifndef _PARSER_H
#define _PARSER_H

#include <string>
#include <cstdint>

#include <boost/config/warning_disable.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_bool.hpp>
#include <boost/spirit/include/qi_int.hpp>

#include "xpathquerypart.h"

// this allows spirit to lazily construct these two structs...
BOOST_FUSION_ADAPT_STRUCT(
    xpathselect::XPathQueryPart,
    (std::string, node_name_)
    (xpathselect::ParamList, parameter)
    );

BOOST_FUSION_ADAPT_STRUCT(
    xpathselect::XPathQueryParam,
    (std::string, param_name)
    (xpathselect::XPathQueryParam::ParamValueType, param_value)
    );

namespace xpathselect
{
namespace parser
{
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

    // python_bool_policy determines what can be considered truthy. We follow python
    // repr format.
    struct python_bool_policy : qi::bool_policies<>
    {
        template <typename Iterator, typename Attribute>
        static bool parse_true(Iterator& first, Iterator const& last, Attribute& attr)
        {
            if (qi::detail::string_parse("True", first, last, qi::unused))
            {
                boost::spirit::traits::assign_to(true, attr);    // result is true
                return true;    // parsing succeeded
            }
            return false;   // parsing failed
        }

        template <typename Iterator, typename Attribute>
        static bool parse_false(Iterator& first, Iterator const& last, Attribute& attr)
        {
            if (qi::detail::string_parse("False", first, last, qi::unused))
            {
                boost::spirit::traits::assign_to(false, attr);    // result is false
                return true;
            }
            return false;
        }
    };

    // This is the main XPath grammar. It looks horrible, until you emerse yourself in it for a few
    // days, then the beauty of boost::spirit creeps into your brain. To help future programmers,
    // I've heavily commented this.
    //
    // The first template parameter to this grammar defines the type of iterator the grammer will operate
    // on - it must adhere to std::forward_iterator. The second template parameter is the type
    // that this grammar will produce (in this case: a list of XPathQueryPart objects).
    template <typename Iterator>
    struct xpath_grammar : qi::grammar<Iterator, QueryList()>
    {
        xpath_grammar() : xpath_grammar::base_type(node_sequence) // node_sequence is the start rule.
        {
            using namespace qi::labels;

            // character escape codes. The input on the left will produce the output on
            // the right:
            unesc_char.add("\\a", '\a')
                        ("\\b", '\b')
                        ("\\f", '\f')
                        ("\\n", '\n')
                        ("\\r", '\r')
                        ("\\t", '\t')
                        ("\\v", '\v')
                        ("\\\\", '\\')
                        ("\\\'", '\'')
                        ("\\\"", '\"');

            unesc_str = '"' >> *(
                                    unesc_char |
                                    qi::alnum |
                                    qi::space |
                                    "\\x" >> qi::hex
                                ) >>  '"';

            unesc_str = '"'
                    >> *(unesc_char | "\\x" >> qi::hex | (qi::print - '"'))
                    >>  '"'
                    ;

            int_type = qi::int_parser<int32_t>();

            // Parameter grammar:
            // parameter name can contain some basic text (no spaces or '.')
            param_name = +qi::char_("a-zA-Z0-9_\\-");

            // parameter values can be several different types.
            // Alternatives are tried left to right, and the first match found is the one used.
            param_value = unesc_str | int_type |  bool_type;
            // parameter specification is simple: name=value
            param %= param_name >> '=' >> param_value;
            // a parameter list is a list of parameters separated by ',''s surrounded in '[...]'
            param_list = '[' >> param % ',' >> ']';


            // spec_node_name is a node name that has been explicitly specified.
            // it must start and end with a non-space character, but you can have
            // spaces in the middle.
            spec_node_name = +qi::char_("a-zA-Z0-9_\\-") >> *(+qi::char_(" :") >> +qi::char_("a-zA-Z0-9_\\-"));
            // a wildcard node name is simply a '*'
            wildcard_node_name = qi::char_("*");


            // a spec_node consists of a specified node name, followed by an *optional* parameter list.
            spec_node %= spec_node_name >> -(param_list);
            // a wildcard node is a '*' without parameters:
            wildcard_node %= wildcard_node_name >> !param_list;
            // wildcard nodes can also have parameters:
            wildcard_node_with_params %= wildcard_node_name >> param_list;
            // A parent node is '..' as long as it's followed by a normal separator or end of input:
            parent_node = qi::lit("..")[qi::_val = XPathQueryPart("..")];

            // node is simply any kind of code defined thus far:
            node = spec_node | wildcard_node_with_params | wildcard_node | parent_node;

            // a search node is '//' as long as it's followed by a spec node or a wildcard node with parameters.
            // we don't allow '//*' since it would match everything in the tree, and cause HUGE amounts of
            // data to be transmitted.
            search_node = "//" >> &(spec_node | wildcard_node_with_params)[qi::_val = XPathQueryPart()];


            // a normal separator is a '/' as long as it's followed by something other than another '/'
            normal_sep = '/' >> !qi::lit('/');
            separator = normal_sep | search_node;  // nodes can be separated by normal_sep or search_node.
            // this is the money shot: a node sequence is one or more of a separator, followed by an
            // optional node.
            node_sequence %= +(separator >> -node);

            // DEBUGGING SUPPORT:
            // define DEBUG in order to have boost::spirit spit out useful debug information:
#ifdef DEBUG
            // this gives english names to all the grammar rules:
            spec_node_name.name("spec_node_name");
            wildcard_node_name.name("wildcard_node_name");
            search_node.name("search_node");
            normal_sep.name("normal_separator");
            separator.name("separator");
            param_name.name("param_name");
            param_value.name("param_value");
            param.name("param");
            spec_node.name("spec_node");
            wildcard_node.name("wildcard_node");
            wildcard_node.name("wildcard_node_with_params");
            node.name("node");
            node_sequence.name("node_sequence");
            param_list.name("param_list");

            // set up error logging:
            qi::on_error<qi::fail>(
                node_sequence,
                std::cout
                    << phoenix::val("Error! Expecting ")
                    << qi::_4                               // what failed?
                    << phoenix::val(" here: \"")
                    << phoenix::construct<std::string>(qi::_3, qi::_2)   // iterators to error-pos, end
                    << phoenix::val("\"")
                    << std::endl
            );
            // specify which rules we want debug info about (all of them):
            qi::debug(spec_node_name);
            qi::debug(wildcard_node_name);
            qi::debug(search_node);
            qi::debug(normal_sep);
            qi::debug(separator);
            qi::debug(param_name);
            qi::debug(param_value);
            qi::debug(param);
            qi::debug(spec_node);
            qi::debug(wildcard_node);
            qi::debug(wildcard_node_with_params);
            qi::debug(node);
            qi::debug(node_sequence);
            qi::debug(param_list);
#endif
        }
        // declare all the rules. The second template parameter is the type they produce.
        // basic type rules:

        // parse python Boolean represetnations 'True' or 'False':
        qi::bool_parser<bool, python_bool_policy> bool_type;

        // parse an escaped byte string.
        qi::rule<Iterator, std::string()> unesc_str;
        // symbol table for chracter scape codes.
        qi::symbols<char const, char const> unesc_char;

        // parse integers, first signed then unsigned:
        qi::rule<Iterator, int32_t()> int_type;

        // more complicated language rules:
        qi::rule<Iterator, std::string()> spec_node_name;
        qi::rule<Iterator, std::string()> wildcard_node_name;
        qi::rule<Iterator, XPathQueryPart()> search_node;
        qi::rule<Iterator, XPathQueryPart()> parent_node;
        qi::rule<Iterator> normal_sep;
        qi::rule<Iterator, xpathselect::QueryList()> separator;

        qi::rule<Iterator, std::string()> param_name;
        qi::rule<Iterator, xpathselect::XPathQueryParam::ParamValueType()> param_value;
        qi::rule<Iterator, XPathQueryParam()> param;
        qi::rule<Iterator, xpathselect::ParamList()> param_list;

        qi::rule<Iterator, XPathQueryPart()> spec_node;
        qi::rule<Iterator, XPathQueryPart()> wildcard_node;
        qi::rule<Iterator, XPathQueryPart()> wildcard_node_with_params;
        qi::rule<Iterator, XPathQueryPart()> node;

        qi::rule<Iterator, xpathselect::QueryList()> node_sequence;
    };

}
}

#endif
