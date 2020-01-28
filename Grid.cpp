#include "precomp.h"
#include "Grid.h"
#include <cmath>


BattleSim::Grid::Grid()
{
     //clear the grid
    for (int x = 0; x < numberOfCells; x++)
    {
        for (int y = 0; y < numberOfCells; ++y)
        {
            tankcells[x][y] = nullptr;
        }
    }
}

BattleSim::Grid::~Grid()
{
}

void BattleSim::Grid::addTank2Cell(Tank* tank)
{
    int cellX = (int)((tank->position.x / Grid::sizeOfCell)+gridOffset);
    int cellY = (int)((tank->position.y / Grid::sizeOfCell)+gridOffset);
    tank->prev_ = nullptr;
    tank->next_ = tankcells[cellX][cellY];
    tankcells[cellX][cellY] = tank;
    if (tank->next_ != nullptr)
    {
        tank->next_->prev_ = tank;
    }
}

void BattleSim::Grid::moveTank2NewCell(Tank* tank, vec2 oldposition)
    {  
    // See which cell it was in.
    int oldCellX = (int)((oldposition.x / Grid::sizeOfCell) + gridOffset);
    int oldCellY = (int)((oldposition.y / Grid::sizeOfCell) + gridOffset);

    int cellX = (int)((tank->position.x / Grid::sizeOfCell) + gridOffset);
    int cellY = (int)((tank->position.y / Grid::sizeOfCell) + gridOffset);
    
    // If it didn't change cells, we're done.
    if (oldCellX == cellX && oldCellY == cellY) return;
    // Unlink it from the list of its old cell.
    if (tank->prev_ != nullptr)
    {
        tank->prev_->next_ = tank->next_;
    }
    if (tank->next_ != nullptr)
    {
        tank->next_->prev_ = tank->prev_;
    }

    if (tank->next_ != nullptr)
    {
        tank->next_->prev_ = tank->prev_;
    }

    // If it's the head of a list, remove it.
    if (tankcells[oldCellX][oldCellY] == tank)
    {
        tankcells[oldCellX][oldCellY] = tank->next_;
    }
    tank->CellX = cellX;
    tank->CellY = cellY;
    // Add it back to the grid at its new cell.
    addTank2Cell(tank);
    }

void BattleSim::Grid::handleTankCell(int x ,int y ,Tank* tank)
{                                        //     1 | 2 | 3
    Tank* currentTankCell = tankcells[x][y];        //     4 | 5 | 6
    if(tank != nullptr)                  //     7 | 8 | 9
    {
        if (tank->next_ != nullptr) {
            handleTank(tank, currentTankCell);
        }
        if (x < numberOfCells - 1)
        {
            handleTank(tank, tankcells[x + 1][y]);
        }
        if (x > 0) // 4
        {
            handleTank(tank, tankcells[x - 1][y]);
        }
                                                                
        if (x > 0 && y > 0)
        {
            handleTank(tank, tankcells[x - 1][y - 1]);
        } // 7

        if (y > 0)
        {
            handleTank(tank, tankcells[x][y - 1]);
        } // 8

        if (x < numberOfCells - 1 && y > 0)
        {
            handleTank(tank, tankcells[x + 1][y - 1]); //9    
        }
        if (x < numberOfCells - 1 && y < numberOfCells - 1)
        {
            handleTank(tank, tankcells[x + 1][y + 1]); // 3  
        }
        if (x > 0 && y < numberOfCells - 1){
            handleTank(tank, tankcells[x - 1][y + 1]); // 1
         }
        if (y < numberOfCells - 1){
            handleTank(tank, tankcells[x][y + 1]); // 2
        }  
    }
}

void BattleSim::Grid::handleTank(Tank* tank, Tank* other)
{ 
    while (other != nullptr)
    {
        if (tank != other)
        {
            collision(tank, other);
            
        }
        other = other->next_;
    }    
}

void BattleSim::Grid::collision(Tank* tank, Tank* other)
{
    vec2 dir = tank->Get_Position() - other->Get_Position();
    float dirSquaredLen = dir.sqrLength();
    float colSquaredLen = (tank->Get_collision_radius() * tank->Get_collision_radius()) +
                          (other->Get_collision_radius() * other->Get_collision_radius());
    if (dirSquaredLen < colSquaredLen)
    {
        tank->Push(dir.normalized(), 1.f);
    }
}

void BattleSim::Grid::addRocket2Cell(Rocket* rocket)
{
    int cellX = (int)((rocket->position.x / Grid::sizeOfCell) + gridOffset);
    int cellY = (int)((rocket->position.y / Grid::sizeOfCell) + gridOffset);
    
    rocketcells[cellX][cellY] = rocket;
}

void BattleSim::Grid::moveRocket2NewCell(Rocket* rocket, vec2 oldposition)
    {

        // See which cell it was in.
        int oldCellX = (int)((oldposition.x / Grid::sizeOfCell) + gridOffset);
        int oldCellY = (int)((oldposition.y / Grid::sizeOfCell) + gridOffset);

        int cellX = (int)((rocket->position.x / Grid::sizeOfCell) + gridOffset);
        int cellY = (int)((rocket->position.y / Grid::sizeOfCell) + gridOffset);
        if (cellX < 0 || cellY < 0)
        {
            rocket->active = false;
            return;
        }
        // If it didn't change cells, we're done.
        if (oldCellX == cellX && oldCellY == cellY) return;
       

        // Add it back to the grid at its new cell.
        handelRocketCell(cellX, cellY, rocket);
    }

void BattleSim::Grid::handelRocketCell(int x, int y,Rocket* rocket)
{
    Tank* tank = tankcells[x][y];
    while (tank != NULL)
    { 
        if (tank->active) {
            handelRocket(rocket, tank);
        }
        tank = tank->next_;
    }
}

void BattleSim::Grid::handelRocket(Rocket* rocket,Tank* tank)
    {
        if (rocket->Intersects(tank->position, tank->collision_radius) && tank->active && (tank->allignment != rocket->allignment))
        {
            rocket->setHitTank(tank);
            rocket->hitTarget = true;
        }
    }
