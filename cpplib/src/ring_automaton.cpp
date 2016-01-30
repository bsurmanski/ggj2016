#include "ring_automaton.hpp"
#include <ctime>
#include <cstdlib>
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

void RingAutomaton::SetCell(const int x, const int y, const bool value) {
  const int rollover_x = x % grid_->getWidth();
  grid_->setPixel(rollover_x, y, Pixel(value, 0, 0, 0));
}

bool RingAutomaton::GetCell(const int x, const int y) const {
  const int rollover_x = x % grid_->getWidth();
  const Pixel p = grid_->getPixel(rollover_x, y);
  return p.r;
}

int RingAutomaton::GetWidth() const {
  return grid_->getWidth();
}

int RingAutomaton::GetHeight() const {
  return grid_->getHeight();
}

std::string RingAutomaton::GetDebugString() const {
  std::string s;
  for (int i = 0; i < grid_->getWidth(); ++i) {
    for (int j = 0; j < grid_->getHeight(); ++j) {
      if (GetCell(i, j)) {
        s += "*";
      } else {
        s += " ";
      }
    }
    s += "\n";
  }
  return s;
}

int RingAutomaton::GetNumberNeighbors(const int x, const int y) const {
  int counter = 0;
  for (int i = x - 1; i < x + 1; ++i) {
    for (int j = y - 1; j < y + 1; ++j) {
      // Skip cases where the height is negative or greater than the heigh of
      // the map.
      if ((j > grid_->getHeight()) || (j < 0)) {
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
