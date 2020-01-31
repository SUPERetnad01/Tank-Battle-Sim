#pragma once

namespace BattleSim
{
//forward declarations
class Tank;
class Rocket;
class Smoke;
class Particle_beam;
class Grid;
class QuadTree;
class QNode;

class Game
{

  public:
    float* posistionX = (float*)malloc(sizeof(float));
    float* posistionY = (float*)malloc(sizeof(float));
    typedef struct tankposistion tankposistions[50];
    ~Game();
    void SetTarget(Surface* surface) { screen = surface; }
    void Init();
    void Shutdown();
    void Update(float deltaTime);
    void Draw();
    void Tick(float deltaTime);
    void MeasurePerformance();
    void UpdateTanks();
    void UpdateRockets();
    void UpdateParticalBeams();
    std::vector<LinkedList> BucketSort(std::vector<Tank*>& unsortedTanks, int numberofbuckets);
    std::vector<int> CountSort(const vector<Tank*>& in);
    void SortHealthBars();
    void DrawBlueHealth();
    void DrawRedHealth();
    void GPGPU(Tank*);
    void DrawHealthBars(int i, char color, int health);
    Tank& FindClosestEnemy(Tank& current_tank);

    void MouseUp(int button)
    { /* implement if you want to detect mouse button presses */
    }

    void MouseDown(int button)
    { /* implement if you want to detect mouse button presses */
    }

    void MouseMove(int x, int y)
    { /* implement if you want to detect mouse movement */
    }

    void KeyUp(int key)
    { /* implement if you want to handle keys */
    }

    void KeyDown(int key)
    { /* implement if you want to handle keys */
    }

  private:
    Surface* screen;
    vec2 topright = (SCRWIDTH, SCRHEIGHT);
    vector<Tank> tanks;
    vector<Rocket> rockets;
    vector<Smoke> smokes;
    vector<Explosion> explosions;
    vector<Particle_beam> particle_beams;
    vector<Tank*> blueTanks;
    vector<Tank*> redTanks;
    Font* frame_count_font;

    float* GridCellY[1000];
    static const int numberOfCells = 250;
    static const int maximumUnitsInCell = 40;
    static const int demensions = 2;
    int gridarrys = numberOfCells * numberOfCells * maximumUnitsInCell * demensions;
    float Xgrid[numberOfCells * numberOfCells * maximumUnitsInCell * demensions];
    float Ygrid[numberOfCells * numberOfCells * maximumUnitsInCell * demensions];
    long long frame_count = 0;
    Grid grid;
    static const int sizeOfCell = 26;
    static const int gridOffset = 15;
    bool lock_update = false;
};

}; // namespace BattleSim