#pragma once

#include "ReadTypes/ApplicationReadTypes.h"
#include "common/MLTypes.h"

class ConfigLoader {
public:
	void Build_StartupConfig(const std::string& path, StartupConfig& config);

private:
	const ML_Attribute* FindAttr(const ML_Node& node, const std::string& name);
	const ML_Node* FindChild(const ML_Node& node, const std::string& name);

};