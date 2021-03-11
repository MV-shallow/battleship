#ifndef SHIPCOLLECTION_H
#define SHIPCOLLECTION_H
#include<fields.h>

class shipCollection
{
    QVector < QVector<ship*> > playerShips;
    int shipQuantity[4] = {4, 3, 2, 1};
public:
    shipCollection() : playerShips(4)
    {

    }
    void connectShip(ship* S, int size)
    {
        if(shipQuantity[size-1] - 1 >= 0)
        {
            playerShips[size - 1].append(S);
            shipQuantity[size - 1]--;
            for (int i = 0; i < size; i++)
            {
                cell *temp = S->getShipPart(i);
                ((shipCell*)temp)->setParentShip(S);
            }
        }
    }
    int getShipQuantity(int size)
    {
        return shipQuantity[size-1];
    }
    QVector<ship*>* getShipCollection(int size)
    {
        return &playerShips[size - 1];
    }
    void incQuantity(int size)
    {
        shipQuantity[size-1]++;
    }
    void decQuantity(int size)
    {
        shipQuantity[size-1]--;
    }
};

#endif // SHIPCOLLECTION_H
