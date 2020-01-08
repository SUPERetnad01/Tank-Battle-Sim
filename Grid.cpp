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

void BattleSim::Grid::handleTank(Tank* tank, Tank* other)
{
    while(other != nullptr)
    {
        collision(tank, other);
        other = other->next_;
    }
}

void BattleSim::Grid::addTank2Cell(Tank* tank)
{
    int cellX = (int)((tank->position.x / Grid::sizeOfCell)+gridOffset);
    int cellY = (int)((tank->position.y / Grid::sizeOfCell)+gridOffset);
    tankcells[cellX][cellY];
    tank->prev_ = nullptr;
    tank->next_ = tankcells[cellX][cellY];
    tankcells[cellX][cellY] = tank;
    if (tank->next_ != nullptr)
    {
        tank->next_->prev_ = tank;
    }
}

void BattleSim::Grid::addRocket2Cell(Rocket* rocket)
{
    int cellX = (int)((rocket->position.x / Grid::sizeOfCell) + gridOffset);
    int cellY = (int)((rocket->position.y / Grid::sizeOfCell) + gridOffset);
    
    rocketcells[cellX][cellY] = rocket;
}

void BattleSim::Grid::handleCell(int x, int y)
{
    tankcells[x][y];
    Tank* tank = tankcells[x][y];
    while (tank != nullptr)
    {
        handleTank(tank, tank->next_); // spot 5
        //     1 | 2 | 3
        //     4 | 5 | 6
        //     7 | 8 | 9 

       if (tank->allignment == RED) {
           if (x < numberOfCells - 1){
               handleTank(tank, tankcells[x + 1][y]); //6
           }
      }
        else if (tank->allignment == BLUE)
        {
            if (x > 0) handleTank(tank, tankcells[x - 1][y]); // 4
        }
        if (x > 0 && y > 0) handleTank(tank, tankcells[x - 1][y - 1]); // 7 
        if (y > 0) handleTank(tank, tankcells[x][y - 1]); // 8
        
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
        if (y < numberOfCells - 1) handleTank(tank, tankcells[x][y + 1]); // 2
       
        tank = tank->next_;
    }
}

void BattleSim::Grid::moveTank2NewCell(Tank* tank, vec2 oldposition)
    {
        
    // See which cell it was in.
    int oldCellX = (int)((oldposition.x / Grid::sizeOfCell) + gridOffset);
    int oldCellY = (int)((oldposition.y / Grid::sizeOfCell) + gridOffset);

    int cellX = (int)((tank->position.x / Grid::sizeOfCell) + gridOffset);
    int cellY = (int)((tank->position.y / Grid::sizeOfCell) + gridOffset);

    tankcells[oldCellX][oldCellY];
    // handelCell(cellX,cellY);
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

    // If it's the head of a list, remove it.
    if (tankcells[oldCellX][oldCellY] == tank)
    {
        tankcells[oldCellX][oldCellY] = tank->next_;
    }

    // Add it back to the grid at its new cell.
    addTank2Cell(tank);
    handleCell(cellX, cellY);
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
        // handelCell(cellX,cellY);
        // If it didn't change cells, we're done.
        if (oldCellX == cellX && oldCellY == cellY) return;

        // Add it back to the grid at its new cell.
        addRocket2Cell(rocket);
        handelRocketCell(cellX, cellY);
    }

    void BattleSim::Grid::handelRocket(Rocket* rocket,Tank* tank)
    {
        if (rocket->Intersects(tank->position, tank->collision_radius) && tank->active && (tank->allignment != rocket->allignment))
        {
            rocket->setHitTank(tank);
            rocket->hitTarget = true;
            //rocket->active = false;
           
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
        vec2 oldPosition = tank->position;
        tank->Push(dir.normalized(), 1.f);
        moveTank2NewCell(tank, oldPosition);

    }
}

void BattleSim::Grid::handelRocketCell(int x, int y)
{
    tankcells[x][y];
    Tank* tank = tankcells[x][y];
    Rocket* rocket = rocketcells[x][y];
    while (tank != NULL)
    { 
        if (tank->active) {
            handelRocket(rocket, tank);
            //     1 | 2 | 3
            //     4 | 5 | 6
            //     7 | 8 | 9
        }
        tank = tank->next_;
    }
}
