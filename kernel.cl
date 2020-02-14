float2 getforce(int cellposition, __global int* tankGrid, __global float* xTank, __global float* yTank, int currentTank)
{
    float forcex = 0;
    float forcey = 0;
    float dirX;
    float dirY;
    float radius = 12.0f;
    float result;
    int tankId;
    //printf("tankGrid: %d , YTank: %f XTank: %f cellposition %d currenttank %d \n", tankGrid[cellposition], yTank[currentTank], xTank[currentTank],cellposition,currentTank);
    float colSquaredLen = ((radius * radius) + (radius * radius));
    const int maximumUnitsInCell = 10;
    for (int i = 0; i < maximumUnitsInCell; i++)
    {
        tankId = tankGrid[cellposition + i];
        if (tankId == -1)
        {

                break;
        }
        if (currentTank != tankId)
        {
            
            
            dirX = xTank[currentTank] - xTank[tankId];
            dirY = yTank[currentTank] - yTank[tankId];

            result = ((dirX * dirX) + (dirY * dirY));
            //printf("xtank %f ytank %f xothertank %f yothertank %f result %f id: %d\n", xTank[currentTank], yTank[currentTank], xTank[tankId], yTank[tankId] ,result,currentTank);
            //printf("dirX %f dirY %f id: %d\n", dirX, dirY, currentTank);
            if (result < colSquaredLen && tankId > currentTank)
            {
                //printf("collided currentTank %d  otherTank %d\n",currentTank,tankId);
                float length = sqrt((dirX * dirX) + (dirY * dirY));
                float r = 1.0f / length;
                forcex += dirX * r;
                forcey += dirY * r;
                //printf("X currentTankx: %f currentTanky: %f ID: %d || otherTank X : %f  otherTankY : %f ID: %d\n", xTank[currentTank], yTank[currentTank],currentTank,xTank[tankId],yTank[tankId],tankId);
            }
        }
    }
    //printf("forceX: %f forceY: %f Id: %d\n", forcex, forcey, currentTank);
    float2 direction = {forcex, forcey};
    forcex =0;
    forcey =0;
    return direction;
}
__kernel void tank_collision(__global const int* tankGrid, __global const float* xTank, __global const float* yTank, __global float* xForce, __global float* yForce)
{

    int i = get_global_id(0);
    const int cellsize = 26;
    const int gridOffset = 15;
    const int numberOfCells = 250;
    const int maximumUnitsInCell = 200;
    const int threadspoepe = 1280;
    if (i < threadspoepe)
    {
        float2 force = {0, 0};
        int cellX = (int)((xTank[i] / cellsize) + gridOffset);
        int cellY = (int)((yTank[i] / cellsize) + gridOffset);
        int celloftank = (cellY * (numberOfCells * maximumUnitsInCell)) + (cellX * maximumUnitsInCell);
        int tankindex = tankGrid[celloftank];
        //printf("f \n");
        //prints input data
        //printf(" celoftank %d  xTank %f yTank %f || CellX %d CellY %d  || Id %d\n" ,celloftank, xTank[i], yTank[i], cellX , cellY , i);
        force += getforce(celloftank, tankGrid, xTank, yTank, i);
        if (cellY < numberOfCells - 1)
        {
            int cellNorth = (cellY + 1 * (numberOfCells * maximumUnitsInCell)) + (cellX * maximumUnitsInCell);
            force += getforce(cellNorth, tankGrid, xTank, yTank, i);
        }
        if (cellY > 0)
        {
            int cellSouth = ((cellY - 1) * (numberOfCells * maximumUnitsInCell)) + (cellX * maximumUnitsInCell);
            force += getforce(cellSouth, tankGrid, xTank, yTank, i);
        }

        if (cellX < numberOfCells - 1)
        {
            int cellWest = (cellY * (numberOfCells * maximumUnitsInCell)) + ((cellX - 1) * maximumUnitsInCell);
            force += getforce(cellWest, tankGrid, xTank, yTank, i);
        }

        if (cellX > 0)
        {
            int cellEast = (cellY * (numberOfCells * maximumUnitsInCell)) + (cellX * maximumUnitsInCell);
            force += getforce(cellEast, tankGrid, xTank, yTank, i);
        }

        if (cellX > 0 && cellY > 0)
        {
            int cellNorthEast = ((cellY + 1) * (numberOfCells * maximumUnitsInCell)) + ((cellX + 1) * maximumUnitsInCell);
            force += getforce(cellNorthEast, tankGrid, xTank, yTank, i);
        }

        if (cellX > 0 && cellY < numberOfCells - 1)
        {
            int cellSouthEast = ((cellY + 1) * (numberOfCells * maximumUnitsInCell)) + ((cellX - 1) * maximumUnitsInCell);
            force += getforce(cellSouthEast, tankGrid, xTank, yTank, i);
        }

        if (cellX < numberOfCells - 1 && cellY > 0)
        {
            int cellNorthWest = ((cellY - 1) * (numberOfCells * maximumUnitsInCell)) + ((cellX + 1) * maximumUnitsInCell);
            force += getforce(cellNorthWest, tankGrid, xTank, yTank, i);
        }

        if (cellX < numberOfCells - 1 && cellY < numberOfCells - 1)
        {
            int cellSouthWest = ((cellY + 1) * (numberOfCells * maximumUnitsInCell)) + ((cellX + 1) * maximumUnitsInCell);
            force += getforce(cellSouthWest, tankGrid, xTank, yTank, i);
        }
        //printf("GPU foce x: %f force y: %f ID: %d \n",force.x , force.y , i);
        xForce[i] = force.x;
        yForce[i] = force.y;
    }
}
