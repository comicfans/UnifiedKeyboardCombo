
#ifndef TREE_TYPE_HPP
#define TREE_TYPE_HPP

#include <boost/property_tree/ptree_fwd.hpp>

#ifdef _WIN32
    using TreeType=boost::property_tree::wptree;
#else
    using TreeType=boost::property_tree::ptree;

#endif

#endif
