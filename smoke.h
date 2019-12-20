#pragma once
namespace BattleSim
{

class Smoke
{
  public:
    Smoke(Sprite& smoke_sprite, vec2 position) : current_frame(0), smoke_sprite(smoke_sprite), position(position) {}

    void Tick();
    void Draw(Surface* screen);

    vec2 position;

    int current_frame;
    Sprite& smoke_sprite;
};
} // namespace BattleSim