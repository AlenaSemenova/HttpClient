#include"RequestSender.h"

RequestSender::RequestSender(QObject* pobj, ReplyHandler* replyHandler) : QObject(pobj)
{
    groupID = 0;
    timer = new QTimer(this);
    networkManager = new QNetworkAccessManager(this);
    for(int i = 0; i < numTimeframes; ++i)
        requests[i] = new Request(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(sendRequests()));
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            replyHandler, SLOT(getReply(QNetworkReply*)));
}

void RequestSender::changeSettings(const QString &quotes, int period, bool fileRecordOn)
{
    for(int i = 0; i < numTimeframes; ++i)
    {
        requests[i] -> formRequest(quotes, timeframes[i]);
    }

    timer ->start(period);
}

void RequestSender::sendRequests()
{
    for (int i = 0; i < numTimeframes; ++i)
    {
        QNetworkReply* reply = networkManager -> get(*(requests[i] -> m_pnr));
        reply ->setProperty("timeframe", QVariant(timeframes[i]));
        reply ->setProperty("groupID", QVariant(groupID));
    }
    qDebug() << "Requests were sent. GroupID " << groupID << " " << QTime().currentTime() <<endl;

    ++groupID;
}

void RequestSender::stopTimer()
{
    timer -> stop();
    emit sendGuiText("Application is paused.", false);
}

void RequestSender::testServerConnection()
{
    QNetworkRequest request;
    request.setUrl(QUrl("https://ssltsw.forexprostools.com/"));

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::AnyProtocol);
    request.setSslConfiguration(config);

    networkManager -> get(request);
}
/*
void RequestSender::increaseTime()
{
    timer -> setInterval((timer -> interval()) + 100);
}
*/
//--------------------------------------------
//--------------------------------------------
Request::Request(QObject* pobj) : QObject(pobj)
{
    m_pnr = new QNetworkRequest;
}

Request:: ~Request()
{
    delete m_pnr;
}

QUrl Request::formUrl(const QString &quotes, int timeframe)
{
    QString query = "https://ssltsw.forexprostools.com/api.php?";
    query.append("action=refresher&pairs=");
    query.append(quotes);
    query.append("&timeframe=");
    query.append(QString::number(timeframe));

    return QUrl(query);
}

void Request::formRequest(const QString &quotes, int timeframe)
{
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::AnyProtocol);
    m_pnr -> setSslConfiguration(config);

    m_pnr->setUrl(formUrl(quotes, timeframe));

    m_pnr->setRawHeader("Host", "ssltsw.forexprostools.com");
    m_pnr->setRawHeader("Connection", "keep-alive");
    m_pnr->setRawHeader("Referer", "https://ssltsw.forexprostools.com/");
}

