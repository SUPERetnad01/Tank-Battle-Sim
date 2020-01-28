#pragma once
namespace BattleSim
{
class QNode
{
  private:
  public:
    QNode(vec2 _pos, Tank* _tank)
    {
        tank = _tank;
        pos = _pos;
    }
    QNode()
    {
        tank = nullptr;
    }
    vec2 pos = (0, 0);
    Tank* tank = nullptr;
};
class QuadTree
{
    int capacity = 4;
    int count;
    vector<Tank*> points;
    QuadTree* NETree;
    QuadTree* NWTree;
    QuadTree* SETree;
    QuadTree* SWTree;

  public:
    vec2 topLeftBorder;
    vec2 botRightBorder;
    Rectangle2D* rectangle;
    QuadTree()
    {
        count = 0;
        topLeftBorder = (0, 0);
        botRightBorder = (0, 0);
        rectangle = nullptr;
        NETree = nullptr;
        NWTree = nullptr;
        SETree = nullptr;
        SWTree = nullptr;
    }
    QuadTree(vec2 topL, vec2 botR, QuadTree* parrent = nullptr)
    {

        count = 0;
        NETree = nullptr;
        NWTree = nullptr;
        SETree = nullptr;
        SWTree = nullptr;
        topLeftBorder = topL;
        botRightBorder = botR;
        rectangle = new Rectangle2D(botR, topL);
    }
    ~QuadTree()
    {
        delete rectangle;
    }
    bool insertNode(Tank*);
    void subdivide();
    tuple<Tank*, float> FindClosest(Tank, Tank*, float);
    vector<Tank*> FindNodesInRange(Tank, vector<Tank*>, float);
    bool inBoundary(vec2);
    bool removeNode(Tank*);
    vector<Tank*> CollisionCheck(Tank, float, vector<Tank*>);
    tuple<Tank*, float> closestDistanceinPoints(Tank, float, Tank*);
};
} // namespace BattleSim
