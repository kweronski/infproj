#include "decrypt.hpp"
#include <cmath>

namespace {
void load_resources(fw::scene_t *s) {
  s->font.load("AnonymousPro", "./font/AnonymousPro-Regular.ttf");
  s->font.load("Digital7", "./font/digital-7.ttf");
  s->font.load("SophieMono", "./font/SophieMonoSerif.ttf");
}

struct key_data_t {
  float box_w, box_h, box_b /* width, height, and border sizes */;
  float tex_s /* text size */, off_x, off_y /* offsets */;
  sf::Color bg, fill, text;
  const std::string *seq;
  std::string id_prefix;
  std::string font_id;
  bool live, visible;
};

void create_panel(fw::scene_t *s, key_data_t *d) {
  auto create_letter = [s, d](float x, float y, char k) {
    auto ptr = new fw::button_t<sf::RectangleShape>{};
    s->root->attach(std::unique_ptr<fw::node_t>{ptr});
    s->vip_nodes[d->id_prefix + std::string{k}] = ptr;

    ptr->shape()->setOutlineColor(d->bg);
    ptr->shape()->setFillColor(d->fill);
    ptr->shape()->setSize({d->box_w, d->box_h});
    ptr->shape()->setOutlineThickness(d->box_b);
    ptr->shape()->setPosition({x, y});
    ptr->label()->setFont(s->font.get(d->font_id));
    ptr->label()->setFillColor(d->text);
    ptr->label()->setCharacterSize(d->tex_s);
    ptr->label()->setString(std::string{k});
    ptr->center();
    if (!d->visible) {
      ptr->deactivate();
      ptr->hide();
    }

    if (d->live) {
      ptr->add_hover_cb(
          [c = d->fill](auto *b) { b->shape()->setOutlineColor(c); });

      ptr->add_unhover_cb(
          [c = d->bg](auto *b) { b->shape()->setOutlineColor(c); });

      ptr->add_click_cb(
          [c = d->bg](auto *b) { b->shape()->setOutlineColor(c); });
    }
  };

  for (std::size_t i = 0; i < d->seq->size(); ++i) {
    const auto xpos = d->box_b + i * (3 * d->box_b + d->box_w);
    create_letter(xpos, d->off_y, (*d->seq)[i]);
  }
}
} // namespace

namespace cf {
void initialize_decrypt(fw::context_t *ctx) {
  const std::string mgn{"decrypt_mini_game"};
  if (ctx->scene_map.contains(mgn))
    ctx->scene_map.erase(mgn);

  auto s = fw::create_scene_skeleton(ctx);
  s.root = std::unique_ptr<fw::node_t>{new decrypt_t{}};
  load_resources(&s);

  initialize_round(&s, ctx);
  ctx->scene_map.emplace(mgn, std::move(s));
}
} // namespace cf

namespace {
void initialize_panels(fw::scene_t *s, float barrier) {
  auto p = dynamic_cast<cf::decrypt_t *>(s->root.get());
  std::shuffle(p->key.begin(), p->key.end(), p->rng);

  const std::string alph{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
  key_data_t panel{.box_w = 50,
                   .box_h = 50,
                   .box_b = 2,
                   .tex_s = 40,
                   .off_x = 0,
                   .off_y = barrier,
                   .bg = sf::Color::Black,
                   .fill = sf::Color{255, 165, 0},
                   .text = sf::Color::Black,
                   .seq = &alph,
                   .id_prefix = "letter",
                   .font_id = "SophieMono",
                   .live = true,
                   .visible = true};
  create_panel(s, &panel);

  panel.off_y = panel.off_y + 60;
  panel.seq = &p->key;
  panel.id_prefix = "key";
  panel.live = false;
  panel.fill = sf::Color::Red;
  panel.visible = false;
  create_panel(s, &panel);

  p->plaintext = cf::pick_word();
  p->encrypted = cf::encrypt(p->key, p->plaintext);

  panel.off_y += 60;
  panel.visible = true;
  panel.seq = &p->encrypted;
  panel.id_prefix = "enc";
  create_panel(s, &panel);

  panel.off_y += 60;
  panel.seq = &p->plaintext;
  panel.fill = sf::Color{255, 165, 0}, panel.id_prefix = "plain";
  panel.visible = false;
  create_panel(s, &panel);

  for (const auto &l : alph) {
    const auto kid = std::string{"key"} + p->key[alph.find(l)];
    const auto id = std::string{"letter"} + l;
    fw::add_routine(s, [id, kid](auto *scene) {
      auto k = dynamic_cast<fw::button_t<sf::RectangleShape> *>(
          scene->vip_nodes.at(kid));
      auto l = dynamic_cast<fw::button_t<sf::RectangleShape> *>(
          scene->vip_nodes.at(id));
      if (!k->visible())
        return;
      if (!l->clicked())
        return;
      dynamic_cast<fw::basic_node_t<sf::Text> *>(scene->vip_nodes.at("hp"))
          ->shape()
          ->setString("HP: CLICKED!");
    });
  }
}

void initialize_countdown(fw::scene_t *s, float barrier) {
  auto ww = fw::get_value_from_register<float>(s, "window_width");
  auto p = dynamic_cast<cf::decrypt_t *>(s->root.get());
  p->time_left = 60;

  auto tl = new fw::basic_node_t<sf::Text>{};
  s->root->attach(std::unique_ptr<fw::node_t>{tl});

  tl->shape()->setFont(s->font.get("Digital7"));
  tl->shape()->setFillColor(sf::Color::White);
  tl->shape()->setCharacterSize(40);
  tl->shape()->setString("TL: " + std::to_string(p->time_left));
  tl->shape()->setPosition(sf::Vector2f{ww - 200, barrier});
  s->vip_nodes["tl"] = tl;

  fw::add_routine(s, [](auto *ptr) {
    auto dc = dynamic_cast<cf::decrypt_t *>(ptr->root.get());
    if (dc->tld_tracker == std::chrono::time_point<std::chrono::steady_clock>{})
      dc->tld_tracker = std::chrono::steady_clock::now();

    auto tl =
        dynamic_cast<fw::basic_node_t<sf::Text> *>(ptr->vip_nodes.at("tl"));

    if (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - dc->tld_tracker)
            .count() < 1000)
      return;

    dc->tld_tracker = std::chrono::steady_clock::now();
    if (dc->time_left > 0)
      --dc->time_left;
    tl->shape()->setString("TL: " + std::to_string(dc->time_left));
  });
}

void initialize_hp(fw::scene_t *s, float barrier) {
  auto ww = fw::get_value_from_register<float>(s, "window_width");
  auto p = dynamic_cast<cf::decrypt_t *>(s->root.get());
  p->health = 4;

  auto hp = new fw::basic_node_t<sf::Text>{};
  s->root->attach(std::unique_ptr<fw::node_t>{hp});

  hp->shape()->setFont(s->font.get("Digital7"));
  hp->shape()->setFillColor(sf::Color::White);
  hp->shape()->setCharacterSize(40);
  hp->shape()->setString("HP: " + std::to_string(p->health));
  hp->shape()->setPosition(sf::Vector2f{ww - 200, barrier + 60});
  s->vip_nodes["hp"] = hp;
}

void initialize_indicators(fw::scene_t *s, float barrier) {
  initialize_countdown(s, barrier);
  initialize_hp(s, barrier);
}
} // namespace

void initialize_buttons(fw::scene_t *s, fw::context_t *ctx) {
  auto wh = fw::get_value_from_register<float>(s, "window_height");
  auto ww = fw::get_value_from_register<float>(s, "window_width");

  auto b = new fw::button_t<sf::RectangleShape>{};
  s->root->attach(std::unique_ptr<fw::node_t>{b});

  constexpr float w = 200, h = 100, p = 2;
  b->shape()->setPosition({ww - w - 2 * p, wh - h - 2 * p});
  b->shape()->setFillColor(sf::Color::Black);
  b->shape()->setSize({w, h});
  b->shape()->setOutlineThickness(p);
  b->shape()->setOutlineColor(sf::Color::White);

  b->label()->setFillColor(sf::Color::White);
  b->label()->setFont(s->font.get("AnonymousPro"));
  b->label()->setCharacterSize(40);
  b->label()->setString("Back");
  b->center();

  b->add_hover_cb(
      [](auto *ptr) { ptr->shape()->setOutlineColor(sf::Color::Red); });

  b->add_unhover_cb(
      [](auto *ptr) { ptr->shape()->setOutlineColor(sf::Color::White); });

  b->add_click_cb([s, ctx](auto *) {
    fw::add_command(&ctx->scene_map.at("selection_menu"), [ctx](auto *) {
      cf::initialize_round(&ctx->scene_map.at("decrypt_mini_game"), ctx);
    });
    fw::activate_scene(ctx, "selection_menu");
  });
}

namespace cf {
void initialize_round(fw::scene_t *s, fw::context_t *ctx) {
  s->root = std::unique_ptr<fw::node_t>{new decrypt_t{}};
  fw::clear_routines(s);
  constexpr float barrier{840};

  initialize_indicators(s, barrier);
  initialize_panels(s, barrier);
  initialize_buttons(s, ctx);
}

std::string pick_word() {
  static std::mt19937 rng{std::random_device{}()};
  static const std::vector<std::string> db = {
      "Ability",       "Access",         "Accident",      "Account",
      "Action",        "Activity",       "Actor",         "Addition",
      "Address",       "Administration", "Advantage",     "Advertising",
      "Advice",        "Affair",         "Agency",        "Agreement",
      "Airport",       "Alcohol",        "Ambition",      "Amount",
      "Analysis",      "Analyst",        "Animal",        "Answer",
      "Anxiety",       "Apartment",      "Appearance",    "Apple",
      "Application",   "Appointment",    "Argument",      "Arrival",
      "Article",       "Aspect",         "Assignment",    "Assistance",
      "Assistant",     "Association",    "Assumption",    "Atmosphere",
      "Attempt",       "Attention",      "Attitude",      "Audience",
      "Average",       "Awareness",      "Balance",       "Baseball",
      "Basis",         "Basket",         "Bathroom",      "Bedroom",
      "Beginning",     "Benefit",        "Board",         "Bonus",
      "Bottom",        "Bread",          "Breath",        "Building",
      "Business",      "Cabinet",        "Camera",        "Candidate",
      "Capital",       "Career",         "Category",      "Cause",
      "Celebration",   "Championship",   "Chance",        "Chapter",
      "Charity",       "Chemistry",      "Chest",         "Chicken",
      "Child",         "Chocolate",      "Choice",        "Church",
      "Cigarette",     "Class",          "Classroom",     "Client",
      "Climate",       "Clothes",        "Coast",         "Coffee",
      "Collection",    "College",        "Combination",   "Committee",
      "Communication", "Community",      "Company",       "Comparison",
      "Competition",   "Complaint",      "Computer",      "Concept",
      "Conclusion",    "Condition",      "Connection",    "Consequence",
      "Construction",  "Contact",        "Context",       "Contract",
      "Contribution",  "Control",        "Conversation",  "Cookie",
      "Country",       "County",         "Courage",       "Course",
      "Cousin",        "Craft",          "Credit",        "Criticism",
      "Culture",       "Currency",       "Customer",      "Cycle",
      "Database",      "Decision",       "Definition",    "Delivery",
      "Demand",        "Department",     "Departure",     "Depression",
      "Depth",         "Description",    "Design",        "Development",
      "Device",        "Diamond",        "Difference",    "Dinner",
      "Direction",     "Director",       "Disaster",      "Discipline",
      "Discussion",    "Disease",        "Distribution",  "Drama",
      "Drawer",        "Drawing",        "Driver",        "Earth",
      "Economics",     "Economy",        "Editor",        "Education",
      "Effect",        "Efficiency",     "Effort",        "Election",
      "Elevator",      "Emotion",        "Emphasis",      "Employee",
      "Employer",      "Employment",     "Energy",        "Engine",
      "Entertainment", "Enthusiasm",     "Entry",         "Environment",
      "Equipment",     "Error",          "Establishment", "Estate",
      "Event",         "Examination",    "Example",       "Exchange",
      "Excitement",    "Exercise",       "Experience",    "Explanation",
      "Expression",    "Extent",         "Family",        "Farmer",
      "Feature",       "Feedback",       "Field",         "Figure",
      "Finding",       "Flight",         "Focus",         "Football",
      "Force",         "Fortune",        "Foundation",    "Frame",
      "Freedom",       "Friendship",     "Future",        "Garden",
      "Government",    "Grandmother",    "Grocery",       "Group",
      "Growth",        "Guest",          "Guidance",      "Guide",
      "Guitar",        "Health",         "Hearing",       "Heart",
      "Height",        "Highway",        "Historian",     "History",
      "Homework",      "Honey",          "Hospital",      "Hotel",
      "House",         "Housing",        "Image",         "Imagination",
      "Impact",        "Importance",     "Impression",    "Improvement",
      "Income",        "Independence",   "Indication",    "Industry",
      "Inflation",     "Information",    "Initiative",    "Injury",
      "Insect",        "Inside",         "Inspection",    "Inspector",
      "Instance",      "Instruction",    "Insurance",     "Intention",
      "Interaction",   "Interest",       "Internet",      "Introduction",
      "Investment",    "Issue",          "Language",      "Leader",
      "Leadership",    "Length",         "Level",         "Library",
      "Light",         "Literature",     "Location",      "Machine",
      "Magazine",      "Maintenance",    "Management",    "Manager",
      "Manufacturer",  "Market",         "Marketing",     "Marriage",
      "Material",      "Matter",         "Meaning",       "Measurement",
      "Media",         "Medicine",       "Medium",        "Member",
      "Membership",    "Memory",         "Message",       "Metal",
      "Method",        "Midnight",       "Mixture",       "Model",
      "Moment",        "Money",          "Month",         "Morning",
      "Mouse",         "Movie",          "Music",         "Judgment",
      "Knowledge",     "Nation",         "Nature",        "Negotiation",
      "Network",       "Newspaper",      "Night",         "Nothing",
      "Number",        "Object",         "Obligation",    "Office",
      "Operation",     "Opinion",        "Opportunity",   "Orange",
      "Order",         "Organization",   "Outcome",       "Outside",
      "Owner",         "Paint",          "Painting",      "Paper",
      "Passenger",     "Passion",        "Patience",      "Payment",
      "Penalty",       "People",         "Percentage",    "Perception",
      "Performance",   "Period",         "Permission",    "Person",
      "Personality",   "Perspective",    "Philosophy",    "Phone",
      "Photo",         "Physics",        "Piano",         "Picture",
      "Piece",         "Pizza",          "Place",         "Platform",
      "Player",        "Poetry",         "Point",         "Police",
      "Policy",        "Politics",       "Pollution",     "Population",
      "Position",      "Possession",     "Possibility",   "Potato",
      "Power",         "Practice",       "Preference",    "Preparation",
      "Presence",      "Presentation",   "President",     "Pressure",
      "Price",         "Priority",       "Problem",       "Procedure",
      "Process",       "Product",        "Profession",    "Professor",
      "Profit",        "Program",        "Promotion",     "Property",
      "Proposal",      "Protection",     "Psychology",    "Purpose",
      "Quality",       "Quantity",       "Queen",         "Question",
      "Radio",         "Range",          "Ratio",         "Reaction",
      "Reality",       "Reason",         "Reception",     "Recipe",
      "Recognition",   "Recommendation", "Record",        "Recording",
      "Reflection",    "Refrigerator",   "Region",        "Relation",
      "Relationship",  "Replacement",    "Republic",      "Reputation",
      "Requirement",   "Research",       "Resolution",    "Resource",
      "Response",      "Responsibility", "Restaurant",    "Result",
      "Revenue",       "Review",         "Revolution",    "Safety",
      "Salad",         "Sample",         "Satisfaction",  "Scale",
      "Scene",         "School",         "Science",       "Screen",
      "Secretary",     "Section",        "Sector",        "Security",
      "Selection",     "Sense",          "Series",        "Service",
      "Session",       "Setting",        "Shape",         "Share",
      "Shirt",         "Signature",      "Significance",  "Singer",
      "Situation",     "Skill",          "Society",       "Software",
      "Solution",      "Sound",          "Source",        "Space",
      "Speaker",       "Speech",         "Sport",         "Square",
      "Standard",      "State",          "Statement",     "Steak",
      "Stock",         "Storage",        "Store",         "Story",
      "Stranger",      "Strategy",       "Stress",        "Structure",
      "Student",       "Studio",         "Study",         "Style",
      "Subject",       "Success",        "Suggestion",    "Supermarket",
      "Surgery",       "Sympathy",       "System",        "Table",
      "Teacher",       "Technology",     "Television",    "Temperature",
      "Tennis",        "Tension",        "Thanks",        "Theory",
      "Thing",         "Thought",        "Tooth",         "Topic",
      "Trade",         "Tradition",      "Trainer",       "Transportation",
      "Truth",         "Uncle",          "Understanding", "Union",
      "University",    "Value",          "Variation",     "Variety",
      "Vehicle",       "Version",        "Video",         "Village",
      "Voice",         "Volume",         "Water",         "Wealth",
      "Weather",       "While",          "Winner",        "Woman",
      "Worker",        "World",          "Writer",        "Writing"};
  std::uniform_int_distribution<std::size_t> d(0, db.size() - 1);
  std::string word = db[d(rng)];
  for (auto &l : word)
    l = std::toupper(l);
  return word;
}
} // namespace cf
