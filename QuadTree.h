#pragma once
namespace BattleSim
{
class QuadTree
{
    int capacity = 4;
    int count;
    vector<Tank*> points;
    vector<QuadTree*> quad_tree_quadrants;
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
    QuadTree(vec2 topL, vec2 botR)
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
