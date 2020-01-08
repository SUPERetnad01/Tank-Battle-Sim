#pragma once

namespace BattleSim
{

class Rocket
{
    friend class Grid;
  public:
    Rocket(Grid *grid,vec2 position, vec2 direction, float collision_radius, allignments allignment, Sprite* rocket_sprite);
    ~Rocket();

    void Tick();
    void Draw(Surface* screen);
    void setHitTank(Tank* tank);
    bool Intersects(vec2 position_other, float radius_other) const;
    void Moverocket();
    vec2 position;
    vec2 speed;
    vec2 hitTankPos;
    Tank* hitTank;
    float collision_radius;
    bool hitTarget = false;
    bool active;

    allignments allignment;

    int current_frame;
    Sprite* rocket_sprite;

  private:
    Grid* grid;
};

} // namespace BattleSim