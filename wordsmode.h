#ifndef WORDSMODE_H
#define WORDSMODE_H

#include <QDialog>
#include <QDesktopServices>
#include <QInputDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QTime>
#include <QUrl>
#include <QMap>
#include <map>
#include "globalfuncs.h"

namespace Ui {
class wordsMode;
}

class wordsMode : public QDialog
{
    Q_OBJECT

public:
    explicit wordsMode(QWidget *parent = nullptr);
    ~wordsMode();

private:
    Ui::wordsMode *ui;
    //----------------

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

#endif // WORDSMODE_H
