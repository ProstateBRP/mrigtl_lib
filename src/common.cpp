/*=========================================================================

  Program:   mrigtlbridge
  Language:  C++
  Web page:  https://github.com/ProstateBRP/mrigtlbridge_cpp

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "common.h"

namespace mrigtlbridge {

std::map<std::string, std::string> SignalNames = {
    // For IGTL GUI
    {"consoleTextIGTL", "str"},
    // For IGTL Listener
    {"disconnectIGTL", ""},
    {"sendImageIGTL", "dict"},
    {"sendTrackingDataIGTL", "dict"},
    
    // For MR GUI
    {"consoleTextMR", "str"},

    // Signals for listeners to notify GUI of its connection/thread status
    {"listenerConnected", "str"},
    {"listenerDisconnected", "str"},
    {"listenerTerminated", "str"},
    
    // For MR Listener
    {"startSequence", ""},
    {"stopSequence", ""},
    {"updateScanPlane", "dict"}
};

std::map<std::string, std::vector<int>> DataTypeTable = {
    {"int8",    {2, 1}},   //TYPE_INT8    = 2, 1 byte
    {"uint8",   {3, 1}},   //TYPE_UINT8   = 3, 1 byte
    {"int16",   {4, 2}},   //TYPE_INT16   = 4, 2 bytes
    {"uint16",  {5, 2}},   //TYPE_UINT16  = 5, 2 bytes
    {"int32",   {6, 4}},   //TYPE_INT32   = 6, 4 bytes
    {"uint32",  {7, 4}},   //TYPE_UINT32  = 7, 4 bytes
    {"float32", {10, 4}},  //TYPE_FLOAT32 = 10, 4 bytes 
    {"float64", {11, 8}}   //TYPE_FLOAT64 = 11, 8 bytes
};

} // namespace mrigtlbridge
