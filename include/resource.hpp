#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>

namespace fw {
template <typename T> class resource_t {
public:
  void load(const std::string &id, const std::string &path) {
    if (container_.contains(id))
      throw std::runtime_error{"A resource with the specified id: '" + id +
                               "' already exists"};

    T r{};
    if (!r.loadFromFile(path))
      throw std::runtime_error{"Loading a resource with id: '" + id +
                               "' at: '" + path + "' failed."};

    container_.insert({id, std::move(r)});
  }

  T &get(const std::string &id) {
    if (!container_.contains(id))
      throw std::runtime_error{"The requested resource does not exist: " + id};
    return container_.at(id);
  }
  const T &get(const std::string &id) const {
    if (!container_.contains(id))
      throw std::runtime_error{"The requested resource does not exist: " + id};
    return container_.at(id);
  }

  bool unload(const std::string &id) {
    if (!container_.contains(id))
      return false;
    container_.erase(id);
    return true;
  }

private:
  std::unordered_map<std::string, T> container_;
};
} // namespace fw
