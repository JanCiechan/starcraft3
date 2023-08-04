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
    int range;
    int speed;

public:
    Unit(char type, int id, int x, int y, int health, char production)
    {
        this->type = type;
        this->id = id;
        this->x = x;
        this->y = y;
        this->health = health;
        this->production = production;
        calculateRangeAndSpeed(type);
    }
    void calculateRangeAndSpeed(char type)
    { //(K/S/A/P/C/R/W)
        switch (type)
        {
        case ('K'):
            this->speed = 5;
            this->range = 1;
            break;
        case ('S'):
            this->speed = 2;
            this->range = 1;
            break;
        case ('A'):
            this->speed = 2;
            this->range = 5;
            break;
        case ('P'):
            this->speed = 2;
            this->range = 2;
            break;
        case ('C'):
            this->speed = 2;
            this->range = 7;
            break;
        case ('R'):
            this->speed = 2;
            this->range = 1;
            break;
        case ('W'):
            this->speed = 2;
            this->range = 1;
            break;
        default:
            this->speed = 2;
            this->range = 1;
        }
    }
    Unit()
    {
    }
    char getType()
    {
        return this->type;
    }
    char getProduction()
    {
        return this->production;
    }
    int getId()
    {
        return this->id;
    }
    std::array<int, 2> getCoords()
    {
        std::array<int, 2> coord;
        coord[0] = this->x;
        coord[1] = this->y;
        return coord;
    }
};