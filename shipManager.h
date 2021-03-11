#ifndef SHIPMANAGER_H
#define SHIPMANAGER_H

#include <QMainWindow>
#include <player.h>
#include "QDebug"
#include <QMessageBox>
#include <random>
#include <QRandomGenerator>
#include <QTime>
#include<QKeyEvent>
#include<QWheelEvent>
#include<QMouseEvent>
#include <interfaceDialog.h>

enum phase { preparation, playerAttack, botAttack, playerVictory, botVictory};
enum placementCase {incorrectPosition, noMoreShips, success, allPlaced, outsideOfField};
enum outcome {hit, miss, wrongCell};
class shipManager
{
    phase currentPhase = preparation;
    player* realUser;
    player* artificialUser;
    QPoint* mPosPrev;
    QPoint hPos;
    QPoint mPos;
    QPoint sPos[4];
    int shipSize;
    int rotation;
    bool cursorOutOfBounds;
public:
    shipManager()
    {
        mPosPrev = new QPoint(11,11);
        realUser = new user(10,10);
        artificialUser = new bot(370,10);
        shipSize = 1;
        rotation = 0;
        cursorOutOfBounds = false;
        autoSetShip();
    }
    void shipPlacementConfirmed()
    {
        shipSize = 1;
        rotation = 0;
        currentPhase = playerAttack;
    }
    void keyboardKeyPressed(QKeyEvent *event)
    {
        if(currentPhase == preparation)
        {
            if(event->key() == 82)
            {
                rotation++;
                rotation %= 4;
                rotateShip(rotation);
            }
        }
    }
    void wheelScrolled(QWheelEvent *event)
    {
        if(currentPhase == preparation)
        {
            if(event->angleDelta().y() > 0)
            {
                shipSize++;
                shipSize = (shipSize % 5);
                if(shipSize == 0)
                    shipSize++;
            }
            else
            {
                shipSize--;
                shipSize = (shipSize % 5);
                if(shipSize == 0)
                    shipSize = 4;
            }
            updateHighlight();
        }
    }
    void mouseMoved(QMouseEvent *event)
    {
        switch(currentPhase)
        {
        case preparation:
        {
            moveToCellLocation(realUser->getBase() ,event->pos());
            break;
        }
        case playerAttack:
        {
            moveToCellLocation(artificialUser->getBase() ,event->pos());
            break;
        }
        }
    }
    player* getUser()
    {
        return realUser;
    }
    player* getBot()
    {
        return artificialUser;
    }
    void userAttack(QWidget *T)
    {;
        if(sPos[0].x()>9 || sPos[0].y()>9 || currentPhase == botVictory || currentPhase == playerVictory)
            return;
        cell *displayedCell = artificialUser->getFieldForDisplay()->getCell(sPos[0].x(),sPos[0].y());
        switch (displayedCell->getType()) {
        case shot:
        {
            return;
            break;
        }
        case destroyed:
        {
            return;
            break;
        }
        case destroyedPart:
        {
            return;
            break;
        }
        case closed:
        {
            outcome res;
            res = fireAtShip(artificialUser);
            switch (res) {
            case hit:
            {
                break;
            }
            case miss:
            {
                currentPhase = botAttack;
                break;
            }
            }
            break;
        }
        }
        if (!checkForRemainingShips(artificialUser))
        {
            currentPhase = playerVictory;
            finalDialog();
            return;
        }
        if (currentPhase == botAttack)
        {
            AIAttack(T);
        }
    }
    void setPhase(phase P)
    {
        currentPhase = P;
    }
    phase getPhase()
    {
        return currentPhase;
    }
    void drawBattle(QWidget *T)
    {
        drawBaseFields(T);

        switch (currentPhase)
        {
        case preparation:
        {
            for (int i = 0; i < shipSize; i++)
            {
                if(sPos[i].x() >= 0 && sPos[i].x() < 10 && sPos[i].y() >= 0 && sPos[i].y() < 10)
                {
                    cell* temp = realUser->getField()->getCell(sPos[i].x(),sPos[i].y());
                    if(temp->getType() == empty)
                        ((emptyCell*)temp)->drawHover(sPos[i].x(),sPos[i].y(),T,realUser->getBase());
                }
            }
            break;
        }
        case playerAttack:
        {

            for (int i = 0; i < shipSize; i++)
            {
                if(sPos[i].x() >= 0 && sPos[i].x() < 10 && sPos[i].y() >= 0 && sPos[i].y() < 10)
                {
                    cell* temp = artificialUser->getFieldForDisplay()->getCell(sPos[i].x(),sPos[i].y());
                    if (temp->getType() == closed)
                        ((closedCell*)temp)->drawHover(sPos[i].x(),sPos[i].y(),T,artificialUser->getBase());
                }
            }
            break;
        }
        case botAttack:
        {
            break;
        }
        }
    }
    void deleteShip(player *owner)
    {
        if(mPos.x() >= 0 && mPos.x() < 10)
        {
            if(mPos.y() >= 0 && mPos.y() < 10)
            {
                if (owner->getField()->getCell(mPos.x(), mPos.y())->getType() == shipPart)
                {

                    cell* temp = owner->getField()->getCell(mPos.x(), mPos.y());
                    ship* pointer = ((shipCell*)temp)->getParentShip();
                    int iter = pointer->getHealth();

                    for (int i = 0; i < iter; i++)
                    {
                        temp = pointer->getShipPart(i);
                        cell* emptC = new emptyCell(temp->x(),temp->y());
                        owner->getField()->setCell(temp->x(),temp->y(),emptC);
                    }
                    owner->getFleet()->getShipCollection(iter)->removeOne(pointer);
                    delete pointer;
                    owner->getFleet()->incQuantity(iter);
                    owner->decShipCount();
                }
            }
        }
    }
    placementCase setShip(player *owner)
    {
        if(owner->getFleet()->getShipQuantity(shipSize) == 0)
        {
            return noMoreShips;
        }
        if (mPos.x() >= 0 && mPos.x() < 10 && mPos.y() >= 0 && mPos.y() < 10)
        {
            for(int i = 0; i < shipSize; i++)
            {
                if(!isCellSuitable(sPos[i],owner))
                {
                    return incorrectPosition;
                }
            }
            switch (shipSize) {
            case 1:
            {
                cell* temp = new shipCell(sPos[0].x(),sPos[0].y());
                ship* tempShip = new ship_1x1();
                tempShip->connectCell(temp,0);
                owner->getField()->setCell(sPos[0].x(),sPos[0].y(),temp);
                owner->getFleet()->connectShip(tempShip, shipSize);
                owner->incShipCount();
                break;
            }
            case 2:
            {
                cell* temp = new shipCell(sPos[0].x(),sPos[0].y());
                ship* tempShip = new ship_1x2();
                tempShip->connectCell(temp,0);
                owner->getField()->setCell(sPos[0].x(),sPos[0].y(),temp);
                temp = new shipCell(sPos[1].x(),sPos[1].y());
                tempShip->connectCell(temp,1);
                owner->getField()->setCell(sPos[1].x(),sPos[1].y(),temp);
                owner->getFleet()->connectShip(tempShip, shipSize);
                owner->incShipCount();
                break;
            }
            case 3:
            {
                cell* temp = new shipCell(sPos[0].x(),sPos[0].y());
                ship* tempShip = new ship_1x3();
                tempShip->connectCell(temp,0);
                owner->getField()->setCell(sPos[0].x(),sPos[0].y(),temp);
                temp = new shipCell(sPos[1].x(),sPos[1].y());
                tempShip->connectCell(temp,1);
                owner->getField()->setCell(sPos[1].x(),sPos[1].y(),temp);
                temp = new shipCell(sPos[2].x(),sPos[2].y());
                tempShip->connectCell(temp,2);
                owner->getField()->setCell(sPos[2].x(),sPos[2].y(),temp);
                owner->getFleet()->connectShip(tempShip, shipSize);
                owner->incShipCount();
                break;
            }
            case 4:
            {
                cell* temp = new shipCell(sPos[0].x(),sPos[0].y());
                ship* tempShip = new ship_1x4();
                tempShip->connectCell(temp,0);
                owner->getField()->setCell(sPos[0].x(),sPos[0].y(),temp);
                temp = new shipCell(sPos[1].x(),sPos[1].y());
                tempShip->connectCell(temp,1);
                owner->getField()->setCell(sPos[1].x(),sPos[1].y(),temp);
                temp = new shipCell(sPos[2].x(),sPos[2].y());
                tempShip->connectCell(temp,2);
                owner->getField()->setCell(sPos[2].x(),sPos[2].y(),temp);
                temp = new shipCell(sPos[3].x(),sPos[3].y());
                tempShip->connectCell(temp,3);
                owner->getField()->setCell(sPos[3].x(),sPos[3].y(),temp);
                owner->getFleet()->connectShip(tempShip, shipSize);
                owner->incShipCount();
                break;
            }
            }
            if(owner->getShipCount() == 10)
            {
                return allPlaced;
            }
            return success;
        }
        return outsideOfField;
    }
private:
    bool checkForRemainingShips(player* opponent)
    {
        if(opponent->getShipCount()==0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    void finalDialog()
    {
        endingDialog* dd = new endingDialog(0,currentPhase);
        dd->setModal(true);
        dd->show();
    }
    void delay( int ms )
    {
        QTime dieTime = QTime::currentTime().addMSecs( ms );
        while( QTime::currentTime() < dieTime )
        {
            QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
        }
    }
    void AIAttack(QWidget *T)
    {
        bool attackSuccessful = true;
        while(attackSuccessful)
        {
            delay(1000);
            cell *displayedCell = NULL;
            do
            {
                quint32 i = QRandomGenerator::global()->bounded(0, 10);
                quint32 j = QRandomGenerator::global()->bounded(0, 10);
                sPos[0].setX(i);
                sPos[0].setY(j);
                displayedCell = realUser->getFieldForDisplay()->getCell(sPos[0].x(),sPos[0].y());
            }
            while (displayedCell->getType() == shot || displayedCell->getType() == destroyed || displayedCell->getType() == destroyedPart);
            outcome res;
            res = fireAtShip(realUser);
            switch (res) {
            case hit:
            {
                break;
            }
            case miss:
            {
                currentPhase = playerAttack;
                attackSuccessful = false;
                break;
            }
            }
            if (!checkForRemainingShips(realUser))
            {
                currentPhase = botVictory;
                finalDialog();
                attackSuccessful = false;
                return;
            }
            sPos[0].setX(11);
            sPos[0].setY(11);
            T->update();
        }
    }
    outcome fireAtShip(player *recipient)
    {
        cell * attackedCell = recipient->getField()->getCell(sPos[0].x(),sPos[0].y());
        switch(attackedCell->getType())
        {
        case empty:
        {
            cell* badShot = new shotEmptyCell(sPos[0].x(),sPos[0].y());
            recipient->getField()->setCell(sPos[0].x(),sPos[0].y(),badShot);
            recipient->getFieldForDisplay()->setCell(sPos[0].x(),sPos[0].y(),badShot);
            return miss;
            break;
        }
        case shipPart:
        {
            ship* attackedShip;
            attackedShip = ((shipCell*)attackedCell)->getParentShip();
            int attackedShipSize = attackedShip->getSize();
            cell* damagedShipCell = new damagedCell(sPos[0].x(),sPos[0].y());
            int index = attackedShip->getAllShipParts()->indexOf(attackedCell);
            attackedShip->getAllShipParts()->replace(index,damagedShipCell);
            recipient->getField()->setCell(sPos[0].x(),sPos[0].y(),damagedShipCell);
            damagedShipCell = new damagedCell(sPos[0].x(),sPos[0].y());
            recipient->getFieldForDisplay()->setCell(sPos[0].x(),sPos[0].y(),damagedShipCell);
            switch (attackedShip->decHealth()) {
            case shipDamaged:
            {
                return hit;
                break;
            }
            case shipDestroyed:
            {
                QVector<cell*>* temp_vector = attackedShip->getAllShipParts();

                for (int i = 0; i < attackedShipSize; i++)
                {
                    cell* temp = temp_vector->back();
                    int temp_x = temp->x();
                    int temp_y = temp->y();
                    cell* destrCell = new destroyedCell(temp->x(),temp->y());
                    recipient->getField()->setCell(temp_x, temp_y, destrCell);
                    recipient->getFieldForDisplay()->setCell(temp_x, temp_y, destrCell);
                    temp_vector->pop_back();
                }
                  recipient->getFleet()->decQuantity(attackedShipSize);
                  recipient->getFleet()->getShipCollection(attackedShipSize)->removeOne(attackedShip);
                  recipient->decShipCount();
                return hit;
                break;
            }
            }
        }
        }
    }
    void xRAYbotField()
    {
        QString temp;
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                temp.append(artificialUser->getField()->getCell(j,i)->getType() == empty ? "0 ":artificialUser->getField()->getCell(j,i)->getType() == shipPart ? "X ": "- " );
            }
            qDebug()<<temp;
            temp.clear();
        }
    }
    void autoSetShip()
    {
        shipSize = 4;
        bool exit = false;
        int autoRotation = 0;
        while(artificialUser->getShipCount() != 10)
        {
            quint32 i = QRandomGenerator::global()->bounded(0, 10);
            quint32 j = QRandomGenerator::global()->bounded(0, 10);
            mPos.setX(i);
            mPos.setY(j);
            if (artificialUser->getField()->getCell(mPos.x(),mPos.y())->getType() != shipPart)
            {
                exit = false;
                while (!exit)
                {
                    rotateShip(autoRotation);
                    placementCase ans = setShip(artificialUser);
                    switch(ans)
                    {
                    case incorrectPosition:
                    {
                        if (autoRotation == 3)
                        {
                            autoRotation = 0;
                            exit = true;
                        }
                        else
                        {
                            ++autoRotation;
                        }
                        break;
                    }
                    case noMoreShips:
                    {
                        shipSize--;
                        exit = true;
                        break;
                    }
                    case allPlaced:
                    case success:
                    {
                        exit = true;
                        break;
                    }
                    }
                    if(exit)
                        break;
                }
            }
        }
        xRAYbotField();
    }
    bool isCellSuitable(QPoint C, player* owner)
    {
        int x, y;
        for (int i = -1; i <= 1; i++)
        {
            for(int j = -1; j <= 1; j++)
            {
                x = C.x() + i;
                y = C.y() + j;
                if(C.x() == x && C.y() == y)
                {
                    if(x >= 0 && x < 10 && y >= 0 && y < 10)
                    {
                        if(owner->getField()->getCell(x,y)->getType() == empty)
                        {

                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                else if(x >= 0 && x < 10 && y >= 0 && y < 10)
                {
                    if(owner->getField()->getCell(x,y)->getType() == empty)
                    {

                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    void rotateShip(int rotation)
    {
        switch (rotation) {
        case 0:
        {
            for (int i = 0; i < shipSize; i++)
            {
                sPos[i].setX(mPos.x());
                sPos[i].setY(mPos.y() - i);
            }
            break;
        }
        case 1:
        {
            for (int i = 0; i < shipSize; i++)
            {
                sPos[i].setX(mPos.x() + i);
                sPos[i].setY(mPos.y());
            }
            break;
        }
        case 2:
        {
            for (int i = 0; i < shipSize; i++)
            {
                sPos[i].setX(mPos.x());
                sPos[i].setY(mPos.y() + i);
            }
            break;
        }
        case 3:
        {
            for (int i = 0; i < shipSize; i++)
            {
                sPos[i].setX(mPos.x() - i);
                sPos[i].setY(mPos.y());
            }
            break;
        }
        }
    }
    void updateHighlight()
    {
        if(mPos.x() >= 0 && mPos.x() < 10)
        {
            if(mPos.y() >= 0 && mPos.y() < 10)
            {
                cursorOutOfBounds = false;
                if(mPos.x()==10)
                    mPos.setX(9);
                if(mPos.y()==10)
                    mPos.setY(9);
                rotateShip(rotation);
                mPosPrev->setX(mPos.x());
                mPosPrev->setY(mPos.y());
            }
            else
            {
                cursorOutOfBounds = true;
            }
        }
        else
        {
            cursorOutOfBounds = true;
        }
        if(cursorOutOfBounds)
        {
            sPos[0].setX(11);
            sPos[0].setY(11);
            sPos[1].setX(11);
            sPos[1].setY(11);
            sPos[2].setX(11);
            sPos[2].setY(11);
            sPos[3].setX(11);
            sPos[3].setY(11);
            mPosPrev->setX(11);
            mPosPrev->setY(11);
            mPos.setX(11);
            mPos.setY(11);
        }
    }
    void moveToCellLocation(QPoint basePos, QPoint mouse_pos)
    {
        if(mouse_pos.x() >= basePos.x() && mouse_pos.x() <= 320 + basePos.x())
        {
            if(mouse_pos.y() >= basePos.y() && mouse_pos.y() <= 320 + basePos.y())
            {
                cursorOutOfBounds = false;
                mPos.setX((mouse_pos.x()- basePos.x())/32);
                mPos.setY((mouse_pos.y()-basePos.y())/32);
                if(mPos.x()==10)
                    mPos.setX(9);
                if(mPos.y()==10)
                    mPos.setY(9);
                if(mPos != (*mPosPrev))
                    rotateShip(rotation);
                mPosPrev->setX(mPos.x());
                mPosPrev->setY(mPos.y());
            }
            else
            {
                cursorOutOfBounds = true;

            }
        }else
            cursorOutOfBounds = true;
        if(cursorOutOfBounds)
        {
            sPos[0].setX(11);
            sPos[0].setY(11);
            sPos[1].setX(11);
            sPos[1].setY(11);
            sPos[2].setX(11);
            sPos[2].setY(11);
            sPos[3].setX(11);
            sPos[3].setY(11);
            mPosPrev->setX(11);
            mPosPrev->setY(11);
            mPos.setX(11);
            mPos.setY(11);
        }
    }
    void drawBaseFields(QWidget *T)
    {
        for(int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                realUser->getField()->getCell(j,i)->drawCell(j,i,T,realUser->getBase());
                artificialUser->getFieldForDisplay()->getCell(i,j)->drawCell(i,j,T,artificialUser->getBase());
            }
        }
    }
};
#endif // SHIPMANAGER_H

