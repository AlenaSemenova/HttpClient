#include "ReplyHandler.h"

ReplyHandler::ReplyHandler(QObject *pobj): QObject(pobj)
{
    groupID = 0;
    countReceivedRequests = 0;
    outputData = new OutputData;
    db = nullptr;
    serverConnectionEstablished_ = false;
}

ReplyHandler::~ReplyHandler()
{
    delete outputData;
    delete db;
}

bool ReplyHandler::connectDatabase()
{
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC"));
    QString server = "ALENA-PC\\SQLEXPRESS";
    QString database = "Exchange";
    QString user = "user1";
    QString password = "user";
    db -> setDatabaseName(QString("DRIVER={SQL SERVER};SERVER={%1};DATABASE={%2};UID={%3};PWD={%4};Trusted_Connection=Yes;").
                          arg(server).arg(database).arg(user).arg(password));
    db -> setConnectOptions();
    return db -> open();
}

void ReplyHandler::changeSettings(const QString &m_quotes, int, bool m_recordOn)
{
    recordOn = m_recordOn;
    if (recordOn && (db == nullptr || !(db -> isOpen())))
    {
        if (!connectDatabase())
        {
            emit sendGuiText("Database connection error ", false);
        }
    }

    outputData -> quotesInfo.clear();
    QStringList quotes = m_quotes.split(',');
    QStringList::iterator it;
    QuoteInfo quoteInfo;
    for (it = quotes.begin(); it != quotes.end(); ++it)
    {
        outputData -> quotesInfo.insert(it -> toInt(), quoteInfo);
    }
}

void ReplyHandler::getReply(QNetworkReply* reply)
{    
    int error = reply -> error();
    unsigned receivedGroupID = reply -> property("groupID").toInt();

    qDebug() << "Get Request. GRoupID: " << receivedGroupID << endl;

    if (!serverConnectionEstablished_) //пробный запрос
    {
        serverConnectionEstablished_ = true;        
        emit serverConnectionEstablished(error == QNetworkReply::NoError);
        reply -> deleteLater();
        return;
    }
    if (error != QNetworkReply::NoError) //ошибка сервера
    {
        emit sendGuiText("Network error " + QString().setNum(error), true);

    }
    else if (receivedGroupID > groupID) //смешение запросов из разных групп
    {
        emit sendGuiText("Request was skipped " + QString().setNum(groupID), false);
        //emit sendGuiText("Request was skipped" + QTime().currentTime().toString(), false);
        qDebug() << "Skip request. Received id: " <<receivedGroupID << " groupID: " << groupID;

        groupID = receivedGroupID;

        QMap <int, QuoteInfo>::iterator it;
        for (it = outputData -> quotesInfo.begin(); it != outputData -> quotesInfo.end(); ++it)
        {
            it -> timeframePacks.clear();
        }

        saveReply(reply);
        countReceivedRequests = 1;
    }
    else if (receivedGroupID == groupID)
    {
        ++countReceivedRequests;

        saveReply(reply);
        if (countReceivedRequests == numTimeframes)
        {
            outputData -> time = QTime().currentTime();

            // printInfo();
            // Отправление //

            if (recordOn)
            {
                if (!insertInDatabase())
                   emit sendGuiText("Database insert error", true);
            }

            ++groupID;
            countReceivedRequests = 0;
        }

    }
    reply ->deleteLater();
}

void ReplyHandler::saveReply(QNetworkReply *reply)
{
    int timeframe = reply -> property("timeframe").toInt();
    QString strReply = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();

    TimeframePack p_tmfPack;
    QJsonObject quote_obj;
    int quote;
    QJsonObject::iterator it, time_it = --jsonObject.end();
    for (it = jsonObject.begin(); it != time_it; ++it)
    {
        quote = it.key().toInt();

        quote_obj = it.value().toObject();
        p_tmfPack.maBuy = quote_obj["maBuy"].toInt();
        p_tmfPack.maSell = quote_obj["maSell"].toInt();
        p_tmfPack.tiBuy = quote_obj["tiBuy"].toInt();
        p_tmfPack.tiSell = quote_obj["tiSell"].toInt();
        (outputData -> quotesInfo[quote]).timeframePacks[timeframe] = p_tmfPack;
    }
}


bool ReplyHandler::insertInDatabase()
{
    QSqlQuery query;
    query.prepare("INSERT INTO main (quote, time, timeframe, maBuy, maSell, tiBuy, tiSell) "
              "VALUES (:quote, :time, :timeframe, :maBuy, :maSell, :tiBuy, :tiSell)");

    QMap<int, QuoteInfo>::const_iterator quote_it;
    QMap<int, TimeframePack>::const_iterator tmfr_it;
    int quote;
    for (quote_it = outputData->quotesInfo.constBegin(); quote_it != outputData->quotesInfo.constEnd(); ++quote_it)
    {
        quote = quote_it.key();
        for (tmfr_it = (quote_it -> timeframePacks).constBegin(); tmfr_it != (quote_it -> timeframePacks).constEnd(); ++tmfr_it)
        {
            query.bindValue(":quote", QVariant(quote));
            query.bindValue(":time", QVariant((outputData -> time).toString("hh:mm:ss.zzz")));
            query.bindValue(":timeframe", QVariant(tmfr_it.key()));
            query.bindValue(":maBuy", QVariant(tmfr_it -> maBuy));
            query.bindValue(":maSell", QVariant(tmfr_it -> maSell));
            query.bindValue(":tiBuy", QVariant(tmfr_it -> tiBuy));
            query.bindValue(":tiSell", QVariant(tmfr_it -> tiSell));

            if (!query.exec())
                return false;
        }
    }

    return true;
}

void ReplyHandler:: printInfo()
{
    qDebug() <<" Request: " <<outputData->time << endl;
    QMap<int, QuoteInfo>::const_iterator it;
    QMap<int, TimeframePack>::const_iterator tmfr_it;
    int key;
    for (it = outputData->quotesInfo.constBegin(); it != outputData->quotesInfo.constEnd(); ++it)
    {
        key = it.key();
        qDebug() << key;
        for (tmfr_it = (it -> timeframePacks).constBegin(); tmfr_it != (it -> timeframePacks).constEnd(); ++tmfr_it)
        {
            qDebug() << " " << tmfr_it.key();
            qDebug() << "   maBuy:" << tmfr_it -> maBuy;
            qDebug() << "   maSell:" << tmfr_it -> maSell;
            qDebug() << "   tiBuy:" << tmfr_it -> tiBuy;
            qDebug() << "   tiSell:" << tmfr_it -> tiSell;
        }
    }
}

