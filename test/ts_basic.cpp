#include <gtest/gtest.h>

#include <numbers>

#include "Json_Serializator.h"

struct A {
  int x;
  double y;

  BOOST_DESCRIBE_CLASS(A, (), (), (x, y), ());
};

TEST(Basic, A) {
  std::stringstream ss;

  A a{42, 1.234567890123};

  persistence::dump_in_json(ss, a);

  EXPECT_EQ(ss.str(), R"""({"x" : "42", "y" : "1.235"})""");
}

class B {
  std::vector<int> vec_in_b{42};
  bool is_x = true;

 protected:
  int val;

 public:
  B() : val(21) {};

  BOOST_DESCRIBE_CLASS(B, (), (), (val), (is_x, vec_in_b));
};

TEST(Basic, private_protected_member) {
  std::stringstream ss;
  B b;

  persistence::dump_in_json(ss, b);

  EXPECT_EQ(ss.str(),
            R"""({"val" : "21", "is_x" : "true", "vec_in_b" :  [ "42"]})""");
}

struct C : public A, B {
  std::optional<std::string> s;

  BOOST_DESCRIBE_CLASS(C, (A, B), (), (), (s));
};

TEST(Basic, inherit) {
  std::stringstream ss;

  C c{};
  persistence::dump_in_json(ss, c);

  EXPECT_EQ(
      ss.str(),
      R"""({"x" : "0", "y" : "0", "val" : "21", "is_x" : "true", "vec_in_b" :  [ "42"], "s" : "n/a"})""");
}

class ID {
  int id;

 public:
  ID() = default;
  ID(int id) : id(id) {}

  BOOST_DESCRIBE_CLASS(ID, (), (), (), (id));
};

struct D {
  std::vector<ID> vector_of_a{1, 2, 3, 4};
  BOOST_DESCRIBE_CLASS(D, (), (vector_of_a), (), ());
};

TEST(Basic, composite_object) {
  std::stringstream ss;

  D d;
  persistence::dump_in_json(ss, d);

  EXPECT_EQ(
      ss.str(),
      R"""({"vector_of_a" :  [ {"id" : "1"}, {"id" : "2"}, {"id" : "3"}, {"id" : "4"}]})""");
}

enum class Color { black = 0, blue = 42, white = 255 };

BOOST_DESCRIBE_ENUM(Color, black, blue, white);

TEST(Basic, enum_type) {
  std::stringstream ss;

  Color color;
  color = Color::blue;
  persistence::internal::dump_in_json(ss, color);

  EXPECT_EQ(ss.str(), R"""("blue")""");
}

class Palette {
 public:
  std::vector<Color> color;

  BOOST_DESCRIBE_CLASS(Palette, (), (color), (), ());
};

TEST(Basic, vector_of_enum) {
  std::stringstream ss;

  Palette palette{.color = {Color::white, Color::blue, Color::black}};

  persistence::dump_in_json(ss, palette);

  EXPECT_EQ(ss.str(), R"""({"color" :  [ "white", "blue", "black"]})""");
}
