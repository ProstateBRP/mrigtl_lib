#pragma once

#include <string>
#include <map>
#include <vector>

namespace mrigtlbridge {

// Signal names and their parameter types
extern std::map<std::string, std::string> SignalNames;

// Data type table for OpenIGTLink
extern std::map<std::string, std::vector<int>> DataTypeTable;

} // namespace mrigtlbridge