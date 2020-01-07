#pragma once

#include "Tank.h"
#include <vector>
#include "template.h"

namespace BattleSim
{
class Grid
{
  public:
    Grid();
      ~Grid();
      void handleTank(Tank* tank, Tank* other);
      void addTank2Cell(Tank* tank);
      void handleCell(int x, int y);
      void moveTank2NewCell(Tank* tank, vec2 targetPosition);

      bool collision(Tank* tank, Tank* other);
      static const int numberOfCells = 1000;
      static const int sizeOfCell = 18;

    private:
      Tank* cells_[numberOfCells][numberOfCells];
};
}

