#pragma once

#include <SFML/Graphics.hpp>
#include <list>
#include <memory>
#include <optional>

namespace fw {
struct update_data_t {
  sf::RenderWindow *window;
  sf::Vector2f *last_mouse_pos;
};

class node_t : public sf::Drawable {
public:
  using ptr_t = std::unique_ptr<node_t>;

public:
  node_t() = default;
  node_t(const node_t &) = delete;
  node_t &operator=(const node_t &) = delete;
  node_t(node_t &&) = default;
  node_t &operator=(node_t &&) = default;
  virtual ~node_t() = default;

  void draw(sf::RenderTarget &, sf::RenderStates) const override final;
  virtual void draw_current(sf::RenderTarget &, sf::RenderStates) const;

  void update(update_data_t *);
  virtual void update_current(update_data_t *);
  void activate();
  void deactivate();
  bool active() const;

  void attach(ptr_t);
  ptr_t detach(node_t *);
  void clear();

  void show();
  void hide();
  bool visible() const;

  node_t *parent() const;
  void parent(node_t *);

  virtual void move(float, float);
  virtual std::optional<sf::Rect<float>> bounds() const { return {}; }

private:
  std::list<ptr_t> nodes_;
  bool active_{true};
  bool visible_{true};
  node_t *parent_{nullptr};
};

template <typename T> class basic_node_t : public node_t {
public:
  void draw_current(sf::RenderTarget &r, sf::RenderStates s) const override {
    r.draw(shape_, s);
  }

  void move(float x, float y) override { shape_.move(x, y); }

  std::optional<sf::Rect<float>> bounds() const override {
    return shape_.getGlobalBounds();
  }

  const T *shape() const { return &shape_; }
  T *shape() { return &shape_; }

private:
  T shape_;
};

} // namespace fw
