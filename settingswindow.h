#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QtWidgets>
#include <QThread>
#include "RequestSender.h"

class SettingsWindow: public QWidget{
    Q_OBJECT

private:
    QLabel* quotesText;
    QLabel* freqText;

    QLineEdit* quotesEdit;
    QLineEdit* freqEdit;
    QCheckBox* fileCheckBox;
    QPushButton* goButton;

    QGridLayout* Layout;

    QThread* threadSender;
    RequestSender* requestSender;

signals:
    void settingsChangedCorrectly(const QString &, int, bool);

private slots:
    void settingsChanged();

public:
    SettingsWindow(QWidget* pwgt = 0);
};


#endif // SETTINGSWINDOW_H
