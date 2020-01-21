#include "precomp.h"
#include "QuadTree.h"

bool BattleSim::QuadTree::insertNode(QNode *node)
{
    if (&node == nullptr) return false;
    if (!inBoundary(node->pos)) return false;
    if (points.size() < this->capacity && NWTree == NULL) {
        points.push_back(node);
        count++;

        this->points;
        return true;
  
   }
    this->points;
    if (NWTree == NULL) {
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
                (topLeftBorder.y + botRightBorder.y) / 2),this);

    NETree = new QuadTree(
        vec2(topLeftBorder.x, (topLeftBorder.y + botRightBorder.y) / 2),
        vec2((topLeftBorder.x + botRightBorder.x) / 2, botRightBorder.y),this);

    SWTree = new QuadTree(
        vec2((topLeftBorder.x + botRightBorder.x) / 2, topLeftBorder.y),
        vec2( botRightBorder.x, (topLeftBorder.y + botRightBorder.y) / 2),this);

    SETree = new QuadTree(
        vec2((topLeftBorder.x + botRightBorder.x) / 2, (topLeftBorder.y + botRightBorder.y) / 2),
        vec2( botRightBorder.x, botRightBorder.y),this);
    for (auto node : points) {
        this->NETree->insertNode(node);
        this->NWTree->insertNode(node);
        this->SETree->insertNode(node); 
        this->SWTree->insertNode(node);
        
    }
    points.clear();

}

tuple<QNode*,float> BattleSim::QuadTree::FindClosest(vec2 tankpos, QNode* closestnode = nullptr, float closestdistance = numeric_limits<float>::infinity())
{
    if(this->NETree->count > 0 && this->NETree->rectangle->intersectsCircle(tankpos, closestdistance)) {
        if (this->NETree->points.size() == 0 && this->NETree->count==4) {
           auto result = this->NETree->FindClosest(tankpos, closestnode, closestdistance);
            closestnode = get<0>(result);
            closestdistance = get<1>(result);
        }
        else if (this->NETree->points.size() > 0)
        {
            auto tupleNodeDistantce = this->NETree->closestDistanceinPoints(tankpos, closestdistance, closestnode);
            closestnode = get<0>(tupleNodeDistantce);
            closestdistance = get<1>(tupleNodeDistantce);
            
        }
    }  
    if (this->NWTree->count >0 && this->NWTree->rectangle->intersectsCircle(tankpos, closestdistance))
    {
        if (this->NWTree->points.size() == 0 && this->NWTree->count==4)
        {
            auto result = this->NWTree->FindClosest(tankpos, closestnode, closestdistance);
            closestnode = get<0>(result);
            closestdistance = get<1>(result);
        }
        else if (this->NWTree->points.size() > 0)
        {
            auto tupleNodeDistantce = this->NWTree->closestDistanceinPoints(tankpos, closestdistance, closestnode);
            closestnode = get<0>(tupleNodeDistantce);
            closestdistance = get<1>(tupleNodeDistantce);
        }
    }   
    if (this->SETree->count >0 && this->SETree->rectangle->intersectsCircle(tankpos, closestdistance))
    {
        if (this->SETree->points.size() == 0 && this->SETree->count == 4)
        {
            auto result  = this->SETree->FindClosest(tankpos, closestnode, closestdistance);
            closestnode = get<0>(result);
            closestdistance = get<1>(result);
        }
        else if (this->SETree->points.size()>0)
        {
            auto tupleNodeDistantce = this->SETree->closestDistanceinPoints(tankpos, closestdistance, closestnode);
            closestnode = get<0>(tupleNodeDistantce);
            closestdistance = get<1>(tupleNodeDistantce);
        }
    }   
    if (this->SWTree->count >0 && this->SWTree->rectangle->intersectsCircle(tankpos, closestdistance))
    {
        if (this->SWTree->points.size() == 0 && this->SWTree->count == 4)
        {
            auto result = this->SWTree->FindClosest(tankpos, closestnode, closestdistance);
            closestnode = get<0>(result);
            closestdistance = get<1>(result);
        }
        else if (this->SWTree->points.size()>0)
        {
            auto tupleNodeDistantce = this->SWTree->closestDistanceinPoints(tankpos, closestdistance, closestnode);
            closestnode = get<0>(tupleNodeDistantce);
            closestdistance = get<1>(tupleNodeDistantce);
        }
    }
    return make_tuple(closestnode, closestdistance);
}

bool BattleSim::QuadTree::inBoundary(vec2 point)
{
   return(point.x <= topLeftBorder.x &&
        point.x >= botRightBorder.x &&
        point.y <= topLeftBorder.y &&
        point.y >= botRightBorder.y);
}

void BattleSim::QuadTree::updateTank(Tank *tank)
{
    QNode *deletednode = new QNode(tank->Get_Position(),tank);
    this->insertNode(this->removeNode(deletednode));
}

QNode* BattleSim::QuadTree::removeNode(QNode* node)
{
    if(&node == nullptr) return node;
    if (!inBoundary(node->pos)) return node;
    if (points.size() > 0)
    {
        for (int i = 0; i < points.size(); i++)
        {
            if (points[i] == node) {
                QNode* _node = points[i];
                points.erase(points.begin() + i);
                return _node;
            }
        }
    }
    if (NWTree == NULL)
    {
        return node;
    }
    if (NWTree->removeNode(node)) return node;
    if (NETree->removeNode(node)) return node;
    if (SWTree->removeNode(node)) return node;
    if (SETree->removeNode(node)) return node;

    return node;
}

tuple<QNode*, float> BattleSim::QuadTree::closestDistanceinPoints(vec2 tankpos, float closestdistance, QNode* closestNode)
{
    for (int i = 0; i < points.size(); i++)
    {
        float distance = (tankpos - points[i]->pos).length(); //sqrt((tankpos.x - points[i]->pos.x) + sqrtf(tankpos.y - points[i]->pos.y));
        if (distance < closestdistance)
        {
            closestNode = points[i];
            closestdistance = distance;
        }
    }
    return make_tuple(closestNode,closestdistance);
}

