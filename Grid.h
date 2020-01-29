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
      void handleTank(Tank* tank, std::vector<Tank*>);
      void addTank2Cell(Tank* tank);
      void handleTankCell(int x , int y,Tank*);
      void moveTank2NewCell(Tank* tank, vec2 oldPosition);
      void moveRocket2NewCell(Rocket* rocket, vec2 oldPosition);
      void handelRocket(Rocket* rocket,Tank* tank);
      void collision(Tank* tank, Tank* other);
      void handelRocketCell(int x, int y,Rocket*);
      static const int numberOfCells = 250;
      static const int sizeOfCell = 26;
      static const int gridOffset = 15;

    private:
      int largestsize; //largest 33 
     std::vector<Tank*> tankcells[numberOfCells][numberOfCells];

     
};
}

