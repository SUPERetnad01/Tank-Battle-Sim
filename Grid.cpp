#include "precomp.h"
#include "Grid.h"
#include <cmath>

void BattleSim::Grid::handleTank(Tank* tank, Tank* other)
{
    /*while (other != NULL)
    {
        if (colliding(tank, other))
        {
            handlePush();
        }
        other = other->next_;
    }*/
}

void BattleSim::Grid::addTank2Cell(Tank* tank)
{
    /*int cellX = (int)(tank->position.x / Grid::sizeOfCell);
    int cellY = (int)(tank->position.y / Grid::sizeOfCell);
    if (cellX < 0)
    {
        cellX = 0;
    }
    if (cellY < 0)
    {
        cellY = 0;
    }
    tank->prev_ = NULL;
    tank->next_ = cells_[cellX][cellY];
    cells_[cellX][cellY] = tank;

    if (tank->next_ != NULL)
    {
        tank->next_->prev_ = tank;
    }*/
}

void BattleSim::Grid::handleCell(int x, int y)
{
    /*Tank* tank = cells_[x][y];
    while (tank != NULL)
    {
        handelTank(tank, tank->next_);

        if (x > 0 && y > 0) handleTank(tank, cells_[x - 1][y - 1]);
        if (x > 0) handleTank(tank, cells_[x - 1][y]);
        if (y > 0) handleTank(tank, cells_[x][y - 1]);
        if (x > 0 && y < numberOfCells - 1)
        {
            handleTank(tank, cells_[x - 1][y + 1]);
        }

        tank = tank->next_;
    }*/
}

void BattleSim::Grid::moveTank2NewCell(Tank* tank, vec2 targetPosition)
{
}

bool BattleSim::Grid::collision(Tank* tank, Tank* other)
{
    /*vec2 dir = tank->Get_Position() - other->Get_Position();
    float dirSquaredLen = dir.sqrLength();

    float colSquaredLen = (tank->Get_collision_radius() * tank->Get_collision_radius()) +
                          (other->Get_collision_radius() * other->Get_collision_radius());
    if (dirSquaredLen < colSquaredLen)
    {
        tank->Push(dir.normalized(), 1.f);
        return true;
    }
    return false;*/
}

void BattleSim::Grid::handlePush()
{
    /*for (int x = 0; x < numberOfCells; x++)
    {
        for (int y = 0; y < numberOfCells; y++)
        {
            handleCell(x, y);
        }
    }*/
}
