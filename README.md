
It is a header-only library to save in a JSON format C++ classes, optionally described using [Boost.Describe](https://www.boost.org/doc/libs/master/libs/describe/doc/html/describe.html).

Note:
[Boost.Serialization](https://www.boost.org/doc/libs/1_87_0/libs/serialization/doc/index.html) does already this job and it should be preferred in real application.
The reason for this project is to test [Boost.Describe](https://www.boost.org/doc/libs/master/libs/describe/doc/html/describe.html) for serialization.
In the future this library would be simplified to use completely only Boost components.


# Responsabilities

## Developer's Class 
 It defines what should be serialized/deserialized, definining the method:
  ```
  template <Is_Serializer Serializer>
  void serialize(Serializer& ser) const 
  ```

## Serializator

It provides serialization for partial specialized type:
eg: serialization of : optional<T>, forward container<T>.

The production of the stream is not its responsability.

# Writer/Reader

It writes on the stream, every element contributing to the serialization.

It manages the specific rapresentation (binary/xml/json etc...)


# Use case

```

// The user can define the serialization directly ...
class A {
    int x;
    double y;

    public:
    template<Serializer S>
    void serialize(S const &s ) const{
        serialize(s, "x", x);
        serialize(s, "y", y);
    }

};

// ...or can be defined using Boost.Describe .
class Described_A{
    int x;
    double y;

    public:

    BOOST_DESCRIBE_CLASS(Described_A, (), (), (), (x, y));

};

int main(){
    A a;
    std::ostream os;
    Serializer<Json_Writer> sj(os);
    serialize(sj, a);

    std::istream is(os.str());
    Deserializer<Json_Writer> dsj(is);

    Described_A x;
    deserialize(dsj, x);

}

//
template<typename Writer>
class Serializer {
    Writer wr;
    public:
    void serialize(Serializable const& obj){
        wr.start();
        obj.serialize(*this);
        wr.end();
    };
 
};

namespace persistence {
 template <Is_Serializer Serializer, Is_Serializable<Serializer> Serializable>
 void serialize(Serializer& ser, Serializable& obj) {
   ser.start_composite();
   obj.serialize(ser);
   ser.end_composite();
 };
}
```

If the user want to overload the serialization for a specific type should define the template;

```
namespace persistence {
template <typename Serializer>
void serialize(
    Serializer& ser, 
    std::shared_ptr<Node> const& var) {
....
```

In the following case a `shared_ptr<Node>` it would be serialized using a different function, without navigating the complete `Node`.

```
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
```

It is needed, in a directional cyclic graph structure.

