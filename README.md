# Utilities for Boost Property Tree

## merge_xml.h
Contains template function:

    bool merge(PTree & pt, const PTree & pt_update, const Match & match);
    
which updates `pt` by data from `pt_update` by adding new and overwriting existing tags recursively. `match` functor is used to find one-to-one correspondence between nodes with same keys on the same level.
If there any ambiguity is found, the function returns `false`.

Some predefined matching functors are provided:
* `match_by_child_value` - matches by value of a child node specified by relative path
* `match_by_xml_attribute` - convenience function for matching nodes of XML-originating property trees by single XML attribute value
* `match_by_xml_attributes_depending_on_path` - matches ptree nodes by several XML attribute values which (optionally) depend on absolute paths.
