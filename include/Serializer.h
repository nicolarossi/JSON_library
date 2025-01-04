#pragma once
#include <boost/core/demangle.hpp>
#include <boost/describe.hpp>
#include <memory>
#include <type_traits>

#include "concepts.h"

namespace persistence {

template <typename Serializer, Is_Serializable<Serializer> Serializable>
void serialize(Serializer& ser, Serializable const& obj) {
  obj.serialize(ser);
}

template <typename Serializer, typename T>
void serialize(Serializer& ser, T const& obj) {
  ser.serialize(obj);
}

template <Is_Writer Writer>
class Serializer {
  Writer& wr;

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

 public:
  template <typename T>
  void serialize(std::optional<T> const& var) {
    this->start_composite();
    if (var.has_value()) {
      this->serialize("has value", true);
      serialize("value", var.value());
    } else {
      this->serialize("has value", false);
    }
    this->end_composite();
  };

  template <typename T>
  void serialize(std::shared_ptr<T> const& var) {
    this->start_composite();
    if (var) {
      this->serialize("is null", false);
      serialize("value", *var);
    } else {
      this->serialize("is null", true);
    }
    this->end_composite();
  };

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

  template <Forward_Container T>
  void serialize(T const& container) {
    this->start_container();
    for (auto const& el : container) {
      this->new_element();
      persistence::serialize(*this, el);
      this->end_element();
    }
    this->end_container();
  };

 public:
  Serializer() = delete;

  Serializer(Writer& wr) : wr(wr) {};
  Serializer(const Serializer&) = delete;
  Serializer(Serializer&&) = delete;
  virtual ~Serializer() = default;

  template <Is_Serializable<Serializer> Serializable>
  void serialize(Serializable& obj) {
    this->start_composite();
    obj.serialize(*this);
    this->end_composite();
  };

  template <typename S, typename T>
    requires std::is_convertible_v<S, std::string_view>
  void serialize(S name, T const& obj) {
    this->new_element();
    wr.write_id(name, obj);

    serialize(obj);
    this->end_element();
  }

  template <
      typename Y,
      class Bd =
          boost::describe::describe_bases<Y, boost::describe::mod_any_access>,
      class Md =
          boost::describe::describe_members<Y, boost::describe::mod_any_access>,
      class En = std::enable_if_t<!std::is_union<Y>::value> >
  void serialize(Y const& obj, bool flat_object = false) {
    if (!flat_object) {
      this->start_composite();
    }

    boost::mp11::mp_for_each<Bd>([&](auto base) {
      using Base_Type = typename decltype(base)::type;
      Base_Type const& base_part = (Base_Type const&)(obj);

      std::string name_base_class =
          boost::core::demangle(typeid(base_part).name());

      this->serialize(name_base_class, base_part);
    });

    boost::mp11::mp_for_each<Md>([&](auto D) {
      this->new_element();
      wr.write_id(D.name, obj);
      this->serialize((obj).*D.pointer);
      this->end_element();
    });
    if (!flat_object) {
      this->end_composite();
    }
  }
};

}  // namespace persistence
