#include "wordsmode.h"
#include "ui_wordsmode.h"

wordsMode::wordsMode(const bool isFullhd, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wordsMode),
    rit(nullptr),
    fullHd(isFullhd)
{
    ui->setupUi(this);
    //----------------

    setWindowTitle("Word Management");
    setWindowFlags(Qt::Window);

    if(QFile("mfimgw.dll").exists())
        this->setStyleSheet("QDialog{background-image: url(mfimgw.dll)}"
                            "QMessageBox{background-color: rgb(0, 85, 127); font: 14pt Comic Sans MS; background-image: url(0);}"
                            "QLabel{color: rgb(255, 255, 0)}");
    else
        this->setStyleSheet("QDialog{background-color: rgb(0, 85, 127)}"
                            "QMessageBox{background-color: rgb(0, 85, 127); font: 14pt Comic Sans MS; background-image: url(0);}"
                            "QLabel{color: rgb(255, 255, 0)}");

    ui->pushButtonTranslator->setIcon(QPixmap(":/img/images/googletranslate.png"));
    ui->pushButtonTranslator->setIconSize(QSize(205, 40));
    ui->listWidget->setFocusPolicy(Qt::NoFocus);

    if(!fullHd)
    {
        this->setMinimumSize(960, 660);
        this->setMaximumSize(960, 660);
        this->resize(960, 660);

        ui->listWidget->            resize(270, 620);
        ui->pushButtonCrib->        setGeometry(735, 70, 100, 40);
        ui->pushButtonRunNote->     setGeometry(840, 70, 100, 40);
        ui->pushButtonSetImg->      setGeometry(840, 600, 100, 40);
    }

    refreshList();
}

wordsMode::~wordsMode()
{
    delete ui;
}

void wordsMode::refreshList()
{
    ui->listWidget->clear();
    list.clear();

    QFile file("mfwd.dll");
    if(!file.exists())
    {
        ui->labelWordsCount->setText(QString::number(0));
        return;
    }

    int count = 0;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if(file.size() == 0)
        {
            ui->labelWordsCount->setText(QString::number(0));
            file.close();
            return;
        }
        while(!file.atEnd())
        {
            QString word = file.readLine();
            QString eng, rus;
            getWordsSeparately(word, eng, rus);
            list[eng] = rus;
            ui->listWidget->addItem(eng);
            ui->labelWordsCount->setText(QString::number(++count));
        }
        file.close();
    }
}

void wordsMode::showResult(const QString color, const QString result)
{
    ui->labelResult->setStyleSheet("background-color: rgba(255, 0, 0, 100);"+color);
    ui->labelResult->setText(result);
    ui->labelResult->show();

    rit = new ResultInThread();
    connect(rit, &ResultInThread::hideResult, this, [this]() {          // delay a result for 2 sec
        ui->labelResult->hide();
        ui->labelResult->setText("");
    });
    connect(rit, &ResultInThread::finished, rit, &QObject::deleteLater);
    rit->start();
}

void wordsMode::on_pushButtonTranslate_clicked()
{
    if(ui->listWidget->selectedItems().size() == 0)
    {
        QMessageBox::information(this, "Info", "Word is not selected");
        return;
    }

    QMessageBox win(this);
    win.setIcon(QMessageBox::Information);
    win.setWindowTitle("Translation");
    win.setStyleSheet("QLabel{background-color: rgba(255, 0, 0, 100); color: rgb(0, 255, 0); font: 30pt Comic Sans MS; qproperty-alignment: AlignCenter}"
                      "QPushButton{background-color: rgba(255, 0, 0, 100); color: rgb(0, 255, 0); font: 15pt Comic Sans MS;}"
                      "QPushButton::hover{background-color: rgba(255, 0, 0, 150)}");
    win.setText(list[ui->listWidget->currentItem()->text()]);
    win.addButton("OK", QMessageBox::YesRole);
    win.exec();
}

void wordsMode::on_pushButtonAdd_clicked()
{
    QString eng = ui->lineEditAdd->text();

    if(eng.size() == 0)
    {
        QMessageBox::information(this, "Info", "The line is empty");
        return;
    }

    if(!eng.contains(" / "))
    {
        QMessageBox::information(this, "Info", "You've entered the new word with\ntranslation incorrectly!");
        return;
    }

    if(eng[eng.size()-1] == ' ' && eng[eng.size()-2] == '/')
    {
        QString temp = eng;
        temp.truncate(temp.indexOf('/')-1);
        QDesktopServices::openUrl(QUrl("https://translate.google.com/#view=home&op=translate&sl=en&tl=ru&text=" + temp));
        return;
    }

    if(eng == "hello / привет")
    {
        QMessageBox::information(this, "Info", "No no no. Let's something another ))");
        return;
    }

    QString rus = eng;
    eng.truncate(eng.indexOf('/')-1);       // discard the right side after the slash
    rus.remove(0, rus.indexOf('/')+2);      // discard the left side before the slash

    if(!checkCorrectness(eng, "eng"))
    {
        QMessageBox::warning(this, "Error", "In the left side must be English word");
        return;
    }
    if(!checkCorrectness(rus, "rus"))
    {
        QMessageBox::warning(this, "Error", "In the right side must be Russian word");
        return;
    }

    for(int i = 0, amount = ui->listWidget->count(); i < amount; i++)
        if(ui->listWidget->item(i)->text() == eng)
        {
            QMessageBox::information(this, "Info", "This word is already there");
            ui->lineEditAdd->setText("");
            ui->listWidget->setCurrentRow(i);
            return;
        }

    const QString addWord = eng + '_' + rus + '\n';

    QFile recommendations("mfchit.dll");
    if(recommendations.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        bool addToRecom = true;
        while(!recommendations.atEnd())
            if(addWord == recommendations.readLine())
            {
                addToRecom = false;
                break;
            }
        if(addToRecom)
            recommendations.write(addWord.toUtf8());
        recommendations.close();
    }

    addNewWord(addWord);
    ui->lineEditAdd->setText("");
    refreshList();
    showResult("color: rgb(13, 213, 29);", "Word Added");
}

void wordsMode::on_pushButtonDelete_clicked()
{
    if(ui->listWidget->selectedItems().size() == 0)
    {
        QMessageBox::information(this, "Info", "Word is not selected");
        return;
    }

    const QString deleteWord = ui->listWidget->currentItem()->text() + '_' + list[ui->listWidget->currentItem()->text()] + '\n';

    auto move = toVectorFromFile("mfwd.dll");
    QFile words("mfwd.dll");
    if (words.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        move.remove(ui->listWidget->currentRow());
        for(QString have : move)
            words.write(have.toUtf8());
        words.close();
    }

    auto moveChit = toVectorFromFile("mfchit.dll");
    if(moveChit.contains(deleteWord))
    {
        QFile chitFile("mfchit.dll");
        if (chitFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            for(QString have : moveChit)
                if(have != deleteWord)
                    chitFile.write(have.toUtf8());
            chitFile.close();
        }
    }

    refreshList();
    showResult("color: rgb(255, 0, 0);", "Word Deleted");
}

void wordsMode::on_pushButtonFind_clicked()
{
    QInputDialog win(this);
    QString findWord;
    bool isEng;

    win.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
    win.setStyleSheet("QDialog{background-image: url(0); background-color: rgb(0, 85, 127)}"
                       "QLabel{font: 16pt Comic Sans MS; font-weight: bold; color: rgb(255, 255, 0); background-color: rgb(0, 85, 127); qproperty-alignment: AlignCenter}"
                       "QLineEdit{font: 20pt Comic Sans MS; color: rgb(0, 255, 0); background-color: rgba(0, 255, 0, 50)}"
                       "QPushButton{font: 15pt Comic Sans MS; font-weight: bold; color: rgb(255, 255, 0); background-color: rgba(0, 255, 0, 50)}"
                       "QPushButton::hover{background-color: rgba(0, 255, 0, 100)}");
    win.setLabelText("Enter what to look for");
    win.setOkButtonText("Find");

    while(true)
    {
        if(win.exec() != QDialog::Accepted)
            return;

        if(win.textValue().size() == 0)
        {
            QMessageBox::information(this, "Info", "The line is empty");
            continue;
        }

        findWord = win.textValue();
        if(checkCorrectness(findWord, "eng"))
        {
            isEng = true;
            break;
        }
        else if(checkCorrectness(findWord, "rus"))
        {
            isEng = false;
            break;
        }
        else
        {
            QMessageBox::warning(this, "Error", "You entered the word incorrectly!");
            continue;
        }
    }

    auto selectWord = [=](const QString word)
    {
        for(int i = 0, amount = ui->listWidget->count(); i < amount; i++)
            if(ui->listWidget->item(i)->text() == word)
            {
                ui->listWidget->setCurrentRow(i);
                break;
            }
    };

    if(isEng)
    {
        if(list[findWord].isNull())
        {
            QMessageBox::information(this, "Not found", "Your base has not this word");
            on_pushButtonFind_clicked();
            return;
        }
        selectWord(findWord);
    }
    else
    {
        QString fromList = "";
        std::map<QString, QString> temp = list.toStdMap();
        for(std::map<QString, QString>::iterator iter = temp.begin(); iter != temp.end(); iter++)
            if(iter->second == findWord)
            {
                fromList = iter->first;
                break;
            }

        if(fromList.size() == 0)
        {
            QMessageBox::information(this, "Not found", "Your base has not this word");
            on_pushButtonFind_clicked();
            return;
        }
        selectWord(fromList);
    }

    showResult("color: rgb(19, 121, 255);", "Word Found");
}

QString wordsMode::changeWord(const QString word, const QString version)
{
    QInputDialog win(this);
    win.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
    win.setStyleSheet("QDialog{background-image: url(0); background-color: rgb(0, 85, 127)}"
                       "QLabel{font: 16pt Comic Sans MS; font-weight: bold; color: rgb(255, 255, 0); background-color: rgb(0, 85, 127); qproperty-alignment: AlignCenter}"
                       "QLineEdit{font: 20pt Comic Sans MS; color: rgb(0, 255, 0); background-color: rgba(0, 255, 0, 50)}"
                       "QPushButton{font: 15pt Comic Sans MS; font-weight: bold; color: rgb(255, 255, 0); background-color: rgba(0, 255, 0, 50)}"
                       "QPushButton::hover{background-color: rgba(0, 255, 0, 100)}");
    win.setLabelText("Change the " +version+ " version");
    win.setOkButtonText("Apply");

    while(true)
    {
        win.setTextValue(word);
        if(win.exec() != QDialog::Accepted)
            return "cancel";

        QString enter = win.textValue();
        if(enter == word)   // if no changes
            return enter;

        if(enter.size() == 0)
        {
            QMessageBox::information(this, "Info", "The line is empty");
            continue;
        }

        if(version=="English")
        {
            if(!list[enter].isNull())
            {
                QMessageBox::information(this, "Info", "Your base has this word");
                continue;
            }
            if(!(checkCorrectness(enter, "eng")))
            {
                QMessageBox::warning(this, "Error", "In the line must be " +version+ " word");
                continue;
            }
        }
        else if(version=="Russian")
        {
            std::map<QString, QString> temp = list.toStdMap();
            bool have = false;
            for(std::map<QString, QString>::iterator iter = temp.begin(); iter != temp.end(); iter++)
                if(iter->second == enter)
                {
                    QMessageBox::information(this, "Info", "Your base has this word");
                    have = true;
                    break;
                }

            if(have)
                continue;

            if(!(checkCorrectness(enter, "rus")))
            {
                QMessageBox::warning(this, "Error", "In the line must be " +version+ " word");
                continue;
            }
        }

        return enter;
    }
}

void wordsMode::on_pushButtonChange_clicked()
{
    if(ui->listWidget->selectedItems().size() == 0)
    {
        QMessageBox::information(this, "Info", "Word is not selected");
        return;
    }

    QString eng = ui->listWidget->currentItem()->text();
    QString rus = list[ui->listWidget->currentItem()->text()];
    QString original = eng + '_' + rus + '\n';
    QString oldEng = eng;

    eng = changeWord(eng, "English");
    if(eng=="cancel")
        return;
    rus = changeWord(rus, "Russian");
    if(rus=="cancel")
        return;
    const QString newWord = eng + '_' + rus + '\n';

    QFile words("mfwd.dll");
    auto move = toVectorFromFile("mfwd.dll");
    if(words.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        move[ui->listWidget->currentRow()] = newWord;
        for(auto word : move)
            words.write(word.toUtf8());
        words.close();
    }

    QFile cardWords("mfwk.dll");
    if(cardWords.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString all = cardWords.readAll();
        if(all.contains(original))
        {
            cardWords.close();
            QString move;
            if(cardWords.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                while(!cardWords.atEnd())
                {
                QString line = cardWords.readLine();

                if(line == '#'+original)
                    line = '#'+newWord;             // change the translation for a card
                else if(line.contains(oldEng))
                    line.replace(oldEng, eng);      // change the word combination line

                move += line;
                }
                cardWords.close();
            }
            if(cardWords.open(QIODevice::WriteOnly | QIODevice::Text))
                cardWords.write(move.toUtf8());
        }
        cardWords.close();
    }

    QFile recommends("mfchit.dll");
    if(recommends.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        auto move = toVectorFromFile("mfchit.dll");
        if(move.contains(original))
        {
            recommends.close();
            if(recommends.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                move[move.indexOf(original)] = newWord;
                for(auto word : move)
                    recommends.write(word.toUtf8());
            }
        }
        recommends.close();
    }

    refreshList();
    showResult("color: rgb(255, 191, 0);", "Word Changed");
}

void wordsMode::on_pushButtonTranslator_clicked()
{
    QDesktopServices::openUrl(QUrl("https://translate.google.com/"));
}

void wordsMode::on_pushButtonSetImg_clicked()
{
    setImage(this, QSize(1920,1080), "mfimgw.dll");     // 1920x1080 because this resolution will be truncated later
}

void wordsMode::on_pushButtonRunNote_clicked()
{
    this->close();

    QWidget* win =      new QWidget;
    QLineEdit* line =   new QLineEdit(win);
    QPushButton* exit = new QPushButton(win);

    win->   setWindowTitle("NoteLine");
    win->   setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    win->   setMinimumSize(195, 35);
    win->   setMaximumSize(195, 35);
    line->  setGeometry(5,5, 155, 25);
    exit->  setGeometry(165, 5, 25, 25);

    win->   setStyleSheet("QWidget{background-color: rgb(0, 85, 127);}");
    line->  setStyleSheet("font: 14pt Comic Sans MS; color: rgb(255, 255, 0); background-color: rgb(0, 85, 127);");
    exit->  setStyleSheet("QPushButton::hover{background-color: rgb(0, 102, 204)}");

    line->  setFrame(false);
    exit->  setIcon(QPixmap(":/img/images/cross.png"));
    exit->  setIconSize(QSize(20,20));
    exit->  setText("");
    win->   show();

    connect(line, &QLineEdit::returnPressed, [line]()
    {
        if(line->text().size()==0);
        else if(!checkCorrectness(line->text(), "eng"));
        else
        {
            QFile file("mfcrib.dll");
            if(file.open(QIODevice::Append | QIODevice::Text))
            {
                file.write(line->text().toUtf8() + '\n');
                file.close();
            }
            line->setText("");
        }
    });

    connect(exit , &QPushButton::clicked, win, [win]()
    {
        delete win;
    });
}

void wordsMode::on_pushButtonCrib_clicked()
{
    ui->pushButtonCrib->setEnabled(false);
    QWidget* crib =         new QWidget(this);
    QPushButton* del =      new QPushButton(crib);
    QPushButton* close =    new QPushButton(crib);
    QListWidget* list =     new QListWidget(crib);

    crib->  setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
    crib->  setGeometry(this->x() + this->width() + 5, this->y() + 7, 200, 320);
    crib->  setMinimumSize(200, 320);
    crib->  setMaximumSize(200, 320);
    del->   setGeometry(3,3,171,20);
    close-> setGeometry(177, 3, 20, 20);
    list->  setGeometry(3,26,194,291);

    crib->  setStyleSheet("QWidget{background-color: rgb(0, 85, 127);}");
    del->   setStyleSheet("QPushButton{font: 13pt Comic Sans MS; background-color: rgb(0, 85, 127); color: rgb(254, 62, 57);}"
                          "QPushButton::hover{background-color: rgb(0, 102, 204)}");
    close-> setStyleSheet("QPushButton::hover{background-color: rgb(0, 102, 204)}");
    list->  setStyleSheet("QListWidget{ font: 14pt MS Shell Dlg 2; background-color: rgb(255, 153, 51); color: rgb(51, 153, 51);}"
                          "QListWidget::item::selected{ font: 14pt MS Shell Dlg 2; color: rgb(0, 51, 204); background-color: rgb(51, 153, 255, 150); }");

    auto words = toVectorFromFile("mfcrib.dll");
    for(auto word : words)
    {
        word.remove('\n');
        list->addItem(word);
    }

    del->   setText("delete the word");
    close-> setText("");
    close-> setIcon(QPixmap(":/img/images/cross.png"));
    close-> setIconSize(QSize(17,17));
    crib->  show();

    connect(del, &QPushButton::clicked, crib, [this, list]()
    {
        if(list->selectedItems().size() == 0)
            QMessageBox::information(this, "Info", "Word is not selected");
        else
        {
            QVector<QString> temp;
            for(int i = 0, s = list->count(); i < s; i++)
                temp.push_back(list->item(i)->text());

            temp.remove(list->currentRow());
            list->clear();
            for(auto word : temp)
                list->addItem(word);

            QFile file("mfcrib.dll");
            if(file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                for(auto word : temp)
                    file.write(word.toUtf8()+'\n');
                file.close();
            }
        }
    });

    connect(close, &QPushButton::clicked, crib, [this, crib]()
    {
        ui->pushButtonCrib->setEnabled(true);
        delete crib;
    });
}
