#ifndef TRAININGMODE_H
#define TRAININGMODE_H

#include <QDialog>
#include <QMessageBox>
#include <QFile>
#include <ctime>
#include "globalfuncs.h"

namespace Ui {
class trainingMode;
}

class trainingMode : public QDialog
{
    Q_OBJECT

public:
    explicit trainingMode(QWidget *parent = nullptr);
    ~trainingMode();
    //--------------

    QVector<QString> wordBase;
    int sizeOfBase;

private:
    Ui::trainingMode *ui;
    //-------------------

    class exceptionToLoop { };
    QVector<QString> dontRepeatBase;
    QString dontRepeatNow;
    QString checkEnter;
    int count;
    void nextWord();
    int amountOfPulled(const QString);
    void refreshTraining();

private slots:
    void on_pushButtonCheck_clicked();
    void on_pushButtonRepeat_clicked();
    void on_pushButtonIDontKnow_clicked();
    void on_pushButtonSkip_clicked();
    void on_pushButtonStartOver_clicked();
    void on_pushButtonSetImg_clicked();
};

#endif // TRAININGMODE_H
