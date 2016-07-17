#ifndef REQUESTSENDER_H
#define REQUESTSENDER_H

#include <QtNetwork>
#include <QObject>
#include "ReplyHandler.h"

#define NUM_TIMEFRAMES 4

class Request;

class RequestSender: public QObject
{
    Q_OBJECT

private:
    QNetworkAccessManager* networkManager;
    QTimer* timer;
    Request* requests[NUM_TIMEFRAMES];
    int timeframes[NUM_TIMEFRAMES] = {60, 300, 900, 1800};
    int groupID;

    QThread* threadReplyHandler;
    ReplyHandler* replyHandler;

signals:
    void changeHandlerSettings(const QString &, bool);

private slots:
    void changeSettings(const QString &, int, bool);
    void sendRequests();

public:
    RequestSender(QObject* pobj = 0);
};


//------------------------------------------
//------------------------------------------
class Request: public QObject
{
    Q_OBJECT

private:
    QNetworkRequest* m_pnr;

    QUrl formUrl(const QString &, int);

public:
   Request(QObject* pobj);
   void formRequest(const QString &, int);
   QNetworkRequest getQRequest();
};

#endif // REQUESTSENDER_H
