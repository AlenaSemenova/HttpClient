#ifndef REPLYHANDLER_H
#define REPLYHANDLER_H

#include <QtNetwork>
#include <QObject>

#define NUM_TIMEFRAMES 4

class OutputData;

class ReplyHandler: public QObject
{
    Q_OBJECT

private:
    int groupID;
    int countReceivedRequests;
    OutputData* outputData;
    bool fileRecordOn;

    void saveReply(QNetworkReply*);
    void printInfo();

public:
    ReplyHandler(QObject* pobj = 0);

private slots:
    void getReply(QNetworkReply*);
    void changeHandlerSettingsSlot(const QString &, bool);
};


//----------------------------------------
//----------------------------------------

class TimeframePack
{
    int timeframe;
    int maBuy;
    int maSell;
    int tiBuy;
    int tiSell;

    friend ReplyHandler;
};

class QuoteInfo
{
    TimeframePack timeframePacks[NUM_TIMEFRAMES];

    friend ReplyHandler;
};

class OutputData
{
    QTime time;
    QMap <int, QuoteInfo> quotesInfo;

    friend ReplyHandler;
};


#endif // REPLYHANDLER_H
