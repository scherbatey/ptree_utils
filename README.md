# Utilities for Boost Property Tree

## merge_xml.h
Contains template function:

    bool merge(PTree & pt, const PTree & pt_update, const Match & match, const Filter * filter = nullptr);
    
which updates `pt` by data from `pt_update` by adding new and overwriting existing tags recursively. `match` functor is used to find one-to-one correspondence between nodes with same keys on the same level.
If any ambiguity is found, the function returns `false`.

Some predefined matching functors are provided:
* `match_by_child_value` - matches by value of a child node specified by relative path
* `match_by_xml_attribute` - convenience function for matching nodes of XML-originating property trees by single XML attribute value
* `match_by_xml_attributes_depending_on_path` - matches ptree nodes by several XML attribute values which (optionally) depend on absolute paths.

`filter`, if provided, performs advanced filtering of nodes from `pt_update` before merging them into `pt`. It should be a callable with the following signature:
    
    some_filter(const PTree * pt_node, const PTree & pt_update_node, PTree::key_type path)

Here `pt_node` and `pt_update_node` are matching nodes, `path` is the absolute path to `pt_node` in `pt`.
In the case of insertion of new node `pt_node` is `nullptr`
