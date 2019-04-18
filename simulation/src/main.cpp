#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <SFML/Graphics.hpp>

enum class bact_type {
  normal,
  immune,
  toxic
};

// This should be bacterium of course...
class bacteria {
  public:
    bacteria(bact_type t) {
      type = t;
    }
    
    bact_type type;
    
    bacteria get_child() {
      bacteria b(type);
      return b;
    }
    
};

bool attack(bacteria att, bacteria def) {
  return !((att.type == bact_type::toxic && def.type == bact_type::immune) ||
           (att.type == bact_type::immune && def.type == bact_type::normal) ||
           (att.type == bact_type::normal && def.type == bact_type::toxic) ||
           (att.type == def.type));
}

class cell {
  public:
    cell() {
      locked = false;
    }
    
    void clear() {
      inhabitant.clear();
    }
    
    bool empty() {
      return inhabitant.empty();
    }
    
    bool inhabit(bacteria b) {
      if (!inhabitant.empty()) {
        if (!attack(b, inhabitant.at(0))) {
          return false;
        }
      }
      inhabitant.clear();
      inhabitant.push_back(b);
      return true;
    }
    
    bool locked;
    
    std::vector<bacteria> inhabitant{};
};

class grid {
  public:
    grid(int width, int height, int bs) {
      for (int i = 0; i < height; i++) {
        std::vector<cell> v{};
        for (int o = 0; o < width; o++) {
          cell c;
          v.push_back(c);
        }
        cells.push_back(v);
      }
      bact_size = bs;
    }
    
    int bact_size;
    
    cell &get_cell(int x, int y) {
      return cells.at(y).at(x);
    }
    
    void process(sf::RenderWindow &window, sf::Color c_tox, sf::Color c_imm, sf::Color c_nor) {
      for (auto& v : cells) {
        for (auto& c : v) {
          c.locked = false;
        }
      }
      for (unsigned int y = 0; y < cells.size(); y++) {
        for (unsigned int x = 0; x < cells.at(y).size(); x++) {
          cell &c = cells.at(y).at(x);
          if (!(c.empty() || c.locked)) {
            int ry = y;
            int rx = x;
            switch (std::rand() % 4) {
              case 0:
                ry -= 1;
                if (ry < 0) ry = cells.size() - 1;
                if (get_cell(rx, ry).inhabit(c.inhabitant.at(0).get_child()))
                  get_cell(rx, ry).locked = true;
                break;
              case 1:
                ry += 1;
                if (static_cast<unsigned int>(ry) >= cells.size()) ry = 0;
                if (get_cell(rx, ry).inhabit(c.inhabitant.at(0).get_child()))
                  get_cell(rx, ry).locked = true;
                break;
              case 2:
                rx -= 1;
                if (rx < 0) rx = cells.at(y).size() - 1;
                if (get_cell(rx, ry).inhabit(c.inhabitant.at(0).get_child()))
                  get_cell(rx, ry).locked = true;
                break;
              case 3:
                rx += 1;
                if (static_cast<unsigned int>(rx) >= cells.at(y).size()) rx = 0;
                if (get_cell(rx, ry).inhabit(c.inhabitant.at(0).get_child()))
                  get_cell(rx, ry).locked = true;
                break;
            }
          }
          if (!c.empty()) {
            sf::RectangleShape rect(sf::Vector2f(bact_size, bact_size));
            rect.setPosition(x * bact_size, y * bact_size);
            switch (c.inhabitant.at(0).type) {
              case bact_type::toxic:
                rect.setFillColor(c_tox);
                break;
              case bact_type::normal:
                rect.setFillColor(c_nor);
                break;
              case bact_type::immune:
                rect.setFillColor(c_imm);
                break;
            }
            window.draw(rect);
          }
        }
      }
    }
    
  private:
    std::vector<std::vector<cell>> cells{};
};

int main() {
  ///////////////////////////////////////////////////
  // Constants //
  const int grid_width = 100;
  const int grid_height = 100;
  const int bact_size = 5;
  ///////////////
  const sf::Color toxic_color(255, 255, 0);
  const sf::Color immune_color(255, 0, 255);
  const sf::Color normal_color(0, 255, 255);
  ///////////////////////////////////////////////////
  const int screen_width = grid_width * bact_size;
  const int screen_height = grid_height * bact_size;
  ///////////////////////////////////////////////////
  
  std::srand(std::time(0));
  grid g(grid_width, grid_height, bact_size);
  
  ///////////////////////////////////////////////////
  // Starting positions //
  {
    bacteria b1(bact_type::toxic);
    g.get_cell(88, 29).inhabit(b1);
    bacteria b2(bact_type::normal);
    g.get_cell(24, 67).inhabit(b2);
    bacteria b3(bact_type::immune);
    g.get_cell(54, 38).inhabit(b3);
  }
  ///////////////////////////////////////////////////
  
  sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "Rock Paper Scissors - Bacteria Simulator", sf::Style::Titlebar | sf::Style::Close);
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    window.clear();
    g.process(window, toxic_color, immune_color, normal_color);
    window.display();
  }
  
  return 0;
}