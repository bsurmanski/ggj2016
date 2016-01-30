#include "ring_automaton.hpp"

#include <unistd.h>

#include <iostream>
#include <vector>

using namespace std;

class GOLRing : public RingAutomaton {
  public:
  GOLRing(const int x, const int y) : RingAutomaton(x, y) {}

  protected:
  bool ShouldCellSurvive(const int x, const int y, const int number_neighbors)
  const override {
    if (number_neighbors < 2 || number_neighbors >= 4) {
      return false;
    } else if (number_neighbors == 2) {
      return GetCell(x, y);
    } else {
      return true;
    }
  }
};

std::vector<std::vector<bool>> space_ship = {
  {0, 1, 0, 0, 1},
  {1, 0, 0, 0, 0},
  {1, 0, 0, 0, 1},
  {1, 1, 1, 1, 0},
};

int main() {
  const int width = 30;
  const int height = 15;
  const int number_generations = 10;
  const unsigned int period = 100000;
  GOLRing a(width, height);
  const int cx = 0;
  const int cy = 5;
  for (int i = 0; i < space_ship.size(); ++i) {
    for (int j = 0; j < space_ship[i].size(); ++j) {
      a.SetCell(j + cx, i + cy, space_ship[i][j]);
    }
  }
  for (int g = 0; g < number_generations; ++g) {
    cout << "{ ";
    for (int i = 0; i < a.GetWidth(); ++i) {
      for (int j = 0; j < a.GetHeight(); ++j) {
        if (a.GetCell(i, j)) {
          Vec4 spherical_coordinate = a.GetCellSphereCoordinate(i, j);
          cout << "{ " << spherical_coordinate.x << ", " << spherical_coordinate.y << ", " << spherical_coordinate.z << "},";
          cout << endl;
        }
      }
    }
    cout << "},";
    a.Poll();
  }
}
