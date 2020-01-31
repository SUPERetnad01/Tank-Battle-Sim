#include "precomp.h" // include (only) this in every .cpp file
#include <CL/cl.h>

#define NUM_TANKS_BLUE 1279
#define NUM_TANKS_RED 1279
#define NUM_TANKS_TOTAL 2558
#define TANK_MAX_HEALTH 1000
#define ROCKET_HIT_VALUE 60
#define PARTICLE_BEAM_HIT_VALUE 50

#define TANK_MAX_SPEED 1.5

#define HEALTH_BARS_OFFSET_X 0
#define HEALTH_BAR_HEIGHT 70
#define HEALTH_BAR_WIDTH 1
#define HEALTH_BAR_SPACING 0

#define MAX_FRAMES 2000
#define MAX_SOURCE_SIZE (0x100000)

//Global performance timer
// dante's performance  = 74318.2
// gert performance = 73804.1
// kevin's performance = 63243.3
#define REF_PERFORMANCE 63243.3 //UPDATE THIS WITH YOUR REFERENCE PERFORMANCE (see console after 2k frames)
static timer perf_timer;
static float duration;

//Load sprite files and initialize sprites
static Surface* background_img = new Surface("assets/Background_Grass.png");
static Surface* tank_red_img = new Surface("assets/Tank_Proj2.png");
static Surface* tank_blue_img = new Surface("assets/Tank_Blue_Proj2.png");
static Surface* rocket_red_img = new Surface("assets/Rocket_Proj2.png");
static Surface* rocket_blue_img = new Surface("assets/Rocket_Blue_Proj2.png");
static Surface* particle_beam_img = new Surface("assets/Particle_Beam.png");
static Surface* smoke_img = new Surface("assets/Smoke.png");
static Surface* explosion_img = new Surface("assets/Explosion.png");

static Sprite background(background_img, 1);
static Sprite tank_red(tank_red_img, 12);
static Sprite tank_blue(tank_blue_img, 12);
static Sprite rocket_red(rocket_red_img, 12);
static Sprite rocket_blue(rocket_blue_img, 12);
static Sprite smoke(smoke_img, 4);
static Sprite explosion(explosion_img, 9);
static Sprite particle_beam_sprite(particle_beam_img, 3);

const static vec2 tank_size(14, 18);
const static vec2 rocket_size(25, 24);

//Bucket sort Vars
//vector<LinkedList> redHealthBars = {};
//vector<LinkedList> blueHealthBars = {};

vector<int> red_health_bars = {};
vector<int> blue_health_bars = {};
vec2 nullpoint = {0, -2000};
vec2 Maxborder = {SCRWIDTH, SCRHEIGHT};
QuadTree* redTanksQTree = new QuadTree(Maxborder, nullpoint);
;
QuadTree* blueTanksQTree = new QuadTree(Maxborder, nullpoint);
QuadTree* allTanksQTree = new QuadTree(Maxborder, nullpoint);

const static float tank_radius = 12.f;
const static float rocket_radius = 10.f;

mutex mtx;

// -----------------------------------------------------------
// Initialize the application
void Game::Init()
{
    ////////
    // Create the two input vectors
    int i;
    const int LIST_SIZE = 1024;
    int* A = (int*)malloc(sizeof(int) * LIST_SIZE);
    int* B = (int*)malloc(sizeof(int) * LIST_SIZE);
    for (i = 0; i < LIST_SIZE; i++)
    {
        A[i] = i;
        B[i] = LIST_SIZE - i;
    }

    // Load the kernel source code into the array source_str
    FILE* fp;
    char* source_str;
    size_t source_size;

    fp = fopen("kernel.cl", "r");
    if (!fp)
    {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1,
                         &device_id, &ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create memory buffers on the device for each vector
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                      LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                      LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                      LIST_SIZE * sizeof(int), NULL, &ret);

    // Copy the lists A and B to their respective memory buffers
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
                               LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
                               LIST_SIZE * sizeof(int), B, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1,
                                                   (const char**)&source_str, (const size_t*)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&b_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&c_mem_obj);

    // Execute the OpenCL kernel on the list
    size_t global_item_size = LIST_SIZE; // Process the entire lists
    size_t local_item_size = 64;         // Divide work items into groups of 64
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
                                 &global_item_size, &local_item_size, 0, NULL, NULL);

    // Read the memory buffer C on the device to the local variable C
    int* C = (int*)malloc(sizeof(int) * LIST_SIZE);
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
                              LIST_SIZE * sizeof(int), C, 0, NULL, NULL);

    // Display the result to the screen
    for (i = 0; i < LIST_SIZE; i++)
        // printf("%d + %d = %d\n", A[i], B[i], C[i]);

        // Clean up
        ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(A);
    free(B);
    free(C);

    ///////
    frame_count_font = new Font("assets/digital_small.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZ:?!=-0123456789.");

    memset(Xgrid, 69420.0f, sizeof(Xgrid));
    memset(Ygrid, 69420.0f, sizeof(Xgrid));
    tanks.reserve(NUM_TANKS_BLUE + NUM_TANKS_RED);
    blueTanks.reserve(NUM_TANKS_BLUE);
    redTanks.reserve(NUM_TANKS_RED);

    uint rows = (uint)sqrt(NUM_TANKS_BLUE + NUM_TANKS_RED);
    uint max_rows = 12;

    float start_blue_x = tank_size.x + 10.0f;
    float start_blue_y = tank_size.y + 80.0f;

    float start_red_x = 980.0f;
    float start_red_y = 100.0f;

    float spacing = 15.0f;

    //Spawn blue tanks
    for (int i = 0; i < NUM_TANKS_BLUE; i++)
    {
        tanks.push_back(Tank(&grid, start_blue_x + ((i % max_rows) * spacing), start_blue_y + ((i / max_rows) * spacing), BLUE, &tank_blue, &smoke, 1200, 600, tank_radius, TANK_MAX_HEALTH, TANK_MAX_SPEED));
    }
    //Spawn red tanks
    for (int i = 0; i < NUM_TANKS_RED; i++)
    {
        tanks.push_back(Tank(&grid, start_red_x + ((i % max_rows) * spacing), start_red_y + ((i / max_rows) * spacing), RED, &tank_red, &smoke, 80, 80, tank_radius, TANK_MAX_HEALTH, TANK_MAX_SPEED));
    }
    for (Tank& tank : tanks)
    {
        tank.grid->addTank2Cell(&tank);
        QNode* x = new QNode(tank.position, &tank);
        allTanksQTree->insertNode(&tank);
        if (tank.allignment == RED)
        {
            redTanks.emplace_back(&tank);
        }
        else
        {
            blueTanks.emplace_back(&tank);
        }
    }
    particle_beams.push_back(Particle_beam(vec2(SCRWIDTH / 2, SCRHEIGHT / 2), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
    particle_beams.push_back(Particle_beam(vec2(80, 80), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
    particle_beams.push_back(Particle_beam(vec2(1200, 600), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{
}

Game::~Game()
{
    delete frame_count_font;
}

// -----------------------------------------------------------
// Iterates through all tanks and returns the closest enemy tank for the given tank
// -----------------------------------------------------------
Tank& Game::FindClosestEnemy(Tank& current_tank)
{
    auto result = allTanksQTree->FindClosest(current_tank, nullptr, numeric_limits<float>::infinity());
    auto closesttank = get<0>(result);
    return *closesttank;
}

// -----------------------------------------------------------
// Update the game state:
// Move all objects
// Update sprite frames
// Collision detection
// Targeting etc..
// -----------------------------------------------------------
void Game::Update(float deltaTime)
{
    //Update tanks
    UpdateTanks();

    //Update smoke plumes
    //for (Smoke& _smoke : smokes)
    tbb::parallel_for(tbb::blocked_range<int>(0, smokes.size()), [&](tbb::blocked_range<int> r) {
        for (auto i = r.begin(); i < r.end(); i++)
        {
            smokes[i].Tick();
        }
    });

    //Update rockets
    UpdateRockets();

    //Remove exploded rockets with remove erase idiom
    rockets.erase(std::remove_if(rockets.begin(), rockets.end(), [](const Rocket& rocket) { return !rocket.active; }), rockets.end());

    //Update particle beams
    UpdateParticalBeams();

    //Update explosion sprites and remove when done with remove erase idiom
    //for (Explosion& _explosion : explosions)
    tbb::parallel_for(tbb::blocked_range<int>(0, explosions.size()), [&](tbb::blocked_range<int> r) {
        for (auto i = r.begin(); i < r.end(); i++)
        {
            explosions[i].Tick();
        }
    });

    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& _explosion) { return _explosion.done(); }), explosions.end());
    SortHealthBars();
}

void Game::Draw()
{
    // clear the graphics window
    screen->Clear(0);

    //Draw background
    background.Draw(screen, 0, 0);

    //Draw sprites

    //*Draw Tanks*
    //for (int i = 0; i < NUM_TANKS_BLUE + NUM_TANKS_RED; i++)
    tbb::parallel_for(tbb::blocked_range<int>(0, NUM_TANKS_BLUE + NUM_TANKS_RED), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            tanks.at(i).Draw(screen);

            vec2 tPos = tanks.at(i).Get_Position();
            // tread marks
            if ((tPos.x >= 0) && (tPos.x < SCRWIDTH) && (tPos.y >= 0) && (tPos.y < SCRHEIGHT))
                background.GetBuffer()[(int)tPos.x + (int)tPos.y * SCRWIDTH] = SubBlend(
                    background.GetBuffer()[(int)tPos.x + (int)tPos.y * SCRWIDTH], 0x808080);
        }
    });

    //*Draw Rockets*
    //for (Rocket& rocket : rockets)
    tbb::parallel_for(tbb::blocked_range<int>(0, rockets.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            rockets[i].Draw(screen);
        }
    });

    //*Draw Smoke*
    //for (Smoke& _smoke : smokes)
    tbb::parallel_for(tbb::blocked_range<int>(0, smokes.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            smokes[i].Draw(screen);
        }
    });

    //*Draw Particle_beam*
    //for (Particle_beam& particle_beam : particle_beams)
    tbb::parallel_for(tbb::blocked_range<int>(0, particle_beams.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            particle_beams[i].Draw(screen);
        }
    });

    //*Draw Explosion*
    //for (Explosion& _explosion : explosions)
    tbb::parallel_for(tbb::blocked_range<int>(0, explosions.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            explosions[i].Draw(screen);
        }
    });

    //Draw sorted health bars
    DrawBlueHealth();
    DrawRedHealth();
}

// -----------------------------------------------------------
// When we reach MAX_FRAMES print the duration and speedup multiplier
// Updating REF_PERFORMANCE at the top of this file with the value
// on your machine gives you an idea of the speedup your optimizations give
// -----------------------------------------------------------
void BattleSim::Game::MeasurePerformance()
{
    char buffer[128];
    if (frame_count >= MAX_FRAMES)
    {
        if (!lock_update)
        {
            duration = perf_timer.elapsed();
            cout << "Duration was: " << duration << " (Replace REF_PERFORMANCE with this value)" << endl;
            lock_update = true;
        }

        frame_count--;
    }

    if (lock_update)
    {
        screen->Bar(420, 170, 870, 430, 0x030000);
        int ms = (int)duration % 1000, sec = ((int)duration / 1000) % 60, min = ((int)duration / 60000);
        sprintf(buffer, "%02i:%02i:%03i", min, sec, ms);
        frame_count_font->Centre(screen, buffer, 200);
        sprintf(buffer, "SPEEDUP: %4.1f", REF_PERFORMANCE / duration);
        frame_count_font->Centre(screen, buffer, 340);
    }
}

void BattleSim::Game::UpdateTanks()
{
    for (Tank& tank : tanks)
    {
        if (tank.active)
        {
            //QuadTreebasedColision
            GPGPU(&tank);
            /*  vector<Tank*> allNodesInRange;
            allNodesInRange = allTanksQTree->FindNodesInRange(tank, allNodesInRange, tank.collision_radius);
            if (allNodesInRange.size() > 0)
            {
                for (Tank* node : allNodesInRange)
                {
                    vec2 dir = tank.Get_Position() - node->Get_Position();
                    tank.Push(dir.normalized(), 1.0f);
                }
            } */
            tank.grid->handleTankCell(tank.CellX, tank.CellY, &tank);
            allTanksQTree->removeNode(&tank);
            tank.Tick();
            allTanksQTree->insertNode(&tank);
            if (tank.Rocket_Reloaded())
            {
                Tank& target = FindClosestEnemy(tank);
                rockets.push_back(
                    Rocket(&grid, tank.position, (target.Get_Position() - tank.position).normalized() * 3, rocket_radius,
                           tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));
                tank.Reload_Rocket();
            }
        }
    }
}

void BattleSim::Game::UpdateRockets()

{
    for (Rocket& rocket : rockets)
    {
        rocket.Tick();
        if (rocket.hitTarget)
        {
            Tank* tank = rocket.hitTank;
            explosions.push_back(Explosion(&explosion, tank->position));
            if (tank->hit(ROCKET_HIT_VALUE))
            {
                smokes.push_back(Smoke(smoke, tank->position - vec2(0, 48)));
            }
            rocket.active = false;
        }

        //Check if rocket collides with enemy tank, spawn explosion and if tank is destroyed spawn a smoke plume
    }
}

void BattleSim::Game::UpdateParticalBeams()
{
    tbb::parallel_for(tbb::blocked_range<int>(0, particle_beams.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            Particle_beam& particle_beam = particle_beams[i];
            particle_beam.tick(tanks);

            //Damage all tanks within the damage window of the beam (the window is an axis-aligned bounding box)
            for (Tank& tank : tanks)
            {
                if (tank.active &&
                    particle_beam.rectangle.intersectsCircle(tank.Get_Position(),
                                                             tank.Get_collision_radius()))
                {
                    if (tank.hit(particle_beam.damage))
                    {
                        smokes.push_back(Smoke(smoke, tank.position - vec2(0, 48)));
                    }
                }
            }
        }
    });
}

/*
vector<LinkedList> BattleSim::Game::BucketSort(vector<Tank*>& unsortedtanks, int numberofbuckets)
{
    vector<LinkedList> buckets(numberofbuckets);
    for (auto& tank : unsortedtanks)
    {
        buckets.at(tank->health > 0 ? tank->health / numberofbuckets : 0).InsertValue(tank->health > 0 ? tank->health : 0);
    }
    return buckets;
}
*/

std::vector<int> BattleSim::Game::CountSort(const vector<Tank*>& in)
{
    std::vector<int> Counters(TANK_MAX_HEALTH + 1, 0);
    std::vector<int> Results;

    for (auto x : in)
        Counters.at(x->health <= 0 ? 0 : x->health)++;

    for (int i = 0; i < TANK_MAX_HEALTH + 1; ++i)
        if (Counters[i] != 0)
            for (int y = 0; y < Counters[i]; ++y)
                Results.push_back(i);

    return Results;
}

void BattleSim::Game::SortHealthBars()
{
    //redHealthBars = BucketSort(redTanks, 100);
    //blueHealthBars = BucketSort(blueTanks, 100);

    red_health_bars = CountSort(redTanks);
    blue_health_bars = CountSort(blueTanks);
}

void BattleSim::Game::DrawBlueHealth()
{
    int count_blue = 0;
    for (int currentBlueTank : blue_health_bars)
    {
        DrawHealthBars(count_blue, 'b', currentBlueTank);
        count_blue++;
    }

    //Bucketsort draw health Blue Tanks
    /*
    for (auto& bucket : blue_health_bars)
    {
        LinkedListnode* currentBlueTank = bucket.head;
        while (currentBlueTank != nullptr)
        {
            DrawHealthBars(count_blue, 'b', currentBlueTank->data);
            currentBlueTank = currentBlueTank->next;
            count_blue++;
        }
    }
    */
}

void BattleSim::Game::DrawRedHealth()
{
    int count_red = 0;
    for (int currentRedTank : red_health_bars)
    {
        DrawHealthBars(count_red, 'r', currentRedTank);
        count_red++;
    }
    //Bucketsort draw health Red Tanks
    /*
    for (auto& bucket : red_health_bars)
    {
        LinkedListnode* currentRedTank = bucket.head;
        while (currentRedTank != nullptr)
        {
            DrawHealthBars(count_red, 'r', currentRedTank->data);
            currentRedTank = currentRedTank->next;
            count_red++;
        }
    }*/
}

void BattleSim::Game::DrawHealthBars(int i, char color, int health)
{
    int health_bar_start_x = i * (HEALTH_BAR_WIDTH + HEALTH_BAR_SPACING) + HEALTH_BARS_OFFSET_X;
    int health_bar_start_y = (color == 'b') ? 0 : (SCRHEIGHT - HEALTH_BAR_HEIGHT) - 1;
    int health_bar_end_x = health_bar_start_x + HEALTH_BAR_WIDTH;
    int health_bar_end_y = (color == 'b') ? HEALTH_BAR_HEIGHT : SCRHEIGHT - 1;

    screen->Bar(health_bar_start_x, health_bar_start_y, health_bar_end_x, health_bar_end_y, REDMASK);
    screen->Bar(health_bar_start_x, health_bar_start_y + (int)((double)HEALTH_BAR_HEIGHT * (1 - ((double)health / (double)TANK_MAX_HEALTH))),
                health_bar_end_x, health_bar_end_y, GREENMASK);
}

void BattleSim::Game::GPGPU(Tank* tank)
{
    int cellpos = tank->CellY * maximumUnitsInCell * tank->CellX - maximumUnitsInCell;
    maximumUnitsInCell;
    for (int i = 0; i < maximumUnitsInCell; i++)
    {
        Xgrid[cellpos] = tank->position.x;
        Ygrid[cellpos] = tank->position.y;
    }
}

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick(float deltaTime)
{
    if (!lock_update)
    {
        Update(deltaTime);
    }
    Draw();

    MeasurePerformance();

    //Print frame count
    frame_count++;
    string frame_count_string = "FRAME: " + std::to_string(frame_count);
    frame_count_font->Print(screen, frame_count_string.c_str(), 350, 580);
}
