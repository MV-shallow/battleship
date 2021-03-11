#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qpainter.h>
#include <shipManager.h>
shipManager* manager = NULL;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->topLevelWidget();
    this->setFixedSize(700, 460);
    ui->confirmShipPlacement->setEnabled(false);
    ui->confirmShipPlacement->setVisible(false);
    ui->botShipsWidget->hide();
    ui->playerShipsWidget->hide();
    setMouseTracking(true);
    this->setMouseTracking(true);
    setWindowTitle("Battleship");
    manager = new shipManager();
    updateLabels();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::updateShipCounts()
{
    ui->totalBotShips->setNum(manager->getBot()->getShipCount());
    ui->totalPlayerShips->setNum(manager->getUser()->getShipCount());
}
void MainWindow::updateLabels()
{
    ui->numberOfShip_1x1->setText(QString::number(manager->getUser()->getFleet()->getShipQuantity(1)));
    ui->numberOfShip_1x2->setText(QString::number(manager->getUser()->getFleet()->getShipQuantity(2)));
    ui->numberOfShip_1x3->setText(QString::number(manager->getUser()->getFleet()->getShipQuantity(3)));
    ui->numberOfShip_1x4->setText(QString::number(manager->getUser()->getFleet()->getShipQuantity(4)));
}
endingDialog::endingDialog( QWidget * parent, int victor) : QDialog(parent) {

    setupUi(this);
    Ui::Dialog *ui;
    ui = this;
    victor == (int)botVictory ?
                ui->battleResult->setText("Поражение"):
                ui->battleResult->setText("Победа");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Новая игра");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Выход");
}
void cell::drawCell(int x, int y, QWidget *T, QPoint base)
{
    QPainter pai(T);
    pai.drawPixmap(base.x() + x * 32, base.y() + y * 32, 32, 32, this->baseImage);
    pai.drawRect(base.x() + 32 * x,base.y() + 32 * y, 32, 32);
}

void emptyCell::drawHover(int x, int y, QWidget* T, QPoint base)
{
    QPainter pai(T);
    pai.drawPixmap(base.x() + x * 32, base.y() + y * 32, 32, 32, this->hoverImage);
    pai.drawRect(base.x() + 32 * x,base.y() + 32 * y, 32, 32);
}
void closedCell::drawHover(int x, int y, QWidget* T, QPoint base)
{
    QPainter pai(T);
    pai.drawPixmap(base.x() + x * 32, base.y() + y * 32, 32, 32, this->hoverImage);
    pai.drawRect(base.x() + 32 * x,base.y() + 32 * y, 32, 32);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    manager->keyboardKeyPressed(event);
    MainWindow::update();
}
void MainWindow::paintEvent(QPaintEvent *event)
{
    manager->drawBattle(this);
    updateShipCounts();
}
void MainWindow::wheelEvent(QWheelEvent *event)
{
    manager->wheelScrolled(event);
    MainWindow::update();
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==  Qt::LeftButton)
    {
        switch (manager->getPhase()) {
        case preparation:
        {
            switch(manager->setShip(manager->getUser()))
            {
            case incorrectPosition:
            {
                QMessageBox msgBox;
                msgBox.setText("Неверное расположение!");
                msgBox.exec();
                break;
            }
            case noMoreShips:
            {

                QMessageBox msgBox;
                msgBox.setText("Нельзя установить еще один корабль такого размера");
                msgBox.exec();
                break;
            }
            case allPlaced:
            {
                updateLabels();
                QMessageBox msgBox;
                msgBox.setText("Все корабли на своих позициях!");
                msgBox.exec();
                break;
            }
            case outsideOfField:
            {
                break;
            }
            }
            break;
        }
        case playerAttack:
        {
            manager->userAttack(this);
        }
        }

        updateLabels();
        MainWindow::update();
    }
    else
    {
        manager->deleteShip(manager->getUser());
        ui->confirmShipPlacement->setEnabled(false);
        ui->confirmShipPlacement->setVisible(false);
        updateLabels();
        MainWindow::update();
    }
    if(manager->getUser()->getShipCount() == 10 && manager->getPhase() == preparation)
    {
        ui->confirmShipPlacement->setEnabled(true);
        ui->confirmShipPlacement->setVisible(true);
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    manager->mouseMoved(event);
    MainWindow::update();
}

void MainWindow::on_confirmShipPlacement_clicked()
{
    ui->shipsWidgetLayout->hide();
    updateShipCounts();
    ui->botShipsWidget->show();
    ui->playerShipsWidget->show();
    ui->playerShipsWidget->move(95,340);
    ui->botShipsWidget->move(455,340);
    ui->confirmShipPlacement->setEnabled(false);
    ui->confirmShipPlacement->setVisible(false);
    this->setFixedSize(700, 380);
    manager->shipPlacementConfirmed();
}
