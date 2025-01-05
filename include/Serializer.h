#pragma once
#include <boost/core/demangle.hpp>
#include <boost/describe.hpp>
#include <memory>
#include <type_traits>

#include "concepts.h"

namespace persistence {

template <Is_Writer Writer>
class Serializer {
  Writer& wr;

 public:
  Serializer() = delete;

  Serializer(Writer& wr) : wr(wr) {};
  Serializer(const Serializer&) = delete;
  Serializer(Serializer&&) = delete;
  virtual ~Serializer() = default;

  /* A 'composite'  is a 'struct' */
  void start_composite() { wr.start_composite(); }
  void end_composite() { wr.end_composite(); }

  /* */
  void start_container() { wr.start_container(); }
  void end_container() { wr.end_container(); }

  /*
   an 'element' is either  :
  - a pair < name, object >
  - or an object in a container

  This is needed for write comma in JSON.

  */

  void new_element() { wr.new_element(); }
  void end_element() {}

  template <typename T, typename Obj>
    requires std::is_convertible_v<T, std::string_view>
  void write_id(T s, Obj obj) {
    wr.write_id(s, obj);
  }

  template <typename T>
    requires std::is_convertible_v<T, std::string_view>
  void serialize(T const& x) {
    wr.write(std::string_view(x));
  };

  template <Is_Primitive T>
  void serialize(T const& var) {
    wr.write(var);
  };

  template <typename E>
    requires std::is_enum_v<E>
  void serialize(E const& var) {
    wr.write(var);
  };
};

template <
    typename Serializer, typename Y,
    typename Bd =
        boost::describe::describe_bases<Y, boost::describe::mod_any_access>,
    typename Md =
        boost::describe::describe_members<Y, boost::describe::mod_any_access>,
    typename En = std::enable_if_t<!std::is_union<Y>::value> >
// void serialize(Serializer& ser, Y const& obj, bool flat_object = false) {
void serialize(Serializer& ser, Y const& obj) {
  // if (!flat_object) {
  ser.start_composite();
  //}

  boost::mp11::mp_for_each<Bd>([&](auto base) {
    using Base_Type = typename decltype(base)::type;
    Base_Type const& base_part = (Base_Type const&)(obj);

    std::string name_base_class =
        boost::core::demangle(typeid(base_part).name());

    serialize(ser, name_base_class, base_part);
  });

  boost::mp11::mp_for_each<Md>([&](auto D) {
    ser.new_element();
    ser.write_id(D.name, obj);
    serialize(ser, (obj).*D.pointer);
    ser.end_element();
  });
  //  if (!flat_object) {
  ser.end_composite();
  //  }
}

template <typename T, typename Serializer>
void serialize(Serializer& ser, std::optional<T> const& var) {
  ser.start_composite();
  if (var.has_value()) {
    serialize(ser, "has value", true);
    serialize(ser, "value", var.value());
  } else {
    serialize(ser, "has value", false);
  }
  ser.end_composite();
};

template <typename T, typename Serializer>
void serialize(Serializer& ser, std::shared_ptr<T> const& var) {
  ser.start_composite();
  if (var) {
    serialize(ser, "is null", false);
    serialize(ser, "value", *var);
  } else {
    serialize(ser, "is null", true);
  }
  ser.end_composite();
};

template <typename T, typename Serializer>
  requires std::is_convertible_v<T, std::string_view>
void serialize(Serializer& ser, T const& x) {
  ser.serialize(std::string_view(x));
};

template <Is_Primitive T, typename Serializer>
void serialize(Serializer& ser, T const& var) {
  ser.serialize(var);
};

template <typename E, typename Serializer>
  requires std::is_enum_v<E>
void serialize(Serializer& ser, E const& var) {
  ser.serialize(var);
};

template <Forward_Container T, typename Serializer>
void serialize(Serializer& ser, T const& container) {
  ser.start_container();
  for (auto const& el : container) {
    ser.new_element();

    serialize(ser, el);
    ser.end_element();
  }
  ser.end_container();
};

/* */
template <Is_Serializer Serializer, Is_Serializable<Serializer> Serializable>
// template <typename Serializer, typename Serializable>
void serialize(Serializer& ser, Serializable& obj) {
  ser.start_composite();
  obj.serialize(ser);
  ser.end_composite();
};

template <typename Serializer, typename S, typename T>
  requires std::is_convertible_v<S, std::string_view>
void serialize(Serializer& ser, S name, T const& obj) {
  ser.new_element();
  ser.write_id(name, obj);

  serialize(ser, obj);
  ser.end_element();
}

}  // namespace persistence
