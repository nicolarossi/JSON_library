#include <gtest/gtest.h>

#include <boost/describe.hpp>
#include <ios>
#include <memory>
#include <numbers>
#include <sstream>
#include <string>

#include "Json_Writer.h"
#include "Serializer.h"

namespace described_testing {

class Described : public testing::Test {
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
  BOOST_DESCRIBE_CLASS(A, (), (), (), (x, y));
};

TEST_F(Described, A) {
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

  BOOST_DESCRIBE_CLASS(B, (), (), (val), (is_x, vec_in_b));
};

TEST_F(Described, private_protected_member) {
  B b;

  std::string json_str = get_json_serialize_of(b);

  EXPECT_EQ(json_str, R"""({"val":21,"is_x":true,"vec_in_b":[42]})""");
}

struct C : public A, B {
  std::optional<std::string> s;

  BOOST_DESCRIBE_CLASS(C, (A, B), (), (), (s));
};

TEST_F(Described, inherit) {
  std::stringstream ss;

  C c{};
  std::string json_str = get_json_serialize_of(c);

  EXPECT_EQ(
      json_str,
      R"""({"described_testing::A":{"x":42,"y":3.14},"described_testing::B":{"val":21,"is_x":true,"vec_in_b":[42]},"s":{"has value":false}})""");
}

TEST_F(Described, inherit_s) {
  std::stringstream ss;

  C c{};
  c.s = "Hello";
  std::string json_str = get_json_serialize_of(c);

  EXPECT_EQ(
      json_str,
      R"""({"described_testing::A":{"x":42,"y":3.14},"described_testing::B":{"val":21,"is_x":true,"vec_in_b":[42]},"s":{"has value":true,"value":"Hello"}})""");
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

TEST_F(Described, composite_object) {
  D d;

  std::string json_str = get_json_serialize_of(d);

  EXPECT_EQ(json_str,
            R"""({"vector_of_a":[{"id":1},{"id":2},{"id":3},{"id":4}]})""");
}

enum class Color { black = 0, blue = 42, white = 255 };

BOOST_DESCRIBE_ENUM(Color, black, blue, white);

class Palette {
 public:
  std::vector<Color> color;

  BOOST_DESCRIBE_CLASS(Palette, (), (color), (), ());
};

TEST_F(Described, vector_of_enum) {
  Palette palette{.color = {Color::white, Color::blue, Color::black}};

  std::string json_str = get_json_serialize_of(palette);

  //  EXPECT_EQ(json_str, R"""({"color":[255,42,0]})""");

  EXPECT_EQ(json_str, R"""({"color":["white","blue","black"]})""");
}

struct Node {
  size_t id;

  std::vector<std::shared_ptr<Node> > child;

  BOOST_DESCRIBE_CLASS(Node, (), (id, child), (), ());
};

template <Is_Serializer Serializer>
void serialize(Serializer& ser, std::shared_ptr<Node> const& ptr) {
  ser.serialize("id child", ptr->id);
}

TEST_F(Described, with_overloading) {
  Node a(0), b(1), c(2), d(3);

  a.child.push_back(std::make_shared<Node>(b));
  a.child.push_back(std::make_shared<Node>(c));
  a.child.push_back(std::make_shared<Node>(d));

  std::string json_str = get_json_serialize_of(a);

  //  EXPECT_EQ(json_str, R"""({"color":[255,42,0]})""");

  EXPECT_EQ(json_str, R"""({"color":["white","blue","black"]})""");
}

}  // namespace described_testing
