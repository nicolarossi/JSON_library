#pragma once

#include <concepts>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string_view>
#include <type_traits>

#include "Json_Writer.h"
#include "Serializer.h"

template <typename T>
concept Forward_Container = requires(T t) {
  ++t.begin();
  t.begin() != t.end();
} && (!std::is_convertible_v<T, std::string_view>);

template <typename T>
concept To_Stringable = requires(T a) { to_string(a); } || std::integral<T>;

template <typename W>
concept Is_Writer = requires(W w) {
  w.start_container();
  w.end_container();
  w.start_composite();
  w.end_composite();
  w.write_id("x", 42);
  w.write(42);
};

template <typename S>
concept Is_Serializer = requires(S s) {
  s.write_id(std::string("name"), 42);
  s.serialize(42);
};

template <typename T, typename Serializer>
concept Is_Serializable =
    requires(T obj, Serializer serializer) { obj.serialize(serializer); };

template <typename T>
concept Is_Primitive = std::is_integral_v<T> || std::is_floating_point_v<T>;

template <typename T>
concept Is_Enum_Boost_Described = requires(T obj) {
  boost::mp11::mp_for_each<boost::describe::describe_enumerators<T> >([]() {});
};
