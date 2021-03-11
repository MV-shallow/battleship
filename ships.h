#ifndef SHIPS_H
#define SHIPS_H

#include <qvector.h>

class cell;
class player;

enum result {shipDamaged, shipDestroyed};

class ship
{
protected:
    int health;
    int size;
    QVector <cell*> pointer;
    ship (int h) : health(h), size(h), pointer(h)
    {

    }
public:
    void connectCell(cell *C, int ind)
    {
        pointer[ind] = C;
    }
    int getHealth()
    {
        return health;
    }
    cell* getShipPart(int ind)
    {
        return pointer[ind];
    }
    int getSize()
    {
        return size;
    }
    QVector <cell*>* getAllShipParts()
    {
        return &pointer;
    }
    result decHealth()
    {
        health--;
        if (!health)
        {
            return shipDestroyed;
        }
        else
        {
            return shipDamaged;
        }
    }
    virtual ~ship() = default;
};

class ship_1x1 : public ship
{
public:
    ship_1x1( ) : ship(1)
    {

    }

};
class ship_1x2 : public ship
{

public:
    ship_1x2() : ship(2)
    {

    }
};
class ship_1x3 : public ship
{
public:
    ship_1x3() : ship(3)
    {

    }
};
class ship_1x4 : public ship
{
public:
    ship_1x4() : ship(4)
    {

    }

};
#endif // SHIPS_H

