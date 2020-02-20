#include "precomp.h"
#include "QuadTree.h"

bool BattleSim::QuadTree::insertNode(Tank* node)
{
    if (&node == nullptr) return false;
    if (!inBoundary(node->position)) return false;
    if (points.size() < this->capacity && NWTree == nullptr)
    {
        points.push_back(node);
        count++;
        return true;
    }

    if (NWTree == nullptr)
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
             (topLeftBorder.y + botRightBorder.y) / 2));

    NETree = new QuadTree(
        vec2(topLeftBorder.x, (topLeftBorder.y + botRightBorder.y) / 2),
        vec2((topLeftBorder.x + botRightBorder.x) / 2, botRightBorder.y));

    SWTree = new QuadTree(
        vec2((topLeftBorder.x + botRightBorder.x) / 2, topLeftBorder.y),
        vec2(botRightBorder.x, (topLeftBorder.y + botRightBorder.y) / 2));

    SETree = new QuadTree(
        vec2((topLeftBorder.x + botRightBorder.x) / 2, (topLeftBorder.y + botRightBorder.y) / 2),
        vec2(botRightBorder.x, botRightBorder.y));

    quad_tree_quadrants.emplace_back(NWTree);
    quad_tree_quadrants.emplace_back(NETree);
    quad_tree_quadrants.emplace_back(SWTree);
    quad_tree_quadrants.emplace_back(SETree);

    for (auto node : points)
    {
        this->NETree->insertNode(node);
        this->NWTree->insertNode(node);
        this->SETree->insertNode(node);
        this->SWTree->insertNode(node);
    }
    points.clear();
}
///
/// looks for the closest enemies
/// by looking at every tree and checking if the radius circle intersects with the quadretents borders 
///
tuple<Tank*, float> BattleSim::QuadTree::FindClosest(Tank tank, Tank* closestnode = nullptr, float closestdistance = numeric_limits<float>::infinity())
{
    // looks in all quadrants  
    for (QuadTree* quad_tree : quad_tree_quadrants)
    {
        //checks if the tree is empty && if the the closest tank radius intersects with the quadrant 
        if (quad_tree->count > 0 && quad_tree->rectangle->intersectsCircle(tank.position, closestdistance))
        {
            //if it is a tree go in it 
            if (quad_tree->points.size() == 0 && quad_tree->count == 4)
            {
                auto result = quad_tree->FindClosest(tank, closestnode, closestdistance);
                closestnode = get<0>(result);
                closestdistance = get<1>(result);
            }
            //if it is a leaf check the nodes in side it 
            else if (quad_tree->points.size() > 0)
            {
                auto tuple_node_distance = quad_tree->closestDistanceinPoints(tank, closestdistance, closestnode);
                closestnode = get<0>(tuple_node_distance);
                closestdistance = get<1>(tuple_node_distance);
            }
        }
    }

    return make_tuple(closestnode, closestdistance);
}
///
/// gets al the tanks that are colliding with the tank inserted
///
vector<Tank*> BattleSim::QuadTree::FindNodesInRange(const Tank tank,  vector<Tank*> in_range, const float range)
{
    // looks in all quadrants  
    for (QuadTree* quad_tree : quad_tree_quadrants)
    {
        //checks if the tree is empty && if the the closest tank radius intersects with the quadrant
        if (quad_tree->count > 0 && quad_tree->rectangle->intersectsCircle(tank.position, range))
        {
            // if it is a quad tree go in the tree other wise check the nodes 
            if (quad_tree->points.empty() && quad_tree->count == 4)
            {
                in_range = quad_tree->FindNodesInRange(tank, in_range, range);
            }
            else if (!quad_tree->points.empty())
            {
                in_range = quad_tree->CollisionCheck(tank, range, in_range);
            }
        }
    }

    return in_range;
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
            if (points[i] == node)
            {
                points.erase(points.begin() + i);
                count--;
                return true;
            }
        }
    }
    if (NWTree == nullptr)
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
