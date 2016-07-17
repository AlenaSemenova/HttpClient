#include "ReplyHandler.h"

ReplyHandler::ReplyHandler(QObject *pobj): QObject(pobj)
{
    groupID = 0;
    countReceivedRequests = 0;
    outputData = new OutputData;
}

void ReplyHandler::changeHandlerSettingsSlot(const QString &m_quotes, bool m_fileRecordOn)
{
    fileRecordOn = m_fileRecordOn;

    outputData->quotesInfo.clear();
    QStringList quotes = m_quotes.split(',');
    QStringList::const_iterator it;
    QuoteInfo quoteInfo;
    for (it = quotes.constBegin(); it != quotes.constEnd(); ++it)
    {
        outputData->quotesInfo.insert(it -> toInt(), quoteInfo);
    }
}

void ReplyHandler::getReply(QNetworkReply* reply)
{
    ++countReceivedRequests;
    int error = reply -> error();
    if (error != QNetworkReply::NoError)
    {
        qDebug() << "Network error: " << error << endl;

        //Ошибка
    }
    else if (reply -> property("groupID").toInt() != groupID)
    {
        qDebug() << "Network error: incorrect groupID: "<< reply -> property("groupID").toInt() << " " <<
                 reply -> property("timeframe").toInt()<< endl;
        //Ошибка
    }
    else
    {

        //qDebug() << "Get Request. GRoupID: " << groupID << " Timeframe: " << reply -> property("timeframe").toInt() << " Time: " << time.currentTime() <<endl;

        saveReply(reply);
        if (countReceivedRequests == NUM_TIMEFRAMES)
        {
            QTime time;
            outputData -> time = time.currentTime();
            if (fileRecordOn)
            {
                // Запись в файл //
            }

            //OutputData.printInfo();
            // Отправка куда-то дальше //

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

    TimeframePack* p_tmfPack;
    QJsonObject quote_obj;
    int quote;
    QJsonObject::const_iterator it, time_it = jsonObject.constEnd();
    --time_it;
    for (it = jsonObject.constBegin(); it != time_it; ++it)
    {
        quote = it.key().toInt();
        p_tmfPack = &((outputData ->quotesInfo[quote]).timeframePacks[countReceivedRequests]);

        quote_obj = it.value().toObject();
        p_tmfPack -> timeframe = timeframe;
        p_tmfPack -> maBuy = quote_obj["maBuy"].toInt();
        p_tmfPack -> maSell = quote_obj["maSell"].toInt();
        p_tmfPack -> tiBuy = quote_obj["tiBuy"].toInt();
        p_tmfPack -> tiSell = quote_obj["tiSell"].toInt();
    }
}


void ReplyHandler:: printInfo()
{
    qDebug() << outputData->time << endl;
    QMap<int, QuoteInfo>::const_iterator it;
    int key;
    for (it = outputData->quotesInfo.constBegin(); it != outputData->quotesInfo.constEnd(); ++it)
    {
        key = it.key();
        qDebug() << "  " << key;
        for (int i = 0; i < 4; ++i)
        {
            qDebug() << "  " << it.value().timeframePacks[i].timeframe;
        }
    }
}

