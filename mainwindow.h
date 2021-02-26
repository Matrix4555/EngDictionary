#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsBlurEffect>
#include <QInputDialog>
#include <QColorDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QTextCursor>
#include <QListWidget>
#include <QTextEdit>
#include <QPainter>
#include <QScreen>
#include <QWidget>
#include <QImage>
#include <QColor>
#include <QLabel>
#include <QFile>
#include <functional>

#include "globalfuncs.h"
#include "trainingmode.h"
#include "wordsmode.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    //-----------------

    class SetTranslationDialog
    {
    public:
        SetTranslationDialog() : winDialog(new QDialog)
        { create(); }

        SetTranslationDialog(QWidget* ref, const QString toInput) : winDialog(new QDialog(ref))
        { create(ref, toInput); }

        ~SetTranslationDialog()
        { delete winDialog; }

        int exec()
        {
            winDialog->exec();
            return conduct;
        }

        void selectText()
        { input->selectAll(); }

        QString getText()
        { return input->text(); }

        void setText(const QString text)
        { input->setText(text); }

        void setStyleSheet(const QString realStSh)
        {
            QString color = realStSh;
            QString background = realStSh;
            color.truncate(color.indexOf('\n')+1);
            background.remove(0, background.indexOf('\n')+1);

            int c = 0, l = 0;
            while(c != 3)
                if(background[l++] == ',')
                    c++;
            background.truncate(l);
            background += "90);";

            winDialog->     setStyleSheet(background);
            label->         setStyleSheet("font: 16pt Comic Sans MS; font-weight: bold; qproperty-alignment: AlignCenter; background-color: rgb(255,255,255,0); " + color);
            input->         setStyleSheet("font: 20pt Comic Sans MS; " + color + background);
            QString temp =  styleSheetForButton(color + background, "font: 15pt Comic Sans MS; font-weight: bold;");

            for(int i=0, s=0; i<3; i++, s+=75)
                button[i]->setStyleSheet(temp);
        }

    private:
        int conduct;
        QDialog* winDialog;
        QLabel* label =             new QLabel(winDialog);
        QLineEdit* input =          new QLineEdit(winDialog);
        QPushButton* button[4] = {  new QPushButton(winDialog), new QPushButton(winDialog),
                                    new QPushButton(winDialog), new QPushButton(winDialog) };

        void create(QWidget* mainWin = nullptr, const QString needTranslate = nullptr)
        {
            winDialog->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
            label->setText("Enter Translation:");

            if(mainWin != nullptr)
            {
                connect(button[0], &QPushButton::clicked, mainWin, [this]()
                {
                    conduct = 0;
                    winDialog->close();
                });
                connect(button[1], &QPushButton::clicked, mainWin, [this]()
                {
                    conduct = 1;
                    winDialog->close();
                });
                connect(button[2], &QPushButton::clicked, mainWin, [this]()
                {
                    conduct = 2;
                    winDialog->close();
                });
                connect(button[3], &QPushButton::clicked, mainWin, [needTranslate]()
                {
                    QDesktopServices::openUrl(QUrl("https://translate.google.com/#view=home&op=translate&sl=en&tl=ru&text=" + needTranslate));
                });

                winDialog->         setGeometry(mainWin->width()/2-170, mainWin->height()/2-75, 340, 150);
            }

            label->                 setGeometry(10,13,320,20);
            input->                 setGeometry(10,45,320,50);
            for(int i=0, s=0; i<3; i++, s+=75)
                button[i]->         setGeometry(110+s, 103, 70, 40);
            button[3]->             setGeometry(10, 103, 40, 40);

            button[3]->setText("");
            button[3]->setStyleSheet("QPushButton{background-color: rgba(255, 255, 255, 0);}"
                                     "QPushButton::hover{background-color: rgba(100, 161, 237, 100);}");
            button[3]->setIcon(QPixmap(":/img/images/googletranslate logo.png"));
            button[3]->setIconSize(QSize(32, 32));
            button[3]->show();

            winDialog->setMinimumSize(QSize(340, 150));
            winDialog->setMaximumSize(QSize(340, 150));

            button[0]->setText("Apply");
            button[1]->setText("Merge");
            button[2]->setText("Cancel");
        }
    };

    QString getTranslations(const QString, const QLabel*, const int);
    void setEnabledButtons(const bool);
    void clarifyDay(const QPushButton*, QLabel*&, QLabel*&, int&);
    bool fullHd;

private slots:
    void changeCard();
    void on_pushButtonCheckKnow_clicked();
    void on_pushButtonTraining_clicked();
    void on_pushButtonWords_clicked();
    void on_pushButtonSetImg_clicked();
    void on_pushButtonMenuColor_clicked();
    void on_pushButtonGetCards_clicked();
    void on_pushButtonInfo_clicked();
};

#endif // MAINWINDOW_H
