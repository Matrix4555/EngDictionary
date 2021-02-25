#ifndef WORDSMODE_H
#define WORDSMODE_H

#include <QDialog>
#include <QDesktopServices>
#include <QInputDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QThread>
#include <QUrl>
#include <QMap>
#include <chrono>
#include <thread>
#include <map>
#include "globalfuncs.h"

namespace Ui {
class wordsMode;
}

class ResultInThread;

class wordsMode : public QDialog
{
    Q_OBJECT

public:
    explicit wordsMode(QWidget *parent = nullptr);
    ~wordsMode();

private:
    Ui::wordsMode *ui;
    //----------------

    ResultInThread* rit;            // for delay of a result after some action
    QMap<QString, QString> list;
    void refreshList();
    void showResult(const QString, const QString);
    QString changeWord(const QString, const QString);

private slots:
    void on_pushButtonTranslate_clicked();
    void on_pushButtonAdd_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonFind_clicked();
    void on_pushButtonChange_clicked();
    void on_pushButtonTranslator_clicked();
    void on_pushButtonSetImg_clicked();
    void on_pushButtonRunNote_clicked();
    void on_pushButtonCrib_clicked();
};

class ResultInThread : public QThread
{
    Q_OBJECT
public:
    void run() override
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        emit hideResult();
    }
signals:
    void hideResult();
};

#endif // WORDSMODE_H
