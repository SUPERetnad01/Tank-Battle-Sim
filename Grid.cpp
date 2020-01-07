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
            cells_[x][y] = nullptr;
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
        if (collision(tank, other))
        {
        }
        other = other->next_;
    }
}

void BattleSim::Grid::addTank2Cell(Tank* tank)
{
    int cellX = (int)(tank->position.x / Grid::sizeOfCell);
    int cellY = (int)(tank->position.y / Grid::sizeOfCell);
    cells_[cellX][cellY];
    tank->prev_ = nullptr;
    tank->next_ = cells_[cellX][cellY];
    cells_[cellX][cellY] = tank;
    if (tank->next_ != nullptr)
    {
        tank->next_->prev_ = tank;
    }
}

void BattleSim::Grid::handleCell(int x, int y)
{
    cells_[x][y];
    Tank* tank = cells_[x][y];
    while (tank != nullptr)
    {
        handleTank(tank, tank->next_);

        if (x > 0 && y > 0) handleTank(tank, cells_[x - 1][y - 1]);
        if (x > 0) handleTank(tank, cells_[x - 1][y]);
        if (y > 0) handleTank(tank, cells_[x][y - 1]);
        if (y < numberOfCells - 1) handleTank(tank, cells_[x][y + 1]);
        if (x > 0 && y < numberOfCells - 1)
        {
            handleTank(tank, cells_[x - 1][y + 1]);
        }
        if (x < numberOfCells - 1 && y < numberOfCells - 1)
        {
            handleTank(tank, cells_[x + 1][y + 1]);
        }
        if (x < numberOfCells - 1)
        {
            handleTank(tank, cells_[x + 1][y]);
        }
        if (x < numberOfCells - 1 && y > 0)
        {
            handleTank(tank, cells_[x + 1][y - 1]);
        }
        if (x > 1 && y > 1) handleTank(tank, cells_[x - 2][y - 2]);
        if (x > 1) handleTank(tank, cells_[x - 2][y]);
        if (y > 1) handleTank(tank, cells_[x][y - 2]);
        if (y < numberOfCells - 2) handleTank(tank, cells_[x][y + 2]);
        if (x > 1 && y < numberOfCells - 2)
        {
            handleTank(tank, cells_[x - 2][y + 2]);
        }
        if (x < numberOfCells - 2 && y < numberOfCells - 2)
        {
            handleTank(tank, cells_[x + 2][y + 2]);
        }
        if (x < numberOfCells - 2)
        {
            handleTank(tank, cells_[x + 2][y]);
        }
        if (x < numberOfCells - 2 && y > 1)
        {
            handleTank(tank, cells_[x + 2][y - 2]);
        }
        tank = tank->next_;
    }
}

void BattleSim::Grid::moveTank2NewCell(Tank* tank, vec2 oldposition)
    {
        
    // See which cell it was in.
    int oldCellX = (int)(oldposition.x / Grid::sizeOfCell);
    int oldCellY = (int)(oldposition.y / Grid::sizeOfCell);

    int cellX = (int)(tank->position.x / Grid::sizeOfCell);
    int cellY = (int)(tank->position.y / Grid::sizeOfCell);
    cells_[oldCellX][oldCellY];
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
    if (cells_[oldCellX][oldCellY] == tank)
    {
        cells_[oldCellX][oldCellY] = tank->next_;
    }

    // Add it back to the grid at its new cell.
    addTank2Cell(tank);
    handleCell(cellX, cellY);
}

bool BattleSim::Grid::collision(Tank* tank, Tank* other)
{
    vec2 dir = tank->Get_Position() - other->Get_Position();
    float dirSquaredLen = dir.sqrLength();

    float colSquaredLen = (tank->Get_collision_radius() * tank->Get_collision_radius()) +
                          (other->Get_collision_radius() * other->Get_collision_radius());
    if (dirSquaredLen < colSquaredLen)
    {
        tank->Push(dir.normalized(), 1.f);
        return true;
    }
    return false;
}
