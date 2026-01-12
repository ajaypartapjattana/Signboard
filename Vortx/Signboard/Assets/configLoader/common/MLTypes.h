#pragma once

#include <string>
#include <vector>

struct ML_Attribute {
	std::string name;
	std::string value;
};

struct ML_Node {
	std::string name;
	std::vector<ML_Attribute> attributes;
	std::vector<ML_Node> children;
};