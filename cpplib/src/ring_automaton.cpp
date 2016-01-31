#include "ring_automaton.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <utility>

RingAutomaton::RingAutomaton(const int width, const int height)
  : grid_(new Image(width, height)) {
  // Seed the random number generator to ensure it's seeded at least once...
  srand(time(0));
  // Start the grid from a clean slate.
  ClearGrid();  
}

void RingAutomaton::RandomState() {
  for (int i = 0; i < grid_->getWidth(); ++i) {
    for (int j = 0; j < grid_->getHeight(); ++j) {
      const bool on_or_off = rand()%2;
      SetCell(i, j, on_or_off);
    }
  }
}

void RingAutomaton::ClearGrid() {
  for (int i = 0; i < grid_->getWidth(); ++i) {
    for (int j = 0; j < grid_->getHeight(); ++j) {
      SetCell(i, j, false);
    }
  }
}

void RingAutomaton::Poll() {
  std::unique_ptr<Image> next_grid(new Image(grid_->getWidth(),
      grid_->getHeight()));
  for (int i = 0; i < grid_->getWidth(); ++i) {
    for (int j = 0; j < grid_->getHeight(); ++j) {
      const int number_neighbors = GetNumberNeighbors(i, j);
      const bool is_alive = ShouldCellSurvive(i, j, number_neighbors);
      const Pixel value = Pixel(is_alive, 0, 0, 0);
      next_grid->setPixel(i, j, value);
    }
  }
  grid_ = std::move(next_grid);
}

// WrapX: The SpaceX of Bubble Wrap.
int RingAutomaton::WrapX(const int x) const {
  const int width = grid_->getWidth();
  const int rollover_x = (x + width) % width;
  return rollover_x;
}


void RingAutomaton::SetCell(const int x, const int y, const bool value) {
  const int rollover_x = WrapX(x);
  grid_->setPixel(rollover_x, y, Pixel(value, 0, 0, 0));
}

bool RingAutomaton::GetCell(const int x, const int y) const {
  const int rollover_x = WrapX(x);
  const Pixel p = grid_->getPixel(rollover_x, y);
  return p.r;
}

Vec4 RingAutomaton::GetCellSphereCoordinate(const int x, const int y) const {
  // en.wikipedia.org/wiki/Spherical_coordinate_system
  // All angles are in radians. Degrees are for wimps.
  const int rollover_x = WrapX(x);
  float x_center = rollover_x + 0.5;
  float y_center = y + 0.5;
  const float r = 1;  // unit sphere.
  const float phi = (x_center/GetWidth()) * 2 * M_PI;
  // For theta we basically want to map the square's y-coordinate to somewhere between PI/10 and (2PI - PI/10).
  const float offset = M_PI/10;
  const float range = M_PI - 2*offset;
  const float theta = (y_center/GetHeight())*range + offset;
  // Okay so spherical coordinates were convenient for describing the location
  // of the grid's coordinate on a sphere... now we have to convert to
  // cartesian.
  Vec4 on_a_sphere;
  on_a_sphere.x = r*sin(theta)*cos(phi);
  on_a_sphere.z = r*sin(theta)*sin(phi);
  on_a_sphere.y = r*cos(theta);
  return on_a_sphere;
}

int RingAutomaton::GetWidth() const {
  return grid_->getWidth();
}

int RingAutomaton::GetHeight() const {
  return grid_->getHeight();
}

std::string RingAutomaton::GetDebugString() const {
  std::string s;
  for (int i = 0; i < grid_->getHeight(); ++i) {
    for (int j = 0; j < grid_->getWidth(); ++j) {
      if (GetCell(j, i)) {
        s += " ";
        s += std::to_string(GetNumberNeighbors(j, i));
      } else {
        s += "__";
      }
    }
    s += "\n";
  }
  return s;
}

int RingAutomaton::GetNumberNeighbors(const int x, const int y) const {
  int counter = 0;
  for (int i = x - 1; i <= x + 1; ++i) {
    for (int j = y - 1; j <= y + 1; ++j) {
      // Skip cases where the height is negative or greater than the heigh of
      // the map.
      if ((j >= grid_->getHeight()) || (j < 0)) {
        continue;
      }
      // Skip (x, y), that's us silly, not a neighbor.
      if ((i == x) && (j == y)) {
        continue;
      }
      counter += static_cast<int>(GetCell(i, j));
    }
  }
  return counter;
}
