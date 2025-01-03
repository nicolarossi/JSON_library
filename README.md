
It is a header-only library to save in a JSON format C++ classes.

It uses [Boost.Describe](https://www.boost.org/doc/libs/master/libs/describe/doc/html/describe.html).

# Responsabilities

## Class 
 It defines what should be serialized/deserialized

## Serializator

It creates satellite informations about the serialization.
eg: the CRC values, calculates the size of the packets

# Writer

It writes on the stream the information required.

eg: it manages the endianess



# Use case

class A {
    int x;
    double y;

    public:
    template<Serializer S>
    void serialize(S const &s ) const{
        s.serialize("x", x);
        s.serialize("y", y);
    }

    template<Deserializer DS>
    void deserialize(DS &ds){
        ...
    } 
};


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
    sj.save( a);

    std::istream is(os.str());
    Deserializer<Json_Writer> dsj(is);

    Described_A x;
    dsj.load(is, x);

}

template<typename Writer>
class Serializer {
    Writer wr;
    public:
    void save(Serializable const& obj){
        wr.start();
        obj.serialize(*this);
        wr.end();
    };

    void load(Deserializable & obj){
        ....
    };
};

template<typename Writer>
class CRC_Serializer {
    Binary_Writer b_wr;
    Writer wr;

    public:
    void save(Serializable const& obj){
        vector<byte> buffer;
        b_wr(buffer);
        b_wr.start();
        obj.serialize(b_wr);
        b_wr.end();

        wr.start();        
        obj.serialize(wr);

        auto crc= calculate_CRC(b_wr);
        wr.serialize("CRC", crc);
        wr.end();       
    };

};

