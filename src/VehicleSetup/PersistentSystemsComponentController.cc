/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/// @file
///     @brief Doodle Config Qml Controller
///     @author Brad Stinson bstinson@echomav.com

#include "DoodleComponentController.h"
#include "QGCApplication.h"
#include "SettingsManager.h"

#include <QSettings>

QGC_LOGGING_CATEGORY(DoodleComponentControllerLog, "DoodleComponentControllerLog")

const char* DoodleComponentController::_settingsGroup =              "DoodleSettings";

DoodleComponentController::DoodleComponentController(void)
{
    _settingsManager = qgcApp()->toolbox()->settingsManager();
    _channel = 0;

}

void DoodleComponentController::start(void)
{
     qDebug() << "Doodle Component Controller Started";
     _statusText->setProperty("text","Loading...");
     _clientListText->setProperty("text","Loading...");
     _membersInMesh = 0;
     _ipClickCounter = 0;
     _isConnectionError = false;
     emit isConnectionErrorChanged(_isConnectionError);
     _isChannelLoadComplete = false;
     emit isChannelLoadCompleteChanged(_isChannelLoadComplete);

     _getDoodleStatus();

}

void DoodleComponentController::clickIP(void)
{
    //qDebug() << "IP Click";
    if (++_ipClickCounter > 4)
    {
        _isIPEditable = true;
        emit isIPEditableChanged(_isIPEditable);
    }
}

void DoodleComponentController::refresh(void)
{
    emit cancelDownload();
    _token = "";  //clear out the token
    start();
}

void DoodleComponentController::setChannel(int channel)
{
    _setChannel = channel;
}

void DoodleComponentController::updateChannelClicked()
{
    qDebug() << "got channel button click";
    QString doodleIP = _settingsManager->appSettings()->doodleIP()->rawValue().toString();
    QString doodleURI = QString("https://%1/ubus").arg(doodleIP);

    QUrl url(doodleURI);
    if (url.isValid() && !_token.isEmpty())
    {
        _setDoodleChannel(_token, url, _setChannel);
    }
}

void DoodleComponentController::_getDoodleStatus(void)
{
    //get the local doodle IP from settings
    QString doodleIP = _settingsManager->appSettings()->doodleIP()->rawValue().toString();
    QString doodleURI = QString("https://%1/ubus").arg(doodleIP);


    QUrl url(doodleURI);
    if (!url.isValid())
    {
        _isDoodleConnected = false;
        emit isConnectedChanged(_isDoodleConnected);
         _statusText->setProperty("text","Local Doodle Radio not found, please check IP address.");
        return;
    }

    //get the doodle token
    if (_token.isEmpty())
    {
        _getDoodleToken(QString("doodle"), QString("doodle"), url);
    }

    if (!_token.isEmpty())
    {
        _isDoodleConnected = true;  //because we got a token, we know the doodle is present
        emit isConnectedChanged(_isDoodleConnected);
        _statusText->setProperty("text","Connected!");

    }
    else
    {
        _isDoodleConnected = false;
        emit isConnectedChanged(_isDoodleConnected);

    }

}



DoodleComponentController::~DoodleComponentController()
{

}

void DoodleComponentController::_getDoodleToken(QString username, QString password, QUrl url)
{
    //use JSON-RPC to get the Doodle Token
    try {

        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);

        QNetworkRequest request(url);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QSslConfiguration conf = request.sslConfiguration();
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(conf);

        QByteArray data = QString("{\"jsonrpc\":\"2.0\",\"id\": 1,\"method\": \"call\",\"params\": [ \"00000000000000000000000000000000\", \"session\", \"login\", { \"username\": \"%1\", \"password\": \"%2\" } ] }" ).arg(username).arg(password).toUtf8();

       // qDebug() << "Sending data" << QString("{\"jsonrpc\":\"2.0\",\"id\": 1,\"method\": \"call\",\"params\": [ \"00000000000000000000000000000000\", \"session\", \"login\", { \"username\": \"%1\", \"password\": \"%2\" } ] }" ).arg(username).arg(password);
        QNetworkReply *reply = mgr->post(request, data);
        QObject::connect(this, SIGNAL(cancelDownload()), reply, SLOT(abort()));

        reply->ignoreSslErrors();

        QObject::connect(reply, &QNetworkReply::finished, [=](){
            if(reply->error() == QNetworkReply::NoError){
                QString contents = QString::fromUtf8(reply->readAll());
                QJsonDocument document = QJsonDocument::fromJson(contents.toUtf8());

                if (!document.isObject())
                    qDebug() << "document is not an object";

                QJsonObject object = document.object();
                QJsonValue result = object.value("result");
                if (!result.isUndefined())
                {
                    QJsonArray array = result.toArray();
                    QString token = array[1].toObject().value("ubus_rpc_session").toString();
                    //qDebug() << "got token" << token;
                    if (token.isEmpty())
                    {
                        _isDoodleConnected = false;  //because we got a token, we know the doodle is present
                        emit isConnectedChanged(_isDoodleConnected);
                        _isConnectionError = true;
                        emit isConnectionErrorChanged(_isConnectionError);
                        _statusText->setProperty("text","Failed: Ensure the Doodle is configured to allow remote configuration! See https://github.com/horiz31/doodle_rssi");
                        _clientListText->setProperty("text","");
                    }
                    else
                    {
                        _token = token;
                        _isDoodleConnected = true;  //because we got a token, we know the doodle is present
                        emit isConnectedChanged(_isDoodleConnected);
                        _statusText->setProperty("text","Connected!");
                        _getDoodleChannel(_token, url);
                        _getDoodleRSSIList(_token, url);
                    }
                }
                else
                {
                    _isDoodleConnected = false;  //because we got a token, we know the doodle is present
                    emit isConnectedChanged(_isDoodleConnected);
                    _statusText->setProperty("text","Failure getting a security token from the radio. Ensure the radio is set up to allow remote access! See https://github.com/horiz31/doodle_rssi");
                    _clientListText->setProperty("text","");
                }
                return;
            }
            else{
                QString err = reply->errorString();
                _statusText->setProperty("text","Failure communicating with the Doodle Radio: " + err);
                _isConnectionError = true;
                emit isConnectionErrorChanged(_isConnectionError);
                _clientListText->setProperty("text","");
                return;
            }
            reply->deleteLater();

         });
    }  catch (...) {

        qDebug() << "Error in _setDoodleChannel";
        return;
    }


    return;


}

void DoodleComponentController::_getDoodleChannel(QString token, QUrl url)
{
    //at this point we have a security token, so do the actual rpc call using file exec to change the channel

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    QByteArray data = QString("{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"call\", \"params\": [ \"%1\", \"iwinfo\", \"info\", {\"device\":\"wlan0\"} ] }").arg(token).toUtf8();

    QNetworkReply *reply = mgr->post(request, data);

    reply->ignoreSslErrors();

    QObject::connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QString contents = QString::fromUtf8(reply->readAll());

            QJsonDocument document = QJsonDocument::fromJson(contents.toUtf8());

            if (!document.isObject())
                qDebug() << "document is not an object";

            QJsonObject object = document.object();
            QJsonValue result = object.value("result");
            QJsonArray arrayouter = result.toArray();

            _channel = arrayouter[1].toObject().value("channel").toInt();
            qDebug() << "got doodle channel" << _channel;
            emit channelChanged(_channel);
        }
        else{
            QString err = reply->errorString();
            qDebug() << "Error getting doodle channel" << err;

        }
        reply->deleteLater();
    });

}

void DoodleComponentController::_setDoodleChannel(QString token, QUrl url, qint16 channel)
{
    //at this point we have a security token, so do the actual rpc call using file exec to change the channel

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    QByteArray data = QString("{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"call\", \"params\": [ \"%1\", \"file\", \"exec\", {\"command\": \"iw\", \"params\":[\"dev\", \"wlan0\", \"mesh\", \"chswitch\", \"%2\", \"10\"]} ] }").arg(token).arg(channel).toUtf8();

    QNetworkReply *reply = mgr->post(request, data);

    reply->ignoreSslErrors();

    QObject::connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QString contents = QString::fromUtf8(reply->readAll());

            QJsonDocument document = QJsonDocument::fromJson(contents.toUtf8());

            if (!document.isObject())
                qDebug() << "document is not an object";

            QJsonObject object = document.object();
            QJsonValue result = object.value("result");
            if (!result.isUndefined())
            {
                QJsonArray arrayouter = result.toArray();
                QJsonValue code = arrayouter[1].toObject().value("code");
                if (!code.isUndefined())
                {
                    qint16 returnCode = code.toInt();
                    if (returnCode == 0 || returnCode == 234)
                    {
                        QMessageBox msgBox;
                        msgBox.setIcon(QMessageBox::Information);
                        msgBox.setText("The Doodle channel has been changed successfully.");
                        msgBox.exec();
                    }
                }
                else
                {
                    QMessageBox msgBox;
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.setText("Changing the Doodle channel has FAILED: ensure that the local Doodle radio has permissions set up to allow this action! See https://github.com/horiz31/doodle_rssi");
                    msgBox.exec();
                    refresh();
                }
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setText("Changing the Doodle channel has FAILED: ensure that the local Doodle radio has permissions set up to allow this action! See https://github.com/horiz31/doodle_rssi");
                msgBox.exec();
                refresh();
            }



        }
        else{
            QString err = reply->errorString();
            qDebug() << "Error setting doodle channel in step 2" << err;
        }
        reply->deleteLater();
    });

}

void DoodleComponentController::_getDoodleRSSIList(QString token, QUrl url)
{
    const int rssi_limits[2] = {-85, -40};

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    QByteArray data = QString("{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"call\", \"params\": [ \"%1\", \"iwinfo\", \"assoclist\", {\"device\":\"wlan0\"} ] }").arg(token).toUtf8();

    QNetworkReply *reply = mgr->post(request, data);

    reply->ignoreSslErrors();

    QObject::connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QString contents = QString::fromUtf8(reply->readAll());
            QJsonDocument document = QJsonDocument::fromJson(contents.toUtf8());

            if (!document.isObject())
                qDebug() << "document is not an object";

            QJsonObject object = document.object();
            QJsonValue result = object.value("result");
            QJsonArray arrayouter = result.toArray();
            QJsonArray arrayinner = arrayouter[1].toObject().value("results").toArray();

            _doodleRSSIList.clear();
            QString tempBuilder;
            _membersInMesh = 0;
            int minRssi = 100;
            foreach (const QJsonValue & value, arrayinner)
            {
                QString mac = value.toObject().value("mac").toString();
                qint16 signal = value.toObject().value("signal").toInt();
                qDebug() << "Station"<< mac << "Signal"<< signal;
                DoodleRSSIEntry_t   rssi;
                rssi.mac =      mac;
                rssi.signal =   signal;
                //calculate the percentage

                if (signal >= rssi_limits[1]) rssi.percentage = 100;
                else if (signal <= rssi_limits[0]) rssi.percentage = 0;
                else
                {
                    rssi.percentage = ((100.0 / ((float)(rssi_limits[1] - rssi_limits[0]))) * (signal - rssi_limits[0] + 1));
                }
                tempBuilder += rssi.mac + " (RSSI: " + QString::number(rssi.percentage) + "%)\n";
                if (rssi.percentage < minRssi)
                    minRssi  = rssi.percentage;
                _membersInMesh++;

            }
            if (_membersInMesh == 0)
            {
                _clientListText->setProperty("text","No Radios Found in Mesh");
                _isChannelChangeOK = true;
                emit isChannelChangeOKChanged(_isChannelChangeOK);
            }
            else
            {
                if (minRssi > 50)
                {
                    _isChannelChangeOK = true;
                    emit isChannelChangeOKChanged(_isChannelChangeOK);
                }
                else
                {
                    _isChannelChangeOK = false;
                    emit isChannelChangeOKChanged(_isChannelChangeOK);
                }
                if (_membersInMesh == 1)
                    tempBuilder = QString::number(_membersInMesh) + " node in mesh\n" + tempBuilder;
                else
                    tempBuilder = QString::number(_membersInMesh) + " nodes in mesh\n" + tempBuilder;
                _clientListText->setProperty("text",tempBuilder);
            }
            _isChannelLoadComplete = true;
            emit isChannelLoadCompleteChanged(_isChannelLoadComplete);


        }
        else{
            QString err = reply->errorString();
            _doodleRSSIList.clear();

            qDebug() << "Error getting RSSI from Doodle radio step 2" << err;
        }
        reply->deleteLater();
    });
}


