#pragma once

#include "node.hpp"
#include <functional>

namespace fw {
template <typename S> class button_t : public node_t {
public:
  using cb_t = std::function<void(button_t *)>;

public:
  void draw_current(sf::RenderTarget &t, sf::RenderStates s) const override {
    t.draw(shape_, s);
    t.draw(label_, s);
  }

  void update_current(update_data_t *data) override {
    if (!data)
      return;
    const auto ipos = sf::Mouse::getPosition(*data->window);
    sf::Vector2f pos{(float)ipos.x, (float)ipos.y};
    if (shape_.getGlobalBounds().contains(pos))
      hovered_ = true;
    else
      hovered_ = false;
    if (hovered_ && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
      clicked_ = true;
    else
      clicked_ = false;

    if (hovered_ && !hover_tracker_) {
      hover_tracker_ = true;
      this->on_hover();
    }
    if (!hovered_ && hover_tracker_) {
      hover_tracker_ = false;
      this->on_unhover();
    }

    if (clicked_ && !click_tracker_) {
      click_tracker_ = true;
      this->on_click();
    }
    if (!clicked_ && click_tracker_) {
      click_tracker_ = false;
      this->on_unclick();
    }
  }

  void on_click(cb_t f) { click_cb_ = f; }
  void on_hover(cb_t f) { hover_cb_ = f; }

  void on_unclick(cb_t f) { unclick_cb_ = f; }
  void on_unhover(cb_t f) { unhover_cb_ = f; }

  void on_click() {
    if (click_cb_)
      click_cb_(this);
  }
  void on_hover() {
    if (hover_cb_)
      hover_cb_(this);
  }

  void on_unclick() {
    if (unclick_cb_)
      unclick_cb_(this);
  }
  void on_unhover() {
    if (unhover_cb_)
      unhover_cb_(this);
  }

  bool hovered() const { return hovered_; }
  bool clicked() const { return clicked_; }

  sf::Text *label() { return &label_; }
  S *shape() { return &shape_; }

  void move(sf::Vector2f d) {
    shape_.move(d);
    label_.move(d);
  }

  void center() {
    auto sbb = shape_.getGlobalBounds();
    auto lbb = label_.getLocalBounds();
    if (sbb.width <= lbb.width || sbb.height <= lbb.height)
      return;
    label_.setOrigin(lbb.left + lbb.width / 2, lbb.top + lbb.height / 2);
    label_.setPosition(
        sf::Vector2f{sbb.left + sbb.width / 2, sbb.top + sbb.height / 2});
  }

private:
  bool hover_tracker_{false};
  bool click_tracker_{false};

  bool hovered_{false};
  bool clicked_{false};

  cb_t unhover_cb_{};
  cb_t unclick_cb_{};

  cb_t hover_cb_{};
  cb_t click_cb_{};

  sf::Text label_{};
  S shape_{};
};
} // namespace fw
