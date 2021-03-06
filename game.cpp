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
vec2 bottom_right_border = {0, -2000};
vec2 top_left_border = {SCRWIDTH, SCRHEIGHT};
QuadTree* redTanksQTree = new QuadTree(top_left_border, bottom_right_border);
QuadTree* blueTanksQTree = new QuadTree(top_left_border, bottom_right_border);
QuadTree* allTanksQTree = new QuadTree(top_left_border, bottom_right_border);

const static float tank_radius = 12.f;
const static float rocket_radius = 10.f;

mutex mtx;

// -----------------------------------------------------------
// Initialize the application
void Game::Init()
{
    frame_count_font = new Font("assets/digital_small.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZ:?!=-0123456789.");

    tanks.reserve(NUM_TANKS_BLUE + NUM_TANKS_RED);
    blueTanks.reserve(NUM_TANKS_BLUE);
    redTanks.reserve(NUM_TANKS_RED);

    uint rows = sqrt(NUM_TANKS_BLUE + NUM_TANKS_RED);
    const uint max_rows = 12;

    const float start_blue_x = tank_size.x + 10.0f;
    const float start_blue_y = tank_size.y + 80.0f;

    const float start_red_x = 980.0f;
    const float start_red_y = 100.0f;

    const float spacing = 15.0f;

    //Spawn blue tanks
    for (int i = 0; i < NUM_TANKS_BLUE; i++)
    {
        tanks.emplace_back(Tank(&grid, start_blue_x + ((i % max_rows) * spacing), start_blue_y + ((i / max_rows) * spacing), BLUE, &tank_blue, &smoke, 1200, 600, tank_radius, TANK_MAX_HEALTH, TANK_MAX_SPEED));
    }
    //Spawn red tanks
    for (int i = 0; i < NUM_TANKS_RED; i++)
    {
        tanks.emplace_back(Tank(&grid, start_red_x + ((i % max_rows) * spacing), start_red_y + ((i / max_rows) * spacing), RED, &tank_red, &smoke, 80, 80, tank_radius, TANK_MAX_HEALTH, TANK_MAX_SPEED));
    }
    for (Tank& tank : tanks)
    {
        tank.grid->addTank2Cell(&tank);
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
    particle_beams.emplace_back(Particle_beam(vec2(SCRWIDTH / 2, SCRHEIGHT / 2), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
    particle_beams.emplace_back(Particle_beam(vec2(80, 80), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
    particle_beams.emplace_back(Particle_beam(vec2(1200, 600), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
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
const Tank& Game::find_closest_enemy(Tank& current_tank)
{
    auto result = allTanksQTree->FindClosest(current_tank, nullptr, numeric_limits<float>::infinity());
    const auto closest_tank = get<0>(result);
    return *closest_tank;
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
    update_particle_beams();

    //Update explosion sprites and remove when done with remove erase idiom
    //for (Explosion& _explosion : explosions)
    tbb::parallel_for(tbb::blocked_range<int>(0, explosions.size()), [&](tbb::blocked_range<int> r) {
        for (auto i = r.begin(); i < r.end(); i++)
        {
            explosions[i].Tick();
        }
    });

    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& _explosion) { return _explosion.done(); }), explosions.end());
    sort_health_bars();
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
            scoped_lock(mtx);
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
            scoped_lock(mtx);
            rockets[i].Draw(screen);
        }
    });

    //*Draw Smoke*
    //for (Smoke& _smoke : smokes)
    tbb::parallel_for(tbb::blocked_range<int>(0, smokes.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            scoped_lock(mtx);
            smokes[i].Draw(screen);
        }
    });

    //*Draw Particle_beam*
    //for (Particle_beam& particle_beam : particle_beams)
    tbb::parallel_for(tbb::blocked_range<int>(0, particle_beams.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            scoped_lock(mtx);
            particle_beams[i].Draw(screen);
        }
    });

    //*Draw Explosion*
    //for (Explosion& _explosion : explosions)
    tbb::parallel_for(tbb::blocked_range<int>(0, explosions.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            scoped_lock(mtx);
            explosions[i].Draw(screen);
        }
    });

    //Draw sorted health bars
    draw_blue_health();
    draw_red_health();
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
            //QuadTree based Collision
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
                const Tank& target = find_closest_enemy(tank);
                rockets.emplace_back(
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
            explosions.emplace_back(Explosion(&explosion, tank->position));
            if (tank->hit(ROCKET_HIT_VALUE))
            {
                smokes.emplace_back(Smoke(smoke, tank->position - vec2(0, 48)));
            }
            rocket.active = false;
        }

        //Check if rocket collides with enemy tank, spawn explosion and if tank is destroyed spawn a smoke plume
    }
}

void BattleSim::Game::update_particle_beams()
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
                        smokes.emplace_back(Smoke(smoke, tank.position - vec2(0, 48)));
                    }
                }
            }
        }
    });
}

/* Bucket sort
vector<LinkedList> BattleSim::Game::bucket_sort(vector<Tank*>& unsortedtanks, int numberofbuckets)
{
    vector<LinkedList> buckets(numberofbuckets);
    for (auto& tank : unsortedtanks)
    {
        buckets.at(tank->health > 0 ? tank->health / numberofbuckets : 0).InsertValue(tank->health > 0 ? tank->health : 0);
    }
    return buckets;
}
*/

std::vector<int> BattleSim::Game::counting_sort(const vector<Tank*>& in)
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

void BattleSim::Game::sort_health_bars()
{
    //redHealthBars = bucket_sort(redTanks, 100);
    //blueHealthBars = bucket_sort(blueTanks, 100);

    red_health_bars = counting_sort(redTanks);
    blue_health_bars = counting_sort(blueTanks);
}

void BattleSim::Game::draw_blue_health()
{
    int count_blue = 0;
    for (int currentBlueTank : blue_health_bars)
    {
        draw_health_bars(count_blue, 'b', currentBlueTank);
        count_blue++;
    }

    //Bucketsort draw health Blue Tanks
    /*
    for (auto& bucket : blue_health_bars)
    {
        LinkedListnode* currentBlueTank = bucket.head;
        while (currentBlueTank != nullptr)
        {
            draw_health_bars(count_blue, 'b', currentBlueTank->data);
            currentBlueTank = currentBlueTank->next;
            count_blue++;
        }
    }
    */
}

void BattleSim::Game::draw_red_health()
{
    int count_red = 0;
    for (int currentRedTank : red_health_bars)
    {
        draw_health_bars(count_red, 'r', currentRedTank);
        count_red++;
    }
    //Bucketsort draw health Red Tanks
    /*
    for (auto& bucket : red_health_bars)
    {
        LinkedListnode* currentRedTank = bucket.head;
        while (currentRedTank != nullptr)
        {
            draw_health_bars(count_red, 'r', currentRedTank->data);
            currentRedTank = currentRedTank->next;
            count_red++;
        }
    }*/
}

void BattleSim::Game::draw_health_bars(int i, char color, int health)
{
   const int health_bar_start_x = i * (HEALTH_BAR_WIDTH + HEALTH_BAR_SPACING) + HEALTH_BARS_OFFSET_X;
   const int health_bar_start_y = (color == 'b') ? 0 : (SCRHEIGHT - HEALTH_BAR_HEIGHT) - 1;
   const int health_bar_end_x = health_bar_start_x + HEALTH_BAR_WIDTH;
   const int health_bar_end_y = (color == 'b') ? HEALTH_BAR_HEIGHT : SCRHEIGHT - 1;

    screen->Bar(health_bar_start_x, health_bar_start_y, health_bar_end_x, health_bar_end_y, REDMASK);
    screen->Bar(health_bar_start_x, health_bar_start_y + (int)((double)HEALTH_BAR_HEIGHT * (1 - ((double)health / (double)TANK_MAX_HEALTH))),
                health_bar_end_x, health_bar_end_y, GREENMASK);
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
    const string frame_count_string = "FRAME: " + std::to_string(frame_count);
    frame_count_font->Print(screen, frame_count_string.c_str(), 350, 580);
}
