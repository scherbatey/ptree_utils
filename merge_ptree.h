// (c) Alexander Scherbatey 2018 
// https://github.com/scherbatey/ptree_utils.git

#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <set>
#include <functional>

namespace ptree_utils {

template<class PTree>
struct match_by_child_value 
{
	explicit match_by_child_value(const typename PTree::path_type & path_to_child) 
		: path_to_child(path_to_child)
	{
	}
	
	bool operator () (const PTree & pt1, const PTree & pt2, const typename PTree::key_type & path) const
	{
		const auto pt1_child = pt1.get_child_optional(path_to_child);
		const auto pt2_child = pt2.get_child_optional(path_to_child);
		if (!pt1_child && !pt2_child) 
			return true;
		if (pt1_child && pt2_child) 
		{
			const auto & data1 = pt1_child->data();
			const auto & data2 = pt2_child->data();
			return data1 == data2;
		}
		return false;
	}

	const typename PTree::path_type path_to_child;
};


template<class PTree>
match_by_child_value<PTree> match_by_xml_attribute(const typename PTree::key_type & attr_name)
{
	return match_by_child_value<PTree>(PTree::path_type(boost::lexical_cast<PTree::key_type>("<xmlattr>")) / PTree::path_type(attr_name));
}


template<class PTree, class OperationT = std::logical_and<bool>>
class match_by_xml_attributes_depending_on_path
{
public:
	typedef typename PTree::key_type key_type;
	
	explicit match_by_xml_attributes_depending_on_path(const key_type & attr, const key_type & path = key_type(), const OperationT & op = OperationT())
		: op(op)
	{
		path_to_match_map.push_back(std::make_pair(path, match_by_xml_attribute<PTree>(attr)));
	}
	
	template <class Iterator>
	match_by_xml_attributes_depending_on_path(Iterator begin, Iterator end, const OperationT & op = OperationT()) 
		: op(op)
	{
		for (auto it = begin; it != end; ++it)
			path_to_match_map.push_back(std::make_pair(it->second, match_by_xml_attribute<PTree>(it->first)));
	}

	void add(const key_type & attr, const key_type & path)
	{
		path_to_match_map.push_back(std::make_pair(path, match_by_xml_attribute<PTree>(attr)));
	}

	bool operator () (const PTree & pt1, const PTree & pt2, const key_type & path) const
	{
		bool result = true;
		BOOST_FOREACH(const auto & entry, path_to_match_map) 
		{
			if (entry.first.empty() || entry.first == path) 
				result = op(result, entry.second(pt1, pt2, path));
		}
		return result;
	}

private:
	std::list<std::pair<typename PTree::key_type, match_by_child_value<PTree>>> path_to_match_map;
	const OperationT op;
};


template<class PTree, class M>
bool merge(PTree & pt, const PTree & pt_update, const M & match_nodes, const typename PTree::key_type & start_path = typename PTree::key_type())
{
	std::set<const PTree::value_type *> updated_nodes;

	BOOST_FOREACH (const PTree::value_type & node_update, pt_update) {
		auto matching_node_it = pt.end();
		
		const auto path = start_path.empty() ? node_update.first : start_path + PTree::key_type::value_type('.') + node_update.first;
		
		const auto range = pt.equal_range(node_update.first);

		for (auto as_it = range.first; as_it != range.second; ++as_it) {
			if (match_nodes(as_it->second, node_update.second, path)) {
				if (matching_node_it == pt.end()) {
					const auto it = pt.to_iterator(as_it);
					if (!updated_nodes.count(&*it)) {
						matching_node_it = it;
						updated_nodes.insert(&*it);
					}
					else
						return false; // More than one matching nodes are found in source ptree
				}
				else
					return false; // More than one matching nodes are found in target ptree
			}
		}
		
		if (matching_node_it != pt.end()) {
			if (!merge(matching_node_it->second, node_update.second, match_nodes, path))
				return false;
		} else
			pt.insert(pt.end(), node_update);
	}

	pt.data() = pt_update.data();
	
	return true;
}

} // namespace ptree_utils
