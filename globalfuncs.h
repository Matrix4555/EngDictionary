#ifndef GETWORD_H
#define GETWORD_H

#include <QRegularExpression>
#include <QFileDialog>
#include <QString>
#include <QVector>
#include <QLabel>
#include <QFile>

QVector<QString> toVectorFromFile(const QString);
QString styleSheetForButton(const QString, const QString = "");
bool checkCorrectness(const QString, const QString);
void setTextToCards(QLabel**);
void getWordsSeparately(const QString, QString&, QString&);
void addNewWord(const QString);
void setImage(QWidget*, const QSize, const QString);

#endif // GETWORD_H
