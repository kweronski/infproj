#pragma once

#include "node.hpp"
#include <functional>

namespace fw {
template <typename S> class button_t : public node_t {
public:
  using cb_t = std::function<void(button_t *)>;
  using cb_container_t = std::list<std::pair<std::size_t, cb_t>>;

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

private:
  bool remove_cb(unsigned id, std::string type) {
    cb_container_t *con{};
    if (type == "click")
      con = &click_cb_;
    else if (type == "unclick")
      con = &unclick_cb_;
    else if (type == "hover")
      con = &hover_cb_;
    else if (type == "unhover")
      con = &unhover_cb_;
    else
      return false;

    auto it = con->begin();
    for (; it != con->end(); ++it)
      if (it->first == id)
        break;
    if (it != con->end()) {
      con->erase(it);
      return true;
    }
    return false;
  }

public:
  unsigned add_click_cb(cb_t f) {
    click_cb_.push_back({++cb_id_, f});
    return cb_id_;
  }

  unsigned add_hover_cb(cb_t f) {
    hover_cb_.push_back({++cb_id_, f});
    return cb_id_;
  }

  unsigned add_unclick_cb(cb_t f) {
    unclick_cb_.push_back({++cb_id_, f});
    return cb_id_;
  }

  unsigned add_unhover_cb(cb_t f) {
    unhover_cb_.push_back({++cb_id_, f});
    return cb_id_;
  }

  bool remove_click_cb(unsigned id) { return remove_cb(id, "click"); }
  bool remove_hover_cb(unsigned id) { return remove_cb(id, "hover"); }
  bool remove_unclick_cb(unsigned id) { return remove_cb(id, "unclick"); }
  bool remove_unhover_cb(unsigned id) { return remove_cb(id, "unhover"); }

  void clear_cb() {
    click_cb_.clear();
    hover_cb_.clear();
    unclick_cb_.clear();
    unhover_cb_.clear();
    cb_id_ = 0;
  }

  void on_click() {
    for (const auto &f : click_cb_)
      if (f.second)
        f.second(this);
  }
  void on_hover() {
    for (const auto &f : hover_cb_)
      if (f.second)
        f.second(this);
  }

  void on_unclick() {
    for (const auto &f : unclick_cb_)
      if (f.second)
        f.second(this);
  }
  void on_unhover() {
    for (const auto &f : unhover_cb_)
      if (f.second)
        f.second(this);
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

  cb_container_t unhover_cb_{};
  cb_container_t unclick_cb_{};

  cb_container_t hover_cb_{};
  cb_container_t click_cb_{};

  sf::Text label_{};
  S shape_{};

  std::size_t cb_id_{};
};
} // namespace fw
