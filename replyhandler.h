#ifndef REPLYHANDLER_H
#define REPLYHANDLER_H

#include <QtNetwork>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

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
    unsigned groupID;
    int countReceivedRequests;
    OutputData* outputData;
    bool recordOn;
    QSqlDatabase* db;
    bool serverConnectionEstablished_;

    bool connectDatabase();
    void saveReply(QNetworkReply*);
    void printInfo();
    bool insertInDatabase();

private slots:
    void getReply(QNetworkReply*);
    void changeSettings(const QString &, int, bool);

signals:
    void sendGuiText(const QString &, bool);
    void serverConnectionEstablished(bool);
};


//----------------------------------------
//----------------------------------------

struct ReplyHandler::TimeframePack
{
    int maBuy;
    int maSell;
    int tiBuy;
    int tiSell;
};

struct ReplyHandler::QuoteInfo
{
    static const int numTimeframes = 4;
    QMap <int, TimeframePack> timeframePacks;
};

struct ReplyHandler::OutputData
{
    QTime time;
    QMap <int, QuoteInfo> quotesInfo;
};


#endif // REPLYHANDLER_H
