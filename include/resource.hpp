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
  const T &get(const std::string &id) const { return container_.at(id); }

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
