#pragma once
#include <string>
#include "concepts.h"

namespace json_library {

class Json_Serializator {
 public:
  Json_Serializator() = default;
  ~Json_Serializator() = default;



  std::string_view operator()(int val) const {
    return std::to_string(val);
  };

  
};

} /* namespace json_library */
