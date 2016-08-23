#ifndef REPLYHANDLER_H
#define REPLYHANDLER_H

#include <QtNetwork>
#include <QObject>

class ReplyHandler: public QObject
{
    Q_OBJECT

public:
    ReplyHandler(QObject* pobj = 0);
    ~ReplyHandler();
    struct TimeframePack;
    struct QuoteInfo;
    struct OutputData;


private:
    static const int numTimeframes = 4;
    int groupID;
    int countReceivedRequests;
    OutputData* outputData;
    bool fileRecordOn;

    void saveReply(QNetworkReply*);
    void printInfo();

private slots:
    void getReply(QNetworkReply*);
    void changeSettings(const QString &, int, bool);
};


//----------------------------------------
//----------------------------------------

struct ReplyHandler::TimeframePack
{
    int timeframe;
    int maBuy;
    int maSell;
    int tiBuy;
    int tiSell;
};

struct ReplyHandler::QuoteInfo
{
    static const int numTimeframes = 4;
    TimeframePack timeframePacks[numTimeframes];
};

struct ReplyHandler::OutputData
{
    QTime time;
    QMap <int, QuoteInfo> quotesInfo;
};


#endif // REPLYHANDLER_H
