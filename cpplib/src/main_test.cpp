#include <iostream>
#include "ring_automaton.hpp"

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

int main() {
  GOLRing a(5, 5);
  a.RandomState();
  for (int i = 0; i < 10; ++i) {
    cout << a.GetDebugString() << endl;
    a.Poll();
    cout << "---------------------" << endl;
  }
}
