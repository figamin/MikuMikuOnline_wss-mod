#pragma once

// The base class for all characters.
// Includes the player, other players, and NPCs.

class Character
{
public:
    virtual ~Character() {}

    virtual void Draw() const = 0;
    virtual void Update() = 0;
};
