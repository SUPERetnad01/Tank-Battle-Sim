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
    void update_partical_beams();
    std::vector<LinkedList> bucket_sort(std::vector<Tank*>& unsortedTanks, int numberofbuckets);
    std::vector<int> counting_sort(const vector<Tank*>& in);
    void sort_health_bars();
    void draw_blue_health();
    void draw_red_health();
    void draw_health_bars(int i, char color, int health);
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

    long long frame_count = 0;
    Grid grid;
    static const int sizeOfCell = 26;
    static const int gridOffset = 15;
    bool lock_update = false;
};

}; // namespace BattleSim