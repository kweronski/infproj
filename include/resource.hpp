#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>

namespace fw {
template <typename T> class resource_t {
public:
  bool load(const std::string &path, const std::string &id) {
    T r{};
    if (!r.loadFromFile(path))
      return false;
    container_.insert({id, std::move(r)});
    return true;
  }

  T &get(const std::string &id) { return container_.at(id); }

  void unload(const std::string &id) { container_.erase(id); }

private:
  std::unordered_map<std::string, T> container_;
};
} // namespace fw
