#pragma once

#include <SFML/Graphics.hpp>
#include <list>
#include <memory>

namespace fw {
struct update_data_t {};

class node_t : public sf::Drawable {
public:
  using ptr_t = std::unique_ptr<node_t>;

public:
  node_t() = default;
  node_t(const node_t &) = delete;
  node_t &operator=(const node_t &) = delete;
  node_t(node_t &&) = default;
  node_t &operator=(node_t &&) = default;
  ~node_t() = default;

  void draw(sf::RenderTarget &, sf::RenderStates) const override final;
  virtual void drawCurrent(sf::RenderTarget &, sf::RenderStates) const;

  void update(update_data_t *);
  virtual void updateCurrent(update_data_t *);
  void activate();
  void deactivate();
  bool active() const;

  void attach(ptr_t);
  ptr_t detach(node_t *);

  void show();
  void hide();
  bool visible() const;

  node_t *parent() const;
  void parent(node_t *);

private:
  std::list<ptr_t> nodes_;
  bool active_{true};
  bool visible_{true};
  node_t *parent_{nullptr};
};
} // namespace fw
