#include "trainingmode.h"
#include "ui_trainingmode.h"

trainingMode::trainingMode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::trainingMode)
{
    ui->setupUi(this);
    //----------------

    setWindowTitle("Training Mode");
    setWindowFlags(Qt::Window);

    if(QFile("mfimgt.dll").exists())
        this->setStyleSheet("QDialog{background-image: url(mfimgt.dll)}"
                            "QMessageBox{background-color: rgb(0, 85, 127); font: 14pt Comic Sans MS; background-image: url(0);}"
                            "QLabel{color: rgb(255, 255, 0)}");
    else
        this->setStyleSheet("QDialog{background-color: rgb(0, 85, 127)}"
                            "QMessageBox{background-color: rgb(0, 85, 127); font: 14pt Comic Sans MS; background-image: url(0);}"
                            "QLabel{color: rgb(255, 255, 0)}");

    ui->labelCountWords->       setText(QString::number(count=0));
    ui->lineEditCheck->         setVisible(false);
    ui->pushButtonCheck->       setEnabled(false);
    ui->pushButtonIDontKnow->   setEnabled(false);
    ui->pushButtonRepeat->      setEnabled(false);
    ui->pushButtonSkip->        setEnabled(false);
    ui->pushButtonStartOver->   setEnabled(false);
    ui->radioButtonEndAndRus->  setEnabled(false);
    ui->radioButtonEng->        setEnabled(false);
    ui->radioButtonRus->        setEnabled(false);

    QPushButton* start = new QPushButton(this);
    start->setGeometry(ui->lineEditCheck->geometry());
    start->setStyleSheet("QPushButton{background-color: rgba(0, 0, 127, 150); color: rgb(0, 210, 255); font: 25pt Comic Sans MS; font-weight: bold}\n"
                         "QPushButton::hover{color: rgb(255, 51, 0)}");
    start->setText("Start Training!");
    start->setVisible(true);

    connect(start, &QPushButton::clicked, this, [=]()
    {
        ui->pushButtonCheck->       setEnabled(true);
        ui->pushButtonIDontKnow->   setEnabled(true);
        ui->pushButtonRepeat->      setEnabled(true);
        ui->pushButtonSkip->        setEnabled(true);
        ui->pushButtonStartOver->   setEnabled(true);
        ui->radioButtonEndAndRus->  setEnabled(true);
        ui->radioButtonEng->        setEnabled(true);
        ui->radioButtonRus->        setEnabled(true);
        ui->pushButtonCheck->       setDefault(true);
        ui->lineEditCheck->         setVisible(true);

        sizeOfBase*=2;      // 1 word with translation = 1 eng word + 1 rus word
        ui->labelEntireNumber->setText('/' + QString::number(sizeOfBase));
        timer->start(1000);
        nextWord();
        delete start;
    });

    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    QLabel* l[3] = { ui->labelCountText, ui->labelCountWords, ui->labelEntireNumber };
    for(int i=0; i<3; i++)
        l[i]->setToolTip("If the number of words you've translated is bigger then you entered improper\n"
                         "translation sometimes, but if smaller then you skipped some words or selected\n"
                         "an only-language point. If the number is equal to general amount of words then\n"
                         "you didn't have any mistakes and didn't use an only-language point.");
}

trainingMode::~trainingMode()
{
    delete ui;
}

void trainingMode::nextWord()
{
    if(ui->radioButtonEndAndRus->isChecked() && dontRepeatBase.size() == wordBase.size()*2)
        refreshTraining();
    else if(ui->radioButtonEng->isChecked() && amountOfPulled("eng") == wordBase.size())
        refreshTraining();
    else if(ui->radioButtonRus->isChecked() && amountOfPulled("rus") == wordBase.size())
        refreshTraining();

    QString show;
    srand(time(0));

    try     // more details in the message text in the catch
    {
        unsigned int critical = 0;
        while(true)
        {
            if(++critical > 15000)
                throw exceptionToLoop();

            QString word = wordBase[rand() % wordBase.size()];
            QString language = "";

            if(ui->radioButtonEndAndRus->isChecked())
                switch(rand() % 2 + 1)
                {
                case 1:
                    getWordsSeparately(word, show, checkEnter);
                    if(checkEnter.contains('('))
                        checkEnter.truncate(checkEnter.indexOf('('));
                    break;
                case 2:
                    getWordsSeparately(word, checkEnter, show);
                    break;
                }

            else if(ui->radioButtonEng->isChecked())
            {
                language = "eng";
                getWordsSeparately(word, show, checkEnter);
                if(checkEnter.contains('('))
                    checkEnter.truncate(checkEnter.indexOf('('));   // remove clarification
            }

            else if(ui->radioButtonRus->isChecked())
            {
                language = "rus";
                getWordsSeparately(word, checkEnter, show);
            }

            // (about the right part after "||") If current word which should be shown is equal to word which should not be shown right away AND
            // if this word is not last in whole word base AND if this word is not last from only-language words, then this condition is TRUE, then we can't show this word.
            // But if the word is last in whole word base or from only-language words then this condition is FALSE and get skipped
            // because last word can repeating after "repeat" or "show translation" is clicked.
            if(dontRepeatBase.contains(show) || (dontRepeatNow == show && dontRepeatBase.size() != wordBase.size()*2-1 && amountOfPulled(language) != wordBase.size()-1))
                continue;

            break;
        }
    }
    catch(const exceptionToLoop&)
    {
        QMessageBox::critical(this, "Critical Error", "Error with exception has occurred. Possibly you have two or more the same word that should not repeating. "
                                                      "Please check presence of one or more copies of this word. If there are copies on the cards or in the base then delete them.");
        this->close();
    }

    dontRepeatBase.push_back(show);
    dontRepeatNow = "";

    clarifyFontSize(show);
    ui->labelWord->setText(show);
}

void trainingMode::updateTime()
{
    if(++s > 59)
    {
        m++;
        s = 0;
    }
    if(s<10 && m<10)
        ui->labelTime->setText("Time: 0"+QString::number(m)+":0"+QString::number(s));
    else if(m<10)
        ui->labelTime->setText("Time: 0"+QString::number(m)+":"+QString::number(s));
    else if(s<10)
        ui->labelTime->setText("Time: "+QString::number(m)+":0"+QString::number(s));
    else
        ui->labelTime->setText("Time: "+QString::number(m)+":"+QString::number(s));
}

void trainingMode::clarifyFontSize(QString& show)
{
    auto separateAndSetSize = [this, &show](const int size)
    {
        int a = show.indexOf(',');
        int b = show.indexOf(',', a+1);

        QString left = show;
        QString right = show;
        left.truncate(b);
        right.remove(0, b+1);

        if(show[b+1] == ' ')
            show.remove(b+1, 1);
        show.insert(b+1, '\n');

        if(left.size() > 23 || right.size() > 23)
            ui->labelWord->setFont(QFont("Comic Sans MS", size-5, QFont::Bold));
        else
            ui->labelWord->setFont(QFont("Comic Sans MS", size, QFont::Bold));
    };

    if(show.size() > 35)
    {
        if(show.count(',') >= 2)
            separateAndSetSize(20);
        else
            ui->labelWord->setFont(QFont("Comic Sans MS", 15, QFont::Bold));
    }
    else if(show.size() > 23)
    {
        if(show.count(',') == 1)
        {
            int a = show.indexOf(',');
            if(show[a+1] == ' ')
                show.remove(a+1, 1);
            show.insert(a+1, '\n');
            ui->labelWord->setFont(QFont("Comic Sans MS", 25, QFont::Bold));
        }
        else if(show.count(',') >= 2)
            separateAndSetSize(25);
        else
            ui->labelWord->setFont(QFont("Comic Sans MS", 17, QFont::Bold));
    }
    else if(show.size() > 16)
        ui->labelWord->setFont(QFont("Comic Sans MS", 20, QFont::Bold));
    else
        ui->labelWord->setFont(QFont("Comic Sans MS", 25, QFont::Bold));
}

int trainingMode::amountOfPulled(const QString lang)
{
    int k = 0;
    if(lang == "eng")
    {
        for(QString have : dontRepeatBase)
            if(have[1] >= 'A' && have[1] <= 'z')    // check second element because user can mistake inputted first letter as gap
                k++;
    }
    else if(lang == "rus")
        for(QString have : dontRepeatBase)
            if(have.contains(QRegularExpression("[А-Яа-я]")) || have.contains(' ') || have.contains('('))
                k++;
    return k;
}

void trainingMode::refreshTraining()
{
    timer->stop();
    QMessageBox::information(this, "Info", "You've repeated all of words.\nReset of training occurred.\nTime passed: "+QString::number(m)+" m "+QString::number(s)+" s.");
    ui->labelCountWords->setText(QString::number(count=0));
    ui->labelTime->setText("Time: 00:00");
    s = 0;
    m = 0;
    dontRepeatBase.clear();
    timer->start();
}

void trainingMode::on_pushButtonCheck_clicked()
{
    if(ui->lineEditCheck->text().size() == 0)
    {
        QMessageBox::information(this, "Info", "This line is empty");
        return;
    }

    auto improperly = [this](bool message)
    {
        if(message)
            QMessageBox::information(this, "Info", "Improperly!");

        if(dontRepeatNow.size() == 0)    // if this message will emerge one more time then don't repeat actions with vector because notRepeatNow-word is ready
        {
            dontRepeatNow = dontRepeatBase.last();
            dontRepeatBase.pop_back();
        }
    };

    auto properly = [this]()
    {
        QMessageBox::information(this, "Info", "Properly!");
        ui->labelCountWords->setText(QString::number(++count));
        ui->lineEditCheck->setText("");
        checkEnter = "";
        nextWord();
    };

    if(checkEnter.contains(","))
    {
        const int n = checkEnter.count(',') + 1;    // the amount of words in the translation
        QVector<QString> word;

        for(int i = 0, letter = 0; i < n; i++)
        {
            QString temp;

            while(checkEnter[letter] != ',' && checkEnter[letter] != nullptr)
                temp += checkEnter[letter++];
            letter++;

            if(checkEnter[letter] == ' ')
                letter++;

            word.push_back(temp);
        }

        int forgot = 0;
        QString temp = ui->lineEditCheck->text();
        for(int i = 0; i < n; i++)
            if(!(ui->lineEditCheck->text().contains(word[i])))
                forgot++;
            else
                temp.remove(word[i]);

        if(forgot == n)
        {
            improperly(true);
            return;
        }
        if(forgot == 1)
        {
            QMessageBox::information(this, "Info", "You forgot to write 1 more word.\nEither you've entered the wrong\ntranslation for it.");
            improperly(false);
            return;
        }
        if(forgot > 0)
        {
            QMessageBox::information(this, "Info", "You forgot to write " + QString::number(forgot) + " more words.\nEither you've entered the wrong\ntranslations for them.");
            improperly(false);
            return;
        }

        if(temp.contains(QRegularExpression("[^ ,]")))      // if the correct words is entered but the line has something else beside gaps and commas then wrong
        {
            QMessageBox::information(this, "Info", "Wrong. You've entered something extra.");
            improperly(false);
            return;
        }

        properly();
        return;
    }

    if(checkEnter == ui->lineEditCheck->text())
        properly();
    else
        improperly(true);
}

void trainingMode::on_pushButtonRepeat_clicked()
{
    dontRepeatNow = dontRepeatBase.last();
    dontRepeatBase.pop_back();
    checkEnter = "";
    nextWord();
}

void trainingMode::on_pushButtonIDontKnow_clicked()
{
    QMessageBox win(this);
    win.setIcon(QMessageBox::Information);
    win.setWindowTitle("Translation");
    win.setStyleSheet("QLabel{background-color: rgba(255, 0, 0, 100); color: rgb(0, 255, 0); font: 30pt Comic Sans MS; qproperty-alignment: AlignCenter}"
                      "QPushButton{background-color: rgba(255, 0, 0, 100); color: rgb(0, 255, 0); font: 15pt Comic Sans MS;}"
                      "QPushButton::hover{background-color: rgba(255, 0, 0, 150)}");
    win.setText(checkEnter);
    win.addButton("OK", QMessageBox::YesRole);
    win.exec();

    if(dontRepeatNow.size() == 0)    // here check is need because this button can be pressed after message "improperly" and thus must not repeat actions with vector
    {
        dontRepeatNow = dontRepeatBase.last();
        dontRepeatBase.pop_back();
    }

    ui->lineEditCheck->setText("");
    checkEnter = "";
    nextWord();
}

void trainingMode::on_pushButtonSkip_clicked()
{
    ui->lineEditCheck->setText("");
    checkEnter = "";
    nextWord();
}

void trainingMode::on_pushButtonStartOver_clicked()
{
    timer->stop();
    QMessageBox::StandardButton answer = QMessageBox::question(this, "Start Over", "Are you sure? Non-repeating words\nwill be reseted.", QMessageBox::Yes | QMessageBox::No);
    if(answer == QMessageBox::Yes)
    {
        ui->labelCountWords->setText(QString::number(count=0));
        ui->lineEditCheck->setText("");
        dontRepeatBase.clear();
        nextWord();

        ui->labelTime->setText("Time: 00:00");
        s = 0;
        m = 0;
    }
    timer->start();
}

void trainingMode::on_pushButtonSetImg_clicked()
{
    timer->stop();

    setImage(this, QSize(960,540),"mfimgt.dll");

    if(ui->labelWord->text() != "press it -->")
        timer->start();
}
