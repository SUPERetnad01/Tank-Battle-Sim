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
      bool handleTank(Tank* tank, Tank* other);
      void addTank2Cell(Tank* tank);
      void addRocket2Cell(Rocket* rocket);
      void handleTankCell(int x , int y);
      void moveTank2NewCell(Tank* tank, vec2 oldPosition);
      void moveRocket2NewCell(Rocket* rocket, vec2 oldPosition);
      void handelRocket(Rocket* rocket,Tank* tank);
      bool collision(Tank* tank, Tank* other ,vec2 direction);
      void handelRocketCell(int x, int y);
      static const int numberOfCells = 1000;
      static const int sizeOfCell = 26;
      static const int gridOffset = 15;

    private:
      Rocket* rocketcells[numberOfCells][numberOfCells];
      Tank* tankcells[numberOfCells][numberOfCells];
};
}

