#include "globalfuncs.h"

QVector<QString> toVectorFromFile(const QString fileName)
{
    QVector<QString> temp;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!file.atEnd())
            temp.push_back(file.readLine());
        file.close();
    }
    return temp;
}

QString styleSheetForButton(const QString styleSheet, const QString font)
{
    QString newStSh = styleSheet;
    newStSh.remove(0, styleSheet.indexOf('\n')+1);      // get only background color
    QString* number = new QString;
    *number = newStSh;

    int letter = 0;
    for(int i = 0; i != 3; i++)
        while(true)
            if(newStSh[letter++] == ',')        // find the third comma (r, g, b, a)
                break;

    newStSh.truncate(letter);                   // truncate to comma behind transparency
    number->remove(0, letter);
    number->truncate(number->indexOf(')'));     // get only number of transparency
    int transparency = number->toInt();
    delete number;

    if(transparency<=205)
        transparency+=50;
    else
        transparency-=50;

    newStSh += QString::number(transparency) + ")";
    newStSh = "QPushButton{" + styleSheet + font + "}\nQPushButton::hover{" + newStSh + "}";
    return newStSh;
}

bool checkCorrectness(const QString word, const QString language)
{
    if(language=="eng")
    {
        if(word.contains(QRegularExpression("[^A-Za-z \n\'-]")))                // '\n' is allowed because it may be a text for one of card
            return false;
    }
    else if(language=="rus")
    {
        if(word.contains(QRegularExpression("[()]")))                           // if parentheses are there
        {                                                                       // then english is allowed for clarification
            if(word.contains(QRegularExpression("[^А-Яа-яЁёA-Za-z ()-,]")))
                return false;
        }
        else if(word.contains(QRegularExpression("[^А-Яа-яЁё -,]")))            // else only russian with gaps
            return false;
    }
    return true;
}

void setTextToCards(QLabel** arrayOfCards)
{
    QFile file("mfwk.dll");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString add = "";
        unsigned int n = 0;
        file.readLine();       // skip the line with number 1
        while(!file.atEnd())
        {
            QString line = file.readLine();
            if(line.contains(QRegularExpression("[2-7*]")))
            {
                add.remove(add.size()-1, 1);    // delete '\n' on the end
                arrayOfCards[n++]->setText(add);
                add.clear();
                continue;
            }
            if(line[0] != '#')      // skip translations
                add+=line;
        }
        file.close();
    }
}

void getWordsSeparately(const QString main, QString& first, QString& second)
{
    first = "", second = "";
    int n = 0;
    while(main[n] != '_')       // in the file with word base the underscore
        first+=main[n++];       // is the division between English and Russian words
    while(main[++n] != '\n')
        second+=main[n];
}

void addNewWord(const QString newWord)
{
    auto temp = toVectorFromFile("mfwd.dll");
    QFile file("mfwd.dll");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(newWord.toUtf8());   // add the new word first
        for(auto add : temp)
            file.write(add.toUtf8());
        file.close();
    }
}

void setImage(QWidget* window, const QSize scale, const QString fileName)
{
    QString path = QFileDialog::getOpenFileName(window, "Select Image", "/", "*.png *.jpg *.jpeg");

    if(path.isEmpty())
        return;

    QImage(path).scaled(scale).save("temp.png");

    if(fileName == "mfimgw.dll")
    {
        // x and y: 480 = 1920/2-(960/2) and 105 = 1080/2-(870/2)
        // width and height are size of word management window
        QRect rect(480, 105, 960, 870);
        QImage("temp.png").copy(rect).save("temp.png");     // truncate an image by center
    }

    QFile(fileName).remove();
    QFile("temp.png").rename(fileName);
    if(fileName == "mfimgc.dll")    window->setStyleSheet("QMainWindow{background-image: url("+fileName+");}"
                                                          "QMessageBox{background-color: rgb(0, 85, 127); font: 14pt Comic Sans MS;}"
                                                          "QLabel{color: rgb(255, 255, 0)}");
    else                            window->setStyleSheet("QDialog{background-image: url("+fileName+");}"
                                                          "QMessageBox{background-color: rgb(0, 85, 127); font: 14pt Comic Sans MS; background-image: url(0);}"
                                                          "QLabel{color: rgb(255, 255, 0)}");
}
