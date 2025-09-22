/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtl_lib

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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