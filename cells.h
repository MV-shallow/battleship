
#ifndef CELLS_H
#define CELLS_H

#include <QMainWindow>
#include <QApplication>

#endif // CELLS_H
enum cellType {empty, shipPart, closed, destroyed, shot, destroyedPart};

class ship;

class cell
{
protected:
    QPoint p;
    QPixmap baseImage;
    cellType type;
public:
    void drawCell(int x,int y, QWidget* T, QPoint base);
    cellType getType()
    {
        return type;
    }
    int x()
    {
        return p.x();
    }
    int y()
    {
        return p.y();
    }
    virtual ~cell() = default;
};
class emptyCell:public cell
{
    QPixmap hoverImage;
public:
    emptyCell(int x, int y)
    {
        p.setX(x);
        p.setY(y);
        baseImage.load("D:/pics/emptyCell.png");
        hoverImage.load("D:/pics/emptyCellHover.png");
        type = empty;
    }
    void drawHover(int x,int y, QWidget* T, QPoint base);

};
class shipCell:public cell
{
    ship* pointer;
public:
    shipCell(int x, int y)
    {
        p.setX(x);
        p.setY(y);
        baseImage.load("D:/pics/shipCell.png");
        type = shipPart;
    }
    ship* getParentShip()
    {
        return pointer;
    }
    void setParentShip(ship * parent)
    {
        pointer = parent;
    }
};
class closedCell:public cell
{
    QPixmap hoverImage;
public:
    closedCell(int x, int y)
    {
        p.setX(x);
        p.setY(y);
        baseImage.load("D:/pics/closedCell.png");
        hoverImage.load("D:/pics/closedCellHover.png");
        type = closed;
    }
    void drawHover(int x,int y, QWidget* T, QPoint base);
};
class destroyedCell:public cell
{
public:
    destroyedCell(int x, int y)
    {
        p.setX(x);
        p.setY(y);
        baseImage.load("D:/pics/destroyedCell.png");
        type = destroyedPart;
    }
};
class shotEmptyCell:public cell
{
public:
    shotEmptyCell(int x, int y)
    {
        p.setX(x);
        p.setY(y);
        baseImage.load("D:/pics/shotEmptyCell.png");
        type = shot;
    }
};
class damagedCell:public cell
{
public:
    damagedCell(int x, int y)
    {
        p.setX(x);
        p.setY(y);
        baseImage.load("D:/pics/damagedCell.png");
        type = destroyedPart;
    }
};

