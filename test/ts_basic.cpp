#include <gtest/gtest.h>

#include <boost/describe.hpp>
#include <ios>
#include <numbers>
#include <sstream>
#include <string>

#include "Json_Writer.h"
#include "Serializer.h"

namespace basic_testing {

class Basic : public testing::Test {
 protected:
  std::stringstream ss;
  persistence::Json_Writer json_wr{ss};
  persistence::Serializer<persistence::Json_Writer> sj{json_wr};

  void SetUp() override { ss = std::stringstream(); }

  template <typename T>
  std::string get_json_serialize_of(T&& obj) {
    sj.serialize(obj);
    return ss.str();
  }
};

class A {
  int x{42};
  double y{3.14};

 public:
  template <Is_Serializer Serializer>
  void serialize(Serializer& s) const {
    s.serialize("x", x);
    s.serialize("y", y);
  }
};

TEST_F(Basic, A) {
  A a;

  std::string json_str = get_json_serialize_of(a);

  EXPECT_EQ(json_str, R"""({"x":42,"y":3.14})""");
}

class B {
  std::vector<int> vec_in_b{42};
  bool is_x = true;

 protected:
  int val;

 public:
  B() : val(21) {};

  template <Is_Serializer Serializer>
  void serialize(Serializer& s) const {
    s.serialize("val", val);
    s.serialize("is_x", is_x);
    s.serialize("vec_in_b", vec_in_b);
  }
};
TEST_F(Basic, private_protected_member) {
  B b;

  std::string json_str = get_json_serialize_of(b);

  EXPECT_EQ(json_str, R"""({"val":21,"is_x":true,"vec_in_b":[42]})""");
}

struct C : public A, B {
  std::optional<std::string> s;

  template <Is_Serializer Serializer>
  void serialize(Serializer& ser) const {
    ser.serialize("Base A", *dynamic_cast<const A*>(this));
    ser.serialize("Base B", *dynamic_cast<const B*>(this));
    ser.serialize("s", s);
  }
};
TEST_F(Basic, inherit) {
  std::stringstream ss;

  C c{};
  std::string json_str = get_json_serialize_of(c);

  EXPECT_EQ(
      json_str,
      R"""({"Base A":{"x":42,"y":3.14},"Base B":{"val":21,"is_x":true,"vec_in_b":[42]},"s":{"has value":false}})""");
}

TEST_F(Basic, inherit_s) {
  std::stringstream ss;

  C c{};
  c.s = "Hello";
  std::string json_str = get_json_serialize_of(c);

  EXPECT_EQ(
      json_str,
      R"""({"Base A":{"x":42,"y":3.14},"Base B":{"val":21,"is_x":true,"vec_in_b":[42]},"s":{"has value":true,"value":"Hello"}})""");
}

class ID {
  int id;

 public:
  ID() = default;
  ID(int id) : id(id) {}

  template <Is_Serializer Serializer>
  void serialize(Serializer& ser) const {
    ser.serialize("id", id);
  }
};

struct D {
  std::vector<ID> vector_of_a{1, 2, 3, 4};

  template <Is_Serializer Serializer>
  void serialize(Serializer& ser) const {
    ser.serialize("vector_of_a", vector_of_a);
  }
};

TEST_F(Basic, composite_object) {
  D d;

  std::string json_str = get_json_serialize_of(d);

  EXPECT_EQ(json_str,
            R"""({"vector_of_a":[{"id":1},{"id":2},{"id":3},{"id":4}]})""");
}

enum class Color { black = 0, blue = 42, white = 255 };

class Palette {
 public:
  std::vector<Color> color;

  template <Is_Serializer Serializer>
  void serialize(Serializer& ser) const {
    ser.serialize("color", color);
  }
};

TEST_F(Basic, vector_of_enum) {
  Palette palette{.color = {Color::white, Color::blue, Color::black}};

  std::string json_str = get_json_serialize_of(palette);

  EXPECT_EQ(json_str, R"""({"color":[255,42,0]})""");
}
};  // namespace basic_testing
