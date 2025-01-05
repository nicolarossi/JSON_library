#include <gtest/gtest.h>

#include <boost/describe.hpp>
#include <ios>
#include <memory>
#include <numbers>
#include <sstream>
#include <string>

#include "Json_Writer.h"
#include "Serializer.h"

// template <Is_Serializer Serializer>
// void serialize(Serializer& ser, std::shared_ptr<Node> const& ptr) {

struct Node {
  size_t id;

  std::vector<std::shared_ptr<Node> > child;

  BOOST_DESCRIBE_CLASS(Node, (), (id, child), (), ());
};

namespace persistence {
template <typename Writer>
struct Overloaded_Serializer : public persistence::Serializer<Writer> {
  using persistence::Serializer<Writer>::serialize;
  Overloaded_Serializer(Writer w) : persistence::Serializer<Writer>(w) {};

  virtual void serialize(std::shared_ptr<Node> const& var) {
    this->start_composite();
    if (var) {
      this->serialize("is null", false);
      this->serialize("id child", var->id);
    } else {
      this->serialize("is null", true);
    }
    this->end_composite();
  };
};
};  // namespace persistence

TEST(Overload, shared_ptr) {
  Node a(0), b(1), c(2), d(3);

  std::stringstream ss;
  persistence::Json_Writer jw{ss};
  persistence::Overloaded_Serializer<persistence::Json_Writer> osj{jw};

  a.child.push_back(std::make_shared<Node>(b));
  a.child.push_back(std::make_shared<Node>(c));
  a.child.push_back(std::make_shared<Node>(d));

  osj.serialize(a);

  std::string json_str = ss.str();

  EXPECT_EQ(
      json_str,
      R"""({"id":0:,"child":[{"id child":1},{"id child":2},{"id child":3}]})""");
}
