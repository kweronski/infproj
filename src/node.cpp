#include "node.hpp"

namespace fw {
void node_t::draw(sf::RenderTarget &t, sf::RenderStates s) const {
  if (!this->visible())
    return;
  this->draw_current(t, s);
  for (const auto &c : nodes_)
    c->draw(t, s);
}

void node_t::draw_current(sf::RenderTarget &, sf::RenderStates) const {}

void node_t::update(update_data_t *d) {
  if (!this->active())
    return;
  this->update_current(d);
  for (const auto &c : nodes_)
    c->update(d);
}

void node_t::update_current(update_data_t *) {}

void node_t::activate() { this->active_ = true; }

void node_t::deactivate() { this->active_ = false; }

bool node_t::active() const { return this->active_; }

void node_t::attach(ptr_t p) {
  p->parent(this);
  nodes_.push_back(std::move(p));
}

node_t::ptr_t node_t::detach(node_t *id) {
  auto it = nodes_.begin();
  ptr_t mv{};
  for (; it != nodes_.end(); ++it) {
    if (it->get() == id) {
      mv = std::move(*it);
      break;
    }
  }
  if (it != nodes_.end())
    nodes_.erase(it);
  mv->parent(nullptr);
  return mv;
}

void node_t::show() { this->visible_ = true; }

void node_t::hide() { this->visible_ = false; }

bool node_t::visible() const { return this->visible_; }

node_t *node_t::parent() const { return this->parent_; }

void node_t::parent(node_t *p) { this->parent_ = p; }
} // namespace fw
