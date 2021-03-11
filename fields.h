
#ifndef FIELDS_H
#define FIELDS_H

//#include <QMainWindow>
#include <ships.h>
#include <cells.h>
enum fieldType {emptyField, closedField};
class field
{
private:
   cell* cells[10][10];
public:
   field(fieldType p)
   {
       switch(p)
       {
       case emptyField:
       {
           for(int i = 0; i < 10; i++)
               for(int j = 0; j < 10; j++)
               {
                   emptyCell* temp = new emptyCell(i,j);
                   cells[i][j] = temp;
               }
           break;
       }
       case closedField:
       {
           for(int i = 0; i < 10; i++)
               for(int j = 0; j < 10; j++)
               {
                   closedCell* temp = new closedCell(i,j);
                   cells[i][j] = temp;
               }
           break;
       }
       }
   }
   cell* getCell(int x, int y)
   {
       return cells[x][y];
   }
   void setCell(int x, int y, cell *&c)
   {
       cell* temp = cells[x][y];
       cells[x][y] = c;
       delete temp;
   }
   ~field()
   {
       for(int i = 0; i < 10; i++)
           for(int j = 0; j < 10; j++)
           {
               delete cells[i][j];
           }
   }
};

#endif // FIELDS_H

