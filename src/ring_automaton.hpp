#ifndef RING_AUTOMATON_HPP
#define RING_AUTOMATON_HPP 
#include "draw/image.hpp"
#include "geom/vec.hpp"

#include <vector>
#include <string>
#include <memory>

// This abstract class simulates a custom finite automaton on a special kind of
// grid. The grid is a ring in the x-direction: The right edge of the map loops
// back to connect with the left edge, so that (width + 1, y) = (0, y).
class RingAutomaton {
  public:
  RingAutomaton(const int width, const int height);

  // Initialize all cells in the automaton. 
  void RandomState();

  // Clear all cells in the automaton.
  void ClearGrid();
  
  // Run one cycle of the automaton.
  void Poll();

  // Does modulus operation for the x coordinate (getWidth()) -> 0, -1 ->
  // (getWidth() - 1).
  int WrapX(const int x) const; 

  // If x isgreater than the width, wrap around the edge of the grid since it's
  // a ring. Do NOT do the same for y. Set the boolean value at location (x,y).
  void SetCell(const int x, const int y, const int value);
  
  // If x isgreater than the width, wrap around the edge of the grid since it's
  // a ring. Do NOT do the same for y. Get the boolean value at location (x,y).
  int GetCell(const int x, const int y) const;

  // Imagine we mapped this ring onto a unit sphere. Obviously the tops and bottoms
  // wouldn't really "fit" with this mapping, so imagine the tops and bottoms
  // of the sphere were cut off. This function maps cells from the ring to that
  // unit sphere (tops and bottoms uncovered).
  Vec4 GetCellSphereCoordinate(const int x, const int y) const; 
  int GetCellFromSphereCoords(Vec4 v) const;

  int GetWidth() const;
  int GetHeight() const;

  std::string GetDebugString() const;

  protected:
  int GetNumberNeighbors(const int x, const int y) const;

  virtual bool ShouldCellSurvive(const int x, const int y,
      const int number_neighbors) const = 0;
  
  private:
  std::unique_ptr<Image> grid_;
};

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

class PacmanMap : public RingAutomaton {
    public:
    PacmanMap() : RingAutomaton(19, 21) {
        std::vector<std::vector<int>> MAP = {
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,0,1},
            {1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1},
            {1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1},
            {1,1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,1,1},
            {1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1},
            {0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0},
            {1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1},
            {1,1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,1,1},
            {1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1},
            {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
            {1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1},
            {1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1},
            {1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        };

        ClearGrid();
        for(int i = 0; i < MAP.size(); i++) {
            for(int j = 0; j < MAP[i].size(); j++) {
                SetCell(i, j, MAP[i][j]);
            }
        }
    }
  virtual bool ShouldCellSurvive(const int x, const int y,
      const int number_neighbors) const override {
      return GetCell(x,y) == 1;
  }
};

#endif /* RING_AUTOMATON_HPP */
