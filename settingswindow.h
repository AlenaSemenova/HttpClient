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
    static const int minPeriod = 300;
    QLabel* quotesText;
    QLabel* periodText;

    QLineEdit* quotesEdit;
    QLineEdit* periodEdit;
    QCheckBox* recordCheckBox;
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
    void testServerConnection();

private slots:
    void settingsChanged();
    void printText(const QString &, bool);
    void serverConnectionHandler(bool);

public:
    SettingsWindow(QWidget* pwgt = 0);
    SettingsWindow(const SettingsWindow&);
};


#endif // SETTINGSWINDOW_H
