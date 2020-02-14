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
    void GPGPU();
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
    cl_int ret;
    cl_command_queue command_queue;
    cl_context context;
    cl_kernel kernel;
    cl_kernel movekernel;
    cl_program program;
    cl_mem tankGrid_mem_obj;
    cl_mem ygrid_mem_obj;
    cl_mem xtank_mem_obj;
    cl_mem ytank_mem_obj;
    cl_mem tank_force_x_mem_obj;
    cl_mem tank_force_y_mem_obj;

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

    static const int totalTanks = 1280; //2558;
    static const int numberOfCells = 250;
    static const int maximumUnitsInCell = 200;
    int gridarrys = numberOfCells * numberOfCells * maximumUnitsInCell;
    int tankGrid[numberOfCells * numberOfCells * maximumUnitsInCell];
    float xTank[totalTanks];
    float yTank[totalTanks];
    float xForce[totalTanks];
    float yForce[totalTanks];
    char tankcolour[totalTanks];
    long long frame_count = 0;
    Grid grid;
    static const int sizeOfCell = 26;
    static const int gridOffset = 15;
    bool lock_update = false;
};

}; // namespace BattleSim