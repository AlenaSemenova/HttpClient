#ifndef REQUESTSENDER_H
#define REQUESTSENDER_H

#include <QtNetwork>
#include <QObject>
#include "ReplyHandler.h"

class RequestSender: public QObject
{
    Q_OBJECT

private:
    static const int numTimeframes = 4;
    QNetworkAccessManager* networkManager;
    QTimer* timer;
    class Request* requests[numTimeframes];
    int timeframes[numTimeframes] = {60, 300, 900, 1800};
    unsigned groupID;

private slots:
    void changeSettings(const QString &, int, bool);
    void sendRequests();
    void stopTimer();
    void testServerConnection();

public:
    RequestSender(QObject* pobj = 0, ReplyHandler* = 0);    

signals:
    void sendGuiText(const QString &, bool);
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
   ~Request();
   void formRequest(const QString &, int);

   friend class RequestSender;
};

#endif // REQUESTSENDER_H
