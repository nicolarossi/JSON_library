#pragma once

#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <concepts>
#include <iomanip>
#include <sstream>

#include "concepts.h"

namespace persistence {
namespace internal {
template <typename Stream>
void manage_comma(Stream& os, bool& first) {
  if (!first) {
    os << ", ";
  }
  first = false;
};

template <typename Stream>
void dump_in_json(Stream& os, bool t) {
  if (t) {
    os << std::quoted("true");
  } else {
    os << std::quoted("false");
  }
}

template <typename Stream>
void dump_in_json(Stream& os, std::string const& t) {
  os << std::quoted(t);
}

template <typename Stream, To_Stringable T>
void dump_in_json(Stream& os, T t) {
  dump_in_json(os, std::to_string(t));
}

template <typename Stream, std::floating_point T>
void dump_in_json(Stream& os, T const& t) {
  std::stringstream ss;
  ss << std ::setprecision(4);
  ss << t;
  dump_in_json(os, ss.str());
}

template <typename Stream, Is_Enum E>
void dump_in_json(Stream& os, E const& obj) {
  boost::mp11::mp_for_each<boost::describe::describe_enumerators<E> >(
      [&](auto D) {
        if (obj == D.value) {
          os << std::quoted(D.name);
        }
      });
}

/* Forward declarations of functions on template class */
template <typename Stream, Forward_Container Container>
void dump_in_json(Stream& os, Container const& t);

template <typename Stream, typename T>
void dump_in_json(Stream& os, std::optional<T> const& t);

/* Function on a generic class described with BOOST.Describe.
 *
 * The parameter flat_object is an internal parameter needed to 'flat' the
 * inheritance of the classes.
 * eg :
 * struct Base { int x{42}; };
 * struct Derived : public Base { int y{21}; }
 *
 * Derived in JSON format is '{ "x" : "42", "y" : "21" }'
 *
 */

template <
    typename Stream, typename Y,
    class Bd =
        boost::describe::describe_bases<Y, boost::describe::mod_any_access>,
    class Md =
        boost::describe::describe_members<Y, boost::describe::mod_any_access>,
    class En = std::enable_if_t<!std::is_union<Y>::value> >
void dump_in_json(Stream& os, Y const& obj, bool flat_object = false) {
  if (!flat_object) {
    os << "{";
  }

  bool first = true;

  boost::mp11::mp_for_each<Bd>([&](auto base) {
    manage_comma(os, first);

    using Base_Type = typename decltype(base)::type;

    Base_Type const& base_part = (Base_Type const&)(obj);

    dump_in_json(os, base_part, true);
  });

  boost::mp11::mp_for_each<Md>([&](auto D) {
    manage_comma(os, first);

    os << std::quoted(D.name) << " : ";

    dump_in_json(os, (obj).*D.pointer);
  });
  if (!flat_object) {
    os << "}";
  }
}

/* Definition of composite functions */
template <typename Stream, Forward_Container Container>
void dump_in_json(Stream& os, Container const& t) {
  os << " [ ";
  bool first = true;
  for (auto const& el : t) {
    manage_comma(os, first);

    dump_in_json(os, el);
  }
  os << "]";
}
template <typename Stream, typename T>
void dump_in_json(Stream& os, std::optional<T> const& t) {
  if (t.has_value()) {
    dump_in_json(os, t.value());
  } else {
    os << std::quoted("n/a");
  }
}
}  // namespace internal

// Function for user access
template <
    typename Stream, typename Y,
    class Bd =
        boost::describe::describe_bases<Y, boost::describe::mod_any_access>,
    class Md =
        boost::describe::describe_members<Y, boost::describe::mod_any_access>,
    class En = std::enable_if_t<!std::is_union<Y>::value> >
void dump_in_json(Stream& os, Y const& obj) {
  return internal::dump_in_json(os, obj, false);
}

}  // namespace persistence
