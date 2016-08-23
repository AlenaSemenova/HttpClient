#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QtWidgets>
#include <QThread>
#include "RequestSender.h"
#include "ReplyHandler.h"

class SettingsWindow: public QWidget
{
    Q_OBJECT

private:
    static const int minFreq = 400;
    QLabel* quotesText;
    QLabel* freqText;

    QLineEdit* quotesEdit;
    QLineEdit* freqEdit;
    QCheckBox* fileCheckBox;
    QPushButton* goButton;
    QPushButton* pauseButton;
    QTextEdit* textEdit;

    QGridLayout* Layout;

    QThread* threadSender;
    RequestSender* requestSender;
    QThread* threadReplyHandler;
    ReplyHandler* replyHandler;

    void closeEvent(QCloseEvent * event);

signals:
    void settingsChangedCorrectly(const QString &, int, bool);
    void windowClosed();

private slots:
    void settingsChanged();
    void debug();

public:
    SettingsWindow(QWidget* pwgt = 0);
    SettingsWindow(const SettingsWindow&);
};


#endif // SETTINGSWINDOW_H
