#include "QuadTree.h"
#include "precomp.h"

bool BattleSim::QuadTree::insertNode(Tank* node)
{
    if (&node == nullptr) return false;
    if (!inBoundary(node->position)) return false;
    if (points.size() < this->capacity && NWTree == NULL)
    {
        points.push_back(node);
        count++;

        this->points;
        return true;
    }
    this->points;
    if (NWTree == NULL)
    {
        subdivide();
    }
    if (NWTree->insertNode(node)) return true;
    if (NETree->insertNode(node)) return true;
    if (SWTree->insertNode(node)) return true;
    if (SETree->insertNode(node)) return true;

    return false;
}

void BattleSim::QuadTree::subdivide()
{
    NWTree = new QuadTree(
        vec2(topLeftBorder.x, topLeftBorder.y),
        vec2((topLeftBorder.x + botRightBorder.x) / 2,
             (topLeftBorder.y + botRightBorder.y) / 2),
        this);

    NETree = new QuadTree(
        vec2(topLeftBorder.x, (topLeftBorder.y + botRightBorder.y) / 2),
        vec2((topLeftBorder.x + botRightBorder.x) / 2, botRightBorder.y), this);

    SWTree = new QuadTree(
        vec2((topLeftBorder.x + botRightBorder.x) / 2, topLeftBorder.y),
        vec2(botRightBorder.x, (topLeftBorder.y + botRightBorder.y) / 2), this);

    SETree = new QuadTree(
        vec2((topLeftBorder.x + botRightBorder.x) / 2, (topLeftBorder.y + botRightBorder.y) / 2),
        vec2(botRightBorder.x, botRightBorder.y), this);
    for (auto node : points)
    {
        this->NETree->insertNode(node);
        this->NWTree->insertNode(node);
        this->SETree->insertNode(node);
        this->SWTree->insertNode(node);
    }
    points.clear();
}

tuple<Tank*, float> BattleSim::QuadTree::FindClosest(Tank tank, Tank* closestnode = nullptr, float closestdistance = numeric_limits<float>::infinity())
{
    if (this->NETree->count > 0 && this->NETree->rectangle->intersectsCircle(tank.position, closestdistance))
    {
        if (this->NETree->points.size() == 0 && this->NETree->count == 4)
        {
            auto result = this->NETree->FindClosest(tank, closestnode, closestdistance);
            closestnode = get<0>(result);
            closestdistance = get<1>(result);
        }
        else if (this->NETree->points.size() > 0)
        {
            auto tupleNodeDistantce = this->NETree->closestDistanceinPoints(tank, closestdistance, closestnode);
            closestnode = get<0>(tupleNodeDistantce);
            closestdistance = get<1>(tupleNodeDistantce);
        }
    }
    if (this->NWTree->count > 0 && this->NWTree->rectangle->intersectsCircle(tank.position, closestdistance))
    {
        if (this->NWTree->points.size() == 0 && this->NWTree->count == 4)
        {
            auto result = this->NWTree->FindClosest(tank, closestnode, closestdistance);
            closestnode = get<0>(result);
            closestdistance = get<1>(result);
        }
        else if (this->NWTree->points.size() > 0)
        {
            auto tupleNodeDistantce = this->NWTree->closestDistanceinPoints(tank, closestdistance, closestnode);
            closestnode = get<0>(tupleNodeDistantce);
            closestdistance = get<1>(tupleNodeDistantce);
        }
    }
    if (this->SETree->count > 0 && this->SETree->rectangle->intersectsCircle(tank.position, closestdistance))
    {
        if (this->SETree->points.size() == 0 && this->SETree->count == 4)
        {
            auto result = this->SETree->FindClosest(tank, closestnode, closestdistance);
            closestnode = get<0>(result);
            closestdistance = get<1>(result);
        }
        else if (this->SETree->points.size() > 0)
        {
            auto tupleNodeDistantce = this->SETree->closestDistanceinPoints(tank, closestdistance, closestnode);
            closestnode = get<0>(tupleNodeDistantce);
            closestdistance = get<1>(tupleNodeDistantce);
        }
    }
    if (this->SWTree->count > 0 && this->SWTree->rectangle->intersectsCircle(tank.position, closestdistance))
    {
        if (this->SWTree->points.size() == 0 && this->SWTree->count == 4)
        {
            auto result = this->SWTree->FindClosest(tank, closestnode, closestdistance);
            closestnode = get<0>(result);
            closestdistance = get<1>(result);
        }
        else if (this->SWTree->points.size() > 0)
        {
            auto tupleNodeDistantce = this->SWTree->closestDistanceinPoints(tank, closestdistance, closestnode);
            closestnode = get<0>(tupleNodeDistantce);
            closestdistance = get<1>(tupleNodeDistantce);
        }
    }
    return make_tuple(closestnode, closestdistance);
}

vector<Tank*> BattleSim::QuadTree::FindNodesInRange(Tank tank, vector<Tank*> inrange, float range)
{
    if (this->NETree->count > 0 && this->NETree->rectangle->intersectsCircle(tank.position, range))
    {
        if (this->NETree->points.size() == 0 && this->NETree->count == 4)
        {
            inrange = this->NETree->FindNodesInRange(tank, inrange, range);
        }
        else if (this->NETree->points.size() > 0)
        {
            inrange = this->NETree->CollisionCheck(tank, range, inrange);
        }
    }
    if (this->NWTree->count > 0 && this->NWTree->rectangle->intersectsCircle(tank.position, range))
    {
        if (this->NWTree->points.size() == 0 && this->NWTree->count == 4)
        {
            inrange = this->NWTree->FindNodesInRange(tank, inrange, range);
        }
        else if (this->NWTree->points.size() > 0)
        {
            inrange = this->NWTree->CollisionCheck(tank, range, inrange);
        }
    }
    if (this->SETree->count > 0 && this->SETree->rectangle->intersectsCircle(tank.position, range))
    {
        if (this->SETree->points.size() == 0 && this->SETree->count == 4)
        {
            inrange = this->SETree->FindNodesInRange(tank, inrange, range);
        }
        else if (this->SETree->points.size() > 0)
        {
            inrange = this->SETree->CollisionCheck(tank, range, inrange);
        }
    }
    if (this->SWTree->count > 0 && this->SWTree->rectangle->intersectsCircle(tank.position, range))
    {
        if (this->SWTree->points.size() == 0 && this->SWTree->count == 4)
        {
            inrange = this->SWTree->FindNodesInRange(tank, inrange, range);
        }
        else if (this->SWTree->points.size() > 0)
        {
            inrange = this->SWTree->CollisionCheck(tank, range, inrange);
        }
    }
    return inrange;
}

bool BattleSim::QuadTree::inBoundary(vec2 point)
{
    return (point.x <= topLeftBorder.x &&
            point.x >= botRightBorder.x &&
            point.y <= topLeftBorder.y &&
            point.y >= botRightBorder.y);
}

bool BattleSim::QuadTree::removeNode(Tank* node)
{
    if (&node == nullptr) return false;
    if (!inBoundary(node->position)) return false;
    if (points.size() > 0)
    {
        for (int i = 0; i < points.size(); i++)
        {
            if (points[i] = node)
            {
                points.erase(points.begin() + i);
                count--;
                return true;
            }
        }
    }
    if (NWTree == NULL)
    {
        return false;
    }
    if (NWTree->removeNode(node)) return true;
    if (NETree->removeNode(node)) return true;
    if (SWTree->removeNode(node)) return true;
    if (SETree->removeNode(node)) return true;

    return false;
}

vector<Tank*> BattleSim::QuadTree::CollisionCheck(Tank tank, float range, vector<Tank*> nodesinrage)
{

    for (int i = 0; i < points.size(); i++)
    {
        float distance = (tank.position - points[i]->position).length();
        if (distance < range && tank.position.x != points[i]->position.x && tank.position.y != points[i]->position.y)
        {

            nodesinrage.push_back(points[i]);
        }
    }
    return nodesinrage;
}

tuple<Tank*, float> BattleSim::QuadTree::closestDistanceinPoints(Tank tank, float closestdistance, Tank* closestNode)
{
    for (int i = 0; i < points.size(); i++)
    {
        if (points[i]->allignment != tank.allignment)
        {
            float distance = (tank.position - points[i]->position).length();
            if (distance < closestdistance)
            {
                closestNode = points[i];
                closestdistance = distance;
            }
        }
    }
    return make_tuple(closestNode, closestdistance);
}
