#ifndef RING_AUTOMATON_HPP
#define RING_AUTOMATON_HPP 
#include "draw/image.hpp"

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

  // If x isgreater than the width, wrap around the edge of the grid since it's
  // a ring. Do NOT do the same for y. Set the boolean value at location (x,y).
  void SetCell(const int x, const int y, const bool value);
  
  // If x isgreater than the width, wrap around the edge of the grid since it's
  // a ring. Do NOT do the same for y. Get the boolean value at location (x,y).
  bool GetCell(const int x, const int y) const;

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

#endif /* RING_AUTOMATON_HPP */
