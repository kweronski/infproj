#pragma once
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace fw {
template <typename Id, typename T> struct register_t {
  std::unordered_map<Id, T> data{};

  void add(const Id &id, const T &d, bool overwrite = true) {
    if (!overwrite && data.contains(id)) {
      std::string message{"register_t::add: The overwrite parameter was set "};
      message += "to false, but the key ";
      if constexpr (std::is_same_v<Id, std::string>)
        message += "'" + id + "' ";
      message += "is not unique!";
      throw std::runtime_error{message};
    }
    data[id] = d;
  }

  void add(const Id &id, T &&d, bool overwrite = true) {
    if (!overwrite && data.contains(id)) {
      std::string message{"register_t::add: The overwrite parameter was set "};
      message += "to false, but the key ";
      if constexpr (std::is_same_v<Id, std::string>)
        message += "'" + id + "' ";
      message += "is not unique!";
      throw std::runtime_error{message};
    }
    data[id] = std::move(d);
  }

  const T *get_pointer(const Id &id) const {
    return data.contains(id) ? &data.at(id) : nullptr;
  }

  T *get_pointer(const Id &id) {
    return data.contains(id) ? &data.at(id) : nullptr;
  }

  template <typename Y = T> Y get_value(const Id &id) const {
    try {
      return static_cast<Y>(data.at(id));
    } catch (...) {
      throw std::runtime_error{
          "Failed to retrieve data from register under the following id: '" +
          id + "'"};
    }
  }
};

struct register_pool_t {
  register_t<std::string, std::string> string{};
  register_t<std::string, double> number{};
};
} // namespace fw
