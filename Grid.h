#pragma once

#include "Tank.h"
#include <vector>
#include "template.h"

namespace BattleSim
{
class Grid
{
  public:
    Grid(){
        //clear the grid
        for (int x = 0; x < numberOfCells; x++)
        {
            for (int y = 0; y < numberOfCells; ++y)
            {
                cells_[x][y] = NULL;
            }
        }
    };
      ~Grid();
      void handleTank(Tank* tank, Tank* other);
      void addTank2Cell(Tank* tank);
      void handleCell(int x, int y);
      void moveTank2NewCell(Tank* tank, vec2 targetPosition);

      bool collision(Tank* tank, Tank* other);
      void handlePush();
      static const int numberOfCells = 1000;
      static const int sizeOfCell = 20;

    private:
      Tank* cells_[numberOfCells][numberOfCells];
};
}

