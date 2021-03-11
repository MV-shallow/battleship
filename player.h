
#ifndef PLAYER_H
#define PLAYER_H

#include<shipCollection.h>

class player {
protected:
    shipCollection* fleet;
    int shipCount;
    QPoint basePoint;
    field* F;
    field* fieldForDisplay;
public:
    QPoint getBase()
    {
        return basePoint;
    }
    field* getField()
    {
        return F;
    }
    field* getFieldForDisplay()
    {
        return fieldForDisplay;
    }
    void setFleet(shipCollection* Fl)
    {
        fleet = Fl;
    }
    shipCollection* getFleet()
    {
        return fleet;
    }
    void incShipCount()
    {
        shipCount++;
    }
    void decShipCount()
    {
        shipCount--;
    }
    int getShipCount()
    {
        return shipCount;
    }
    virtual ~player() = default;
};
class user: public player
{
public:
    user(int x, int y) : player()
    {
        shipCount = 0;
        F = new field(emptyField);
        fieldForDisplay = new field(closedField);
        basePoint.setX(x);
        basePoint.setY(y);
        fleet = new shipCollection();
    }
};

class bot : public player
{
public:
   bot(int x, int y) : player()
   {
       shipCount = 0;
       F = new field(emptyField);
       fieldForDisplay = new field(closedField);
       basePoint.setX(x);
       basePoint.setY(y);
       fleet = new shipCollection();
   }
};

#endif // PLAYER_H

