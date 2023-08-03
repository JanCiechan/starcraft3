#include <iostream>
#include <array>
class Unit
{
private:
    char type;
    int id;
    int x;
    int y;
    int health;
    char production;

public:
    Unit(char type, int id, int x, int y, int health, char production)
    {
        this->type = type;
        this->id = id;
        this->x = x;
        this->y = y;
        this->health = health;
        this->production = production;
    }
    Unit()
    {
    }
    std::array<int, 2> getCoords()
    {
        std::array<int, 2> coord;
        coord[0] = this->x;
        coord[1] = this->y;
        return coord;
    }
};