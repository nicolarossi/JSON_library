
#pragma once

template <typename T>
concept Forward_Container = requires(T t) {
  ++t.begin();
  t.begin() != t.end();
};

template <typename T>
concept Is_Enum = std::is_enum_v<T>;

template <typename T>
concept To_Stringable = requires(T a) { to_string(a); } || std::integral<T>;
