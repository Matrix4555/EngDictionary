#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //----------------

    const QDesktopWidget* desk = new QDesktopWidget;
    QRect* square = new QRect;
    *square = desk->availableGeometry();
    delete desk;
    this->move(square->x()-8, square->y());
    delete square;

    ui->pushButtonInfo->setStyleSheet("QPushButton{background-color: rgb(255,255,255,0);} QPushButton::hover{background-color: rgb(153, 204, 255, 70);}");
    ui->pushButtonInfo->setIcon(QPixmap(":/img/images/info.png"));
    ui->pushButtonInfo->setIconSize(QSize(30, 30));

    connect(ui->pushButtonMondayChange,     SIGNAL(clicked()), this, SLOT(changeCard()));
    connect(ui->pushButtonTuesdayChange,    SIGNAL(clicked()), this, SLOT(changeCard()));
    connect(ui->pushButtonWednesdayChange,  SIGNAL(clicked()), this, SLOT(changeCard()));
    connect(ui->pushButtonThursdayChange,   SIGNAL(clicked()), this, SLOT(changeCard()));
    connect(ui->pushButtonFridayChange,     SIGNAL(clicked()), this, SLOT(changeCard()));
    connect(ui->pushButtonSaturdayChange,   SIGNAL(clicked()), this, SLOT(changeCard()));
    connect(ui->pushButtonSundayChange,     SIGNAL(clicked()), this, SLOT(changeCard()));

    QLabel* card[2][7] = { {    ui->labelMonday,    ui->labelTuesday,   ui->labelWednesday, ui->labelThursday,  ui->labelFriday,    ui->labelSaturday,  ui->labelSunday},
                           {    ui->labelMon,       ui->labelTue,       ui->labelWed,       ui->labelThr,       ui->labelFri,       ui->labelSat,       ui->labelSun } };

    QPushButton* button[13]= { ui->pushButtonMondayChange,  ui->pushButtonTuesdayChange,    ui->pushButtonWednesdayChange,  ui->pushButtonThursdayChange,
                               ui->pushButtonFridayChange,  ui->pushButtonSaturdayChange,   ui->pushButtonSundayChange,     ui->pushButtonWords,
                               ui->pushButtonMenuColor,     ui->pushButtonSetImg,           ui->pushButtonTraining,         ui->pushButtonCheckKnow,
                               ui->pushButtonGetCards };

    if(QFile("mfimgc.dll").exists())
        this->setStyleSheet("QMainWindow{background-image: url(mfimgc.dll)}"
                            "QMessageBox{background-color: rgb(0, 85, 127); font: 14pt Comic Sans MS;}"
                            "QLabel{color: rgb(255, 255, 0)}");
    else
        this->setStyleSheet("QMainWindow{background-image: url(:/img/images/defaultpicture.png)}"
                            "QMessageBox{background-color: rgb(0, 85, 127); font: 14pt Comic Sans MS;}"
                            "QLabel{color: rgb(255, 255, 0)}");

    QFile stsh("mfstsh.dll");
    if(!stsh.exists() && stsh.open(QIODevice::WriteOnly | QIODevice::Text))     // default style sheet if the file doesn't exist
    {
        stsh.write("1color: rgb(255,255,0);\n1background-color: rgb(0,0,127,150);\n2color: rgb(255,255,0);\n2background-color: rgb(0,0,127,150);\n"
                   "3color: rgb(255,255,0);\n3background-color: rgb(0,0,127,150);\n4color: rgb(255,255,0);\n4background-color: rgb(0,0,127,150);\n"
                   "5color: rgb(255,255,0);\n5background-color: rgb(0,0,127,150);\n6color: rgb(255,255,0);\n6background-color: rgb(0,0,127,150);\n"
                   "7color: rgb(255,255,0);\n7background-color: rgb(0,0,127,150);\n#color: rgb(255,255,0);\n#background-color: rgb(0,0,127,150);\n");
        stsh.close();
    }
    if(stsh.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        int i = 0;
        QString a, b;

        for(; i < 7; i++)
        {
            a = stsh.readLine().remove(0,1);    // get color
            b = stsh.readLine().remove(0,1);    // get background-color
            card[0][i]->setStyleSheet(a+b);     // set for the card
            card[1][i]->setStyleSheet(a);       // set only color for the name of week day under the card

            button[i]->setStyleSheet(styleSheetForButton(a+b));
        }

        a = stsh.readLine().remove(0,1);        // get menu color
        b = stsh.readLine().remove(0,1);        // get menu background-color
        QString* hoverColor = new QString;
        *hoverColor = styleSheetForButton(a+b);

        for(; i < 13; i++)
            button[i]->setStyleSheet(*hoverColor);
        delete hoverColor;

        ui->labelPrompt->setStyleSheet(a);
        stsh.close();
    }

    QFile cardWords("mfwk.dll");
    if(!cardWords.exists() && cardWords.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        cardWords.write("1\n2\n3\n4\n5\n6\n7\n*");      // default text for the file if it doesn't exist
        cardWords.close();
    }

    setTextToCards(card[0]);
}

MainWindow::~MainWindow()
{
    delete ui;
    //--------

    QString reserve;

    QFile wd("mfwd.dll"), wk("mfwk.dll");
    if(wd.open(QIODevice::ReadOnly | QIODevice::Text) && wk.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        reserve = wd.readAll()+'\n';
        reserve += wk.readAll();
        wd.close();
        wk.close();
    }

    QFile reserveFile("mfres.dll");
    if(reserveFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        reserveFile.write(reserve.toUtf8());
        reserveFile.close();
    }
}

void MainWindow::changeCard()
{
    QPushButton* button = (QPushButton*)sender();
    QLabel* card;
    QLabel* weekday;
    int day;
    clarifyDay(button, card, weekday, day);

    QWidget* cardSettings =             new QWidget(this);
    QListWidget* recommendations =      new QListWidget(this);
    QTextEdit* input =                  new QTextEdit(this);
    QPushButton* settingButton[4] = {   new QPushButton(cardSettings), new QPushButton(cardSettings),
                                        new QPushButton(cardSettings), new QPushButton(cardSettings) };

    cardSettings->setGeometry(card->x(), button->y(), card->width(), button->height());

    settingButton[0]->setText("Cancel");
    settingButton[1]->setText("Background");
    settingButton[2]->setText("Color");
    settingButton[3]->setText("Apply");

    for(int i=0, s=0; i<4 ; i++, s+=115)
    {
        settingButton[i]->setGeometry(0+s, 0, 110, button->height());
        settingButton[i]->setStyleSheet(styleSheetForButton(card->styleSheet(), " font: 15pt Comic Sans MS; "));
        settingButton[i]->setVisible(true);
    }

    // 3 lambdas for setting-buttons:
    //-----------------------------------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------------------------------

    auto closeEditor = [=]()    // "cancel" button
    {
        delete cardSettings;
        delete recommendations;
        delete input;

        setEnabledButtons(true);

        weekday->   setVisible(true);
        card->      setVisible(true);
        button->    setVisible(true);
    };

    //-----------------------------------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------------------------------

    auto changeColor = [=]()    // "color" and "background" buttons
    {
        QColorDialog select(this);
        select.setWindowTitle("Set New Color");
        select.setOption(QColorDialog::ShowAlphaChannel);
        select.setStyleSheet("QColorDialog{background-color: rgb(0, 85, 127);}"
                             "QLabel{color: rgb(255,255,0); font: 13pt Comic Sans MS;}");

        if(select.exec() == 0)
            return;

        QColor color = select.selectedColor();
        int r, g, b, a;
        color.getRgb(&r, &g, &b, &a);

        QFile file("mfstsh.dll");
        QString move;
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            if((QPushButton*)sender() == settingButton[1])
            {
                QString newColor = "background-color: rgb("+QString::number(r)+","+QString::number(g)+","+QString::number(b)+","+QString::number(a)+");\n";

                while(!file.atEnd())
                {
                    QString line = file.readLine();
                    if(line[0] == QString::number(day))
                    {
                        move += line + QString::number(day) + newColor;     // move old color and new background
                        file.readLine();                                    // discard old color
                        card->setStyleSheet(line.remove(0,1) + newColor);   // set to current card new background
                        input->setStyleSheet(card->styleSheet());
                        button->setStyleSheet(styleSheetForButton(line+newColor));

                        QString temp = styleSheetForButton(line+newColor, " font: 15pt Comic Sans MS; ");
                        for(int i=0;i<4;i++)
                            settingButton[i]->setStyleSheet(temp);
                    }
                    else
                        move+=line;
                }
            }
            else if((QPushButton*)sender() == settingButton[2])
            {
                QString newColor = "color: rgb("+QString::number(r)+","+QString::number(g)+","+QString::number(b)+","+QString::number(a)+");\n";

                while(!file.atEnd())
                {
                    QString line = file.readLine();
                    if(line[0] == QString::number(day))
                    {
                        line = file.readLine();                             // catch old background
                        move += QString::number(day) + newColor + line;     // move new color and old background
                        card->setStyleSheet(newColor + line.remove(0,1));   // set to current card new color
                        weekday->setStyleSheet(newColor);
                        input->setStyleSheet(card->styleSheet());
                        button->setStyleSheet(styleSheetForButton(newColor+line));

                        QString temp = styleSheetForButton(newColor+line, " font: 15pt Comic Sans MS; ");
                        for(int i=0;i<4;i++)
                            settingButton[i]->setStyleSheet(temp);
                    }
                    else
                        move+=line;
                }
            }
            file.close();
        }
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            file.write(move.toUtf8());
            file.close();
        }
    };

    //-----------------------------------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------------------------------

    auto applyChanges = [=]()   // "apply" button
    {
        const QString inputted = input->toPlainText();

        if(inputted.size() == 0 && card->text().size() != 0)
        {
            QMessageBox mes(this);
            mes.setIcon(QMessageBox::Question);
            mes.setWindowTitle("Erase");
            mes.setText("Do you want to clear the card?");
            QAbstractButton* ans = mes.addButton("Yes", QMessageBox::YesRole);
            mes.addButton("No", QMessageBox::NoRole);
            mes.exec();

            if(mes.clickedButton() == ans)
            {
                card->setText(inputted);    // set empty text
                QFile file("mfwk.dll");
                QString move;
                if(file.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    while(!file.atEnd())
                    {
                        QString line = file.readLine();
                        if(line == QString::number(day)+'\n')
                        {
                            move+=line;
                            if(day!=7)
                            {
                                while(file.readLine() != QString::number(day+1)+'\n');
                                line = QString::number(day+1)+'\n';
                            }
                            else
                            {
                                while(file.readLine() != "*");
                                line = "*";
                            }
                        }
                        move+=line;
                    }
                    file.close();
                }
                if(file.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    file.write(move.toUtf8());
                    file.close();
                }
                closeEditor();
                return;
            }
            else
            {
                closeEditor();
                return;
            }
        }
        if(inputted.count('\n') > 5)
        {
            QMessageBox::warning(this, "Info", "There should be no more six lines");
            return;
        }
        if(!checkCorrectness(inputted, "eng"))
        {
            QMessageBox::warning(this, "Error", "On the card must be English words only");
            return;
        }

        input->hide();
        card->setVisible(true);
        QString* oldWords = new QString;
        *oldWords = card->text();
        card->setText(inputted);
        auto translationsForCard = getTranslations(inputted, card, day);

        if(translationsForCard == "cancel")
        {
            input->show();
            card->setVisible(false);
            card->setText(*oldWords);
            delete oldWords;
            return;
        }
        delete oldWords;

        auto currentRecom = toVectorFromFile("mfchit.dll");
        for(int i = 0; i < currentRecom.size(); i++)
            if(translationsForCard.contains('#'+currentRecom[i]))    // if a card has a word from recommendations then need to delete it from there
                currentRecom.remove(i--);                            // (recommendations show those words which didn't be on cards ever)

        QFile newRecommendations("mfchit.dll");
        if(newRecommendations.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            for(auto temp : currentRecom)
                newRecommendations.write(temp.toUtf8());
            newRecommendations.close();
        }

        QFile weekWords("mfwk.dll");
        QString move;
        if(weekWords.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            while(!weekWords.atEnd())
            {
                QString line = weekWords.readLine();
                if(line == QString::number(day) + '\n')
                {
                    move += line + translationsForCard + inputted + '\n';
                    while(line != QString::number(day+1) + '\n' && line != "*")
                        line = weekWords.readLine();
                    move += line;
                    continue;
                }
                move+=line;
            }
            weekWords.close();
        }
        if(weekWords.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            weekWords.write(move.toUtf8());
            weekWords.close();
        }

        closeEditor();
    };

    //-----------------------------------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------------------------------------

    connect(settingButton[0], &QPushButton::clicked, this, closeEditor);
    connect(settingButton[1], &QPushButton::clicked, this, changeColor);
    connect(settingButton[2], &QPushButton::clicked, this, changeColor);
    connect(settingButton[3], &QPushButton::clicked, this, applyChanges);

    setEnabledButtons(false);

    weekday->       setVisible(false);
    card->          setVisible(false);
    button->        setVisible(false);
    cardSettings->  setVisible(true);

    input->setGeometry(card->geometry());
    input->setStyleSheet(card->styleSheet() + "selection-background-color: green;");
    input->setFont(card->font());
    input->setText(card->text());

    QTextCursor c = input->textCursor();    // text will get highlighted immediately
    c.select(QTextCursor::Document);
    input->setTextCursor(c);

    input->setAlignment(Qt::AlignCenter);
    input->setFocus();
    input->setVisible(true);

    QFile menuStSh("mfstsh.dll");
    QString* menucolor = new QString;
    if(menuStSh.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        *menucolor = menuStSh.readLine();
        while((*menucolor)[0] != '#')
            *menucolor = menuStSh.readLine();
        menucolor->remove(0,1);
        *menucolor += QString(menuStSh.readLine()).remove(0,1);
        menuStSh.close();
    }

    recommendations->setGeometry(ui->labelMonday->x(), ui->labelFriday->y(), 230, 465);
    recommendations->setStyleSheet("QListWidget{"+ *menucolor + "font: 12pt Comic Sans MS;} QListWidget::item::selected{" + *menucolor + "border: 1px solid red;}");
    recommendations->setFocusPolicy(Qt::NoFocus);
    recommendations->setWordWrap(true);
    delete menucolor;

    QFile recomFile("mfchit.dll");
    if(recomFile.size() > 0 && recomFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        unsigned int n = 0;
        while(!recomFile.atEnd())
        {
            QString whole = recomFile.readLine();
            QString eng, rus;
            getWordsSeparately(whole, eng, rus);
            whole = eng + " - " + rus;
            recommendations->addItem(whole);
            recommendations->item(n++)->setTextAlignment(Qt::AlignCenter);
        }
        recomFile.close();
    }
    else
    {
        recommendations->addItem("No recommendations.\nAll of words from base\nhave been on the cards.");
        recommendations->item(0)->setTextAlignment(Qt::AlignCenter);
    }

    recommendations->setVisible(true);
}

QString MainWindow::getTranslations(const QString text, const QLabel* card, const int number)
{
    auto base = toVectorFromFile("mfwd.dll");
    QString translations, check;
    bool merge = false;

    QString cardWords;
    QFile file("mfwk.dll");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!file.atEnd())
        {
            QString line = file.readLine();
            if(line == QString::number(number) + '\n')
                while(line != QString::number(number+1) + '\n' && line != "*")
                    line = file.readLine();
            if(line[0]=='#')
            {
                line.truncate(line.indexOf('_'));
                cardWords+=line;
            }
        }
        cardWords+='#';
        file.close();
    }

    for(int i = 0; i <= text.size(); i++)
    {
        if(!merge && (i==0 || text[i-1]==' ' || text[i-1]=='\n'))
        {
            if((i+1<text.size() && (text[i+1]==' ' || text[i+1]=='\n')) || i+1==text.size())
                if(text[i]=='a')                                                                // skip "a"
                { i++; continue; }
            if((i+2<text.size() && (text[i+2]==' ' || text[i+2]=='\n')) || i+2==text.size())
                if((text[i]=='a' && text[i+1]=='n') ||                                          // skip "an"
                        (text[i]=='o' && text[i+1]=='f') ||                                     // skip "of"
                        (text[i]=='i' && text[i+1]=='s') ||                                     // skip "is"
                        (text[i]=='i' && text[i+1]=='n') ||                                     // skip "in"
                        (text[i]=='o' && text[i+1]=='n') ||                                     // skip "on"
                        (text[i]=='b' && text[i+1]=='y') ||                                     // skip "by"
                        (text[i]=='t' && text[i+1]=='o') ||                                     // skip "to"
                        (text[i]=='a' && text[i+1]=='t') ||                                     // skip "at"
                        (text[i]=='o' && text[i+1]=='r'))                                       // skip "or"
                { i+=2; continue; }
            if((i+3<text.size() && (text[i+3]==' ' || text[i+3]=='\n')) || i+3==text.size())
                if((text[i]=='t' && text[i+1]=='h' && text[i+2]=='e') ||                        // skip "the"
                        (text[i]=='f' && text[i+1]=='o' && text[i+2]=='r') ||                   // skip "for"
                        (text[i]=='a' && text[i+1]=='r' && text[i+2]=='e') ||                   // skip "are"
                        (text[i]=='d' && text[i+1]=='u' && text[i+2]=='e') ||                   // skip "due"
                        (text[i]=='a' && text[i+1]=='n' && text[i+2]=='d'))                     // skip "and"
                { i+=3; continue; }
            if((i+4<text.size() && (text[i+4]==' ' || text[i+4]=='\n')) || i+4==text.size())
                if((text[i]=='w' && text[i+1]=='i' && text[i+2]=='t' && text[i+3]=='h') ||              // skip "with"
                        (text[i]=='n' && text[i+1]=='e' && text[i+2]=='a' && text[i+3]=='r') ||         // skip "near"
                        (text[i]=='l' && text[i+1]=='i' && text[i+2]=='k' && text[i+3]=='e') ||         // skip "like"
                        (text[i]=='f' && text[i+1]=='r' && text[i+2]=='o' && text[i+3]=='m') ||         // skip "from"
                        (text[i]=='s' && text[i+1]=='a' && text[i+2]=='k' && text[i+3]=='e') ||         // skip "sake"
                        (text[i]=='i' && text[i+1]=='n' && text[i+2]=='t' && text[i+3]=='o') ||         // skip "into"
                        (text[i]=='i' && text[i+1]=='t' && text[i+2]=='\'' && text[i+3]=='s'))          // skip "it's"
                { i+=4; continue; }
            if((i+5<text.size() && (text[i+5]==' ' || text[i+5]=='\n')) || i+5==text.size())
                if((text[i]=='i' && text[i+1]=='t' && text[i+2]==' ' && text[i+3]=='i' && text[i+4]=='s') ||            // skip "it is"
                        (text[i]=='a' && text[i+1]=='b' && text[i+2]=='o' && text[i+3]=='u' && text[i+4]=='t') ||       // skip "about"
                        (text[i]=='s' && text[i+1]=='i' && text[i+2]=='n' && text[i+3]=='c' && text[i+4]=='e') ||       // skip "since"
                        (text[i]=='u' && text[i+1]=='n' && text[i+2]=='d' && text[i+3]=='e' && text[i+4]=='r') ||       // skip "under"
                        (text[i]=='w' && text[i+1]=='h' && text[i+2]=='i' && text[i+3]=='l' && text[i+4]=='e'))         // skip "while"
                { i+=5; continue; }
            if((i+6<text.size() && (text[i+6]==' ' || text[i+6]=='\n')) || i+6==text.size())
                if((text[i]=='d' && text[i+1]=='u' && text[i+2]=='r' && text[i+3]=='i' && text[i+4]=='n' && text[i+5]=='g') ||          // skip "during"
                        (text[i]=='i' && text[i+1]=='n' && text[i+2]=='s' && text[i+3]=='i' && text[i+4]=='d' && text[i+5]=='e') ||     // skip "inside"
                        (text[i]=='b' && text[i+1]=='e' && text[i+2]=='h' && text[i+3]=='i' && text[i+4]=='n' && text[i+5]=='d') ||     // skip "behind"
                        (text[i]=='n' && text[i+1]=='e' && text[i+2]=='a' && text[i+3]=='r' && text[i+4]=='l' && text[i+5]=='y'))       // skip "nearly"
                { i+=6; continue; }
            if((i+7<text.size() && (text[i+7]==' ' || text[i+7]=='\n')) || i+7==text.size())
                if((text[i]=='t' && text[i+1]=='h' && text[i+2]=='r' && text[i+3]=='o' && text[i+4]=='u' && text[i+5]=='g' && text[i+6]=='h') ||        // skip "through"
                        (text[i]=='b' && text[i+1]=='e' && text[i+2]=='c' && text[i+3]=='a' && text[i+4]=='u' && text[i+5]=='s' && text[i+6]=='e') ||   // skip "because"
                        (text[i]=='b' && text[i+1]=='e' && text[i+2]=='t' && text[i+3]=='w' && text[i+4]=='e' && text[i+5]=='e' && text[i+6]=='n'))     // skip "between"
                { i+=7; continue; }
        }
        if(i!=text.size() && text[i]=='\'')
        {
            if((i+2<text.size() && text[i+1]=='s' && (text[i+2]==' ' || text[i+2]=='\n')) || i+2==text.size())     // skip " 's "
            { i++; continue; }
            if((i+6<text.size() && text[i+1]=='c' && text[i+2]=='a' && text[i+3]=='u' && text[i+4]=='s' && text[i+5]=='e' &&    // skip " 'cause "
                (text[i+6]==' ' || text[i+6]=='\n')) || i+6==text.size())
            { i+=5; continue; }
        }

        if(i == text.size() || text[i] == ' ' || text[i] == '\n')      // if end of word is detected
        {
            if(check == "")
                continue;

            QString* checkWithGaps = new QString;
            *checkWithGaps = '#'+check+'#';

            if(cardWords.contains(*checkWithGaps))
            {
                QMessageBox mes(this);
                mes.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
                mes.setIcon(QMessageBox::Warning);
                mes.setWindowTitle("Info");
                mes.setText("Word \"" +check+ "\" is already on the cards.");
                QAbstractButton* ans = mes.addButton("Return", QMessageBox::YesRole);
                mes.addButton("Merge with next word", QMessageBox::NoRole);
                mes.exec();

                if(mes.clickedButton() == ans)
                {
                    delete checkWithGaps;
                    return "cancel";
                }

                else
                {
                    if(i==text.size())
                    {
                        QMessageBox::information(this, "Info", "Merge is impossible. This word is last on the card");
                        return "cancel";
                    }
                    check+=' ';
                    merge = true;
                    delete checkWithGaps;
                    continue;
                }
            }

            cardWords += check + '#';   // add current new word too to check next additions of repeating words that are written while current edit (they must not get added)

            delete checkWithGaps;
            merge = false;
            QString wholeLine;
            bool add = true;

            for(auto have : base)    // check whether vector have the assembled word
            {
                wholeLine = have;
                have.truncate(have.indexOf('_'));   // delete part of word after underscore for comparison in english
                if(check == have)
                {
                    translations+=('#'+wholeLine);
                    add = false;
                    break;
                }
            }

            if(add)     // if no coincidences word should get added with translation
            {
                SetTranslationDialog* enter = new SetTranslationDialog(this, check);
                enter->setStyleSheet(card->styleSheet());

                while(true)
                {
                    enter->setText(check);
                    enter->selectText();
                    int way = enter->exec();

                    if(way == 0)        // if "apply" is clicked
                    {
                        if(enter->getText().size() == 0)
                        {
                            QMessageBox::information(this, "Info", "The line is empty");
                            continue;
                        }
                        if(!checkCorrectness(enter->getText(), "rus"))
                        {
                            QMessageBox::warning(this, "Error", "In the line must be Russian word");
                            continue;
                        }
                    }
                    else if(way == 1)   // if "merge" is clicked
                    {
                        if(i==text.size())
                        {
                            QMessageBox::information(this, "Info", "Merge is impossible. This word is last on the card");
                            continue;
                        }
                        check+=' ';
                        merge = true;
                        break;
                    }
                    else if(way == 2)   // if "cancel" is clicked
                    {
                        delete enter;
                        return "cancel";
                    }
                    break;
                }

                if(merge)
                {
                    delete enter;
                    continue;
                }

                wholeLine = check + '_' + enter->getText() + '\n';
                delete enter;
                translations+=('#'+wholeLine);
                addNewWord(wholeLine);
            }
            check = "";
            continue;
        }
        check+=text[i];
    }
    return translations;
}

void MainWindow::setEnabledButtons(const bool on)
{
    ui->pushButtonMondayChange->    setEnabled(on);
    ui->pushButtonTuesdayChange->   setEnabled(on);
    ui->pushButtonWednesdayChange-> setEnabled(on);
    ui->pushButtonThursdayChange->  setEnabled(on);
    ui->pushButtonFridayChange->    setEnabled(on);
    ui->pushButtonSaturdayChange->  setEnabled(on);
    ui->pushButtonSundayChange->    setEnabled(on);
    ui->pushButtonWords->           setEnabled(on);
    ui->pushButtonMenuColor->       setEnabled(on);
    ui->pushButtonSetImg->          setEnabled(on);
    ui->pushButtonTraining->        setEnabled(on);
    ui->pushButtonCheckKnow->       setEnabled(on);
    ui->pushButtonGetCards->        setEnabled(on);
}

void MainWindow::clarifyDay(const QPushButton* button, QLabel*& card, QLabel*& weekday, int& number)
{
    if(button==ui->pushButtonMondayChange)
    {
        card =      ui->labelMonday;
        weekday =   ui->labelMon;
        number = 1;
    }
    else if(button==ui->pushButtonTuesdayChange)
    {
        card =      ui->labelTuesday;
        weekday =   ui->labelTue;
        number = 2;
    }
    else if(button==ui->pushButtonWednesdayChange)
    {
        card =      ui->labelWednesday;
        weekday =   ui->labelWed;
        number = 3;
    }
    else if(button==ui->pushButtonThursdayChange)
    {
        card =      ui->labelThursday;
        weekday =   ui->labelThr;
        number = 4;
    }
    else if(button==ui->pushButtonFridayChange)
    {
        card =      ui->labelFriday;
        weekday =   ui->labelFri;
        number = 5;
    }
    else if(button==ui->pushButtonSaturdayChange)
    {
        card =      ui->labelSaturday;
        weekday =   ui->labelSat;
        number = 6;
    }
    else if(button==ui->pushButtonSundayChange)
    {
        card =      ui->labelSunday;
        weekday =   ui->labelSun;
        number = 7;
    }
}

void MainWindow::on_pushButtonTraining_clicked()
{
    QFile file("mfwd.dll");
    if(file.size() == 0)
    {
        QMessageBox::information(this, "Info", "There are no words in your word base.\nPlease add words.");
        return;
    }

    int size = 0;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!file.atEnd())
        {
            file.readLine();
            size++;
        }
        file.close();
    }

    trainingMode window;
    window.sizeOfBase = size;
    window.wordBase = toVectorFromFile("mfwd.dll");

    QGraphicsBlurEffect* eff = new QGraphicsBlurEffect;
    eff->setBlurRadius(15);
    this->setGraphicsEffect(eff);
    window.exec();
    delete eff;
}

void MainWindow::on_pushButtonWords_clicked()
{
    wordsMode window;
    window.exec();
    QLabel* card[7] = { ui->labelMonday, ui->labelTuesday, ui->labelWednesday, ui->labelThursday, ui->labelFriday, ui->labelSaturday, ui->labelSunday };
    setTextToCards(card);       // there may be changes for the cards from words management window
}

void MainWindow::on_pushButtonCheckKnow_clicked()
{
    QFile file("mfwk.dll");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString* temp = new QString;
        *temp = file.readAll();
        file.close();

        if(!temp->contains(QRegularExpression("[A-Za-z]")))
        {
            delete temp;
            QMessageBox::information(this, "Info", "There are no words on the cards.\nPlease add words.");
            return;
        }
        delete temp;
    }

    int size = 0;
    QVector<QString> allCards;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!file.atEnd())
        {
            QString append = file.readLine();
            if(append[0]=='#')
            {
                append.remove(0, 1);        // delete '#'
                allCards.push_back(append);
                size++;
            }
        }
        file.close();
    }

    trainingMode window;
    window.sizeOfBase = size;
    window.wordBase = allCards;

    QGraphicsBlurEffect* eff = new QGraphicsBlurEffect;
    eff->setBlurRadius(15);
    this->setGraphicsEffect(eff);
    window.exec();
    delete eff;
}

void MainWindow::on_pushButtonMenuColor_clicked()
{
    QString newColor[2];
    for(int i = 0; i <2;)
    {
        QColorDialog select(this);
        select.setOption(QColorDialog::ShowAlphaChannel);
        select.setStyleSheet("QColorDialog{background-color: rgb(0, 85, 127);}"
                             "QLabel{color: rgb(255,255,0); font: 13pt Comic Sans MS;}");

        if(i==0)    select.setWindowTitle("Set Letters Color");
        else        select.setWindowTitle("Set Background Color");

        if(select.exec() == 0)
            return;

        QColor color = select.selectedColor();
        int r, g, b, a;
        color.getRgb(&r, &g, &b, &a);

        if(i==0)    newColor[i++] = "color: rgb("               +QString::number(r)+","+QString::number(g)+","+QString::number(b)+","+QString::number(a)+");\n";
        else        newColor[i++] = "background-color: rgb("    +QString::number(r)+","+QString::number(g)+","+QString::number(b)+","+QString::number(a)+");\n";
    }

    QFile file("mfstsh.dll");
    QString move;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!file.atEnd())
        {
            QString line = file.readLine();
            if(line[0]=='#')
                break;
            move+=line;     // get non-menu colors
        }
        file.close();
    }
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write((move + '#' + newColor[0] + '#' + newColor[1]).toUtf8());
        file.close();
    }

    const QString stsh = styleSheetForButton(newColor[0] + newColor[1]);
    ui->pushButtonWords->       setStyleSheet(stsh);
    ui->pushButtonMenuColor->   setStyleSheet(stsh);
    ui->pushButtonSetImg->      setStyleSheet(stsh);
    ui->pushButtonTraining->    setStyleSheet(stsh);
    ui->pushButtonCheckKnow->   setStyleSheet(stsh);
    ui->pushButtonGetCards->    setStyleSheet(stsh);
    ui->labelPrompt->           setStyleSheet(newColor[0]);
}

void MainWindow::on_pushButtonSetImg_clicked()
{
    setImage(this, QSize(1920,1080), "mfimgc.dll");
}

void MainWindow::on_pushButtonGetCards_clicked()
{
    QLabel* card[7] = { ui->labelMonday, ui->labelTuesday, ui->labelWednesday, ui->labelThursday, ui->labelFriday, ui->labelSaturday, ui->labelSunday };

    QFile file("mfwk.dll");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString* temp = new QString;
        *temp = file.readAll();
        file.close();

        if(!temp->contains(QRegularExpression("[A-Za-z]")))
        {
            delete temp;
            QMessageBox::information(this, "Info", "There are no words on the cards.\nPlease add words.");
            return;
        }
        delete temp;
    }

    QPixmap mainImg(":/img/images/fillbycards.png");
    QPainter painter(&mainImg);

    for(int i = 0, s = 0; i < 7; i++, s+=240)
    {
        QPixmap label = this->grab(card[i]->geometry());
        painter.drawPixmap(QPoint(0, s), label);
    }

    painter.end();
    QString path = QFileDialog::getExistingDirectory(this, "Select Location", "/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(path == 0)
        return;
    else
        mainImg.save(path + "/cards.png");
}

void MainWindow::on_pushButtonInfo_clicked()
{
    QMessageBox::information(this, "About", "EngDictionary 1.02\n\n"
                                            "Windows teaching application that helps to learn English words with interesting and\ncomfortable functions.");
}
