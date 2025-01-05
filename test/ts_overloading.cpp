#include <gtest/gtest.h>

#include <boost/describe.hpp>
#include <ios>
#include <memory>
#include <numbers>
#include <sstream>
#include <string>

#include "Json_Writer.h"
#include "Serializer.h"

struct Node {
  size_t id;

  std::vector<std::shared_ptr<Node> > child;

  BOOST_DESCRIBE_CLASS(Node, (), (id, child), (), ());
};

namespace persistence {

template <typename Serializer>
void serialize(Serializer& ser, std::shared_ptr<Node> const& var) {
  ser.start_composite();
  if (var) {
    serialize(ser, "is null", false);
    serialize(ser, "id child", var->id);
  } else {
    serialize(ser, "is null", true);
  }
  ser.end_composite();
};

}  // namespace persistence

TEST(Overload, shared_ptr) {
  Node a(0), b(1), c(2), d(3);

  std::stringstream ss;
  persistence::Json_Writer jw{ss};
  persistence::Serializer<persistence::Json_Writer> osj{jw};

  a.child.push_back(std::make_shared<Node>(b));
  a.child.push_back(std::make_shared<Node>(c));
  a.child.push_back(std::make_shared<Node>(d));

  persistence::serialize(osj, a);

  std::string json_str = ss.str();

  EXPECT_EQ(
      json_str,
      R"""({"id":0,"child":[{"is null":false,"id child":1},{"is null":false,"id child":2},{"is null":false,"id child":3}]})""");
}
