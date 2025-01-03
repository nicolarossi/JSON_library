#pragma once

#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <concepts>
#include <iomanip>
#include <optional>
#include <sstream>
#include <stack>
#include <type_traits>

#include "Serializer.h"
#include "concepts.h"

namespace persistence {

class Json_Writer {
  std::stack<bool> first;
  void manage_comma() {
    if (first.empty()) {
      first.push(true);
    }
    if (first.top()) {
      first.top() = false;
    } else {
      os << ",";
    }
  }

 public:
  using os_type = std::ostream;
  Json_Writer() = delete;
  Json_Writer(os_type& os) : os(os) {};
  void comment(std::string const& x) { os << "/*" << x << "*/"; }
  void start_container() {
    first.push(true);
    os << "[";
  }
  void end_container() {
    first.pop();
    os << "]";
  }
  void start_composite() {
    first.push(true);
    os << "{";
  }
  void end_composite() {
    os << "}";
    first.pop();
  }

  void new_element() { manage_comma(); }
  template <typename S, typename T>
    requires std::is_convertible_v<S, std::string_view>
  void write_id(
      S name,
      T const&) {  // A Writer can be interested to the size of the object
    os << std::quoted(name);
    os << ":";
  }

  template <typename T>
    requires std::is_convertible_v<T, std::string_view>
  void write(T x) {
    os << std::quoted(x);
  }

  void write(bool x) {
    os << std::boolalpha;
    os << x;
  }

  template <typename T>
    requires std::integral<T>
  void write(T x) {
    os << x;
  }

  template <typename F>
    requires std::floating_point<F>
  void write(F x) {
    os << std::setprecision(4);
    os << x;
  }

  template <Is_Enum_Boost_Described E>
  void write(E x) {
    boost::mp11::mp_for_each<boost::describe::describe_enumerators<E> >(
        [&](auto D) {
          if (x == D.value) {
            os << std::quoted(D.name);
          }
        });
  }

  template <typename E>
    requires std::is_enum_v<E> && (!Is_Enum_Boost_Described<E>)
  void write(E x) {
    os << static_cast<std::underlying_type<E>::type>(x);
  }

  os_type& os;
};
}  // namespace persistence
