#ifndef ENDDIALOG_H
#define ENDDIALOG_H

#include<QDialog>
#include "ui_interfaceDialog.h"
#include<QProcess>

class endingDialog : public QDialog, public Ui::Dialog {
    Q_OBJECT

public:
    endingDialog( QWidget * parent = 0, int i = 0);
private slots:
    void on_Dialog_accepted()
    {
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        qApp->activeWindow();
    }
    void on_Dialog_rejected()
    {
        QCoreApplication::quit();
    }
};

#endif // ENDDIALOG_H
