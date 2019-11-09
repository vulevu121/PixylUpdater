#include "Firebase.h"

Firebase::Firebase(QObject *parent) : QObject(parent)
{

}

QString Firebase::idToken() {
    return m_idToken;
}

void Firebase::setIdToken(const QString &idToken) {
    if (idToken != m_idToken)
        m_idToken = idToken;
}

bool Firebase::ready() {
    return m_ready;
}

void Firebase::setReady(bool ready) {
    if (ready != m_ready)
        m_ready = ready;
}

QString Firebase::refreshToken() {
    return m_idToken;
}

void Firebase::setRefreshToken(const QString &refreshToken) {
    if (refreshToken != m_refreshToken)
        m_idToken = refreshToken;
}

void Firebase::initManager() {
    manager->disconnect();
    if (manager == nullptr) {
        manager = new QNetworkAccessManager(this);
    }
}

void Firebase::authenticate(const QString &user, const QString &password) {
    initManager();
    if (!this->ready()) return;
    m_ready = false;

    qDebug() << "[Firebase] Authenticating";
//    emit status("Authenticating");

    QUrl endpoint("https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword?key=" + key);
    QNetworkRequest req(endpoint);

    req.setRawHeader("Content-Type","application/json");

    QJsonObject jsonObject {
        {"email", user},
        {"password", password},
        {"returnSecureToken", true}
    };

    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonRequest = jsonDoc.toJson();
    QByteArray postDataSize = QByteArray::number(jsonRequest.size());

    req.setRawHeader("Content-Length", postDataSize);
    manager->post(req, jsonRequest);

    connect(this->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(authenticateReply(QNetworkReply*)));

}

void Firebase::authenticateReply(QNetworkReply *reply) {
    manager->disconnect();
    if(reply->error()) {
        qDebug() << "[Firebase]" << reply->error() << reply->errorString();
        if (reply->error() == QNetworkReply::HostNotFoundError) {
            emit authenticationError(reply->errorString());
            emit status("Unable to connect");

            return;
        }

        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();

        QString msg(jsonObject["error"].toObject()["message"].toString());
        msg = msg.replace("_", " ").toLower();
        msg[0] = msg[0].toUpper();

        qDebug() << "[Firebase]" << msg;
        emit authenticationError(msg);


    } else {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        static QJsonObject jsonObject = jsonDoc.object();

        if (jsonObject.contains("idToken")) {
            QString idToken(jsonObject["idToken"].toString());
            QString refreshToken(jsonObject["refreshToken"].toString());

            if (idToken.length() > 100) {
                userJsonObject = &jsonObject;

//                qDebug() << userJsonObject->value("idToken").toString();

                m_idToken = jsonObject["idToken"].toString();
                m_refreshToken = jsonObject["refreshToken"].toString();
//                getAccountInfo();
//                getLatestDownload();
                emit status("Connected");
                emit authenticationSuccess("Login successful");
            }
        } else {
            emit authenticationError("Unable to retrieve ID token");
        }

    }

    m_ready = true;
}

void Firebase::getUserData() {
    initManager();
    if (!this->ready()) return;
    m_ready = false;

    qDebug() << "[Firebase] Getting user data";
    if (userJsonObject->contains("email")) {
        QString userEmail(userJsonObject->value("email").toString());

        QUrl endpoint("https://firestore.googleapis.com/v1beta1/projects/pixylbooth/databases/(default)/documents/users/" + userEmail);
        QNetworkRequest req(endpoint);


        QString authHeader("Bearer " + userJsonObject->value("idToken").toString());

        qDebug() << "[Firebase] User email: " << userEmail;
        qDebug() << "[Firebase] User ID token: " << userJsonObject->value("idToken").toString();

        req.setRawHeader("Authorization", authHeader.toUtf8());

        manager->get(req);

        connect(this->manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(getUserDataReply(QNetworkReply*)));
    }
    else {
        emit requestError("User data does not contain email");
    }

}

void Firebase::getUserDataReply(QNetworkReply *reply) {
    manager->disconnect();
    if(reply->error()) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();
        QString errString = jsonObject["error"].toObject()["message"].toString() + reply->errorString();
        qDebug() << "[Firebase]" << errString;
        emit requestError(errString);
    } else {

        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();

//        userInfoJsonObject = &jsonObject;

        qDebug() << "[Firebase]" << jsonObject["fields"];

//        qDebug() << jsonObject["fields"].toObject()["registration"].toObject()["stringValue"].toString();

    }
    m_ready = true;

}

void Firebase::getLatestDownload() {
    initManager();
    if (!this->ready()) return;
    m_ready = false;

    qDebug() << "[Firebase] Getting latest download link";

    QUrl endpoint("https://firestore.googleapis.com/v1/projects/pixylbooth/databases/(default)/documents/apps/pixylbooth");
    QNetworkRequest req(endpoint);

//    qDebug() << userJsonObject->value("idToken").toString();

    QString authHeader("Bearer " + userJsonObject->value("idToken").toString());

    req.setRawHeader("Authorization", authHeader.toUtf8());

    manager->get(req);

    connect(this->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(getLatestDownloadReply(QNetworkReply*)));
}

void Firebase::getLatestDownloadReply(QNetworkReply *reply) {
//    qDebug() << "getLatestDownloadReply";
    manager->disconnect();
    if(reply->error()) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();
        QString errString = jsonObject["error"].toObject()["message"].toString() + reply->errorString();
        qDebug() << "[Firebase]" << errString;
        emit requestError(errString);
    } else {

        QString downloadLink;
        QByteArray response = reply->readAll();

        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();

//        userInfoJsonObject = &jsonObject;

        downloadLink = jsonObject["fields"].toObject()["latestDownload"].toObject()["stringValue"].toString();

        if (downloadLink.length() > 0) {
//            qDebug() << "[Firebase] Link is" << downloadLink;
            download(downloadLink, "testPath");
        }

    }
    m_ready = true;


}

void Firebase::getDocument(const QString &docPath) {
    initManager();
    if (!this->ready()) return;
    m_ready = false;

    qDebug() << "[Firebase] Getting fields";

    QUrl endpoint("https://firestore.googleapis.com/v1/projects/pixylbooth/databases/(default)/documents" + docPath);
    QNetworkRequest req(endpoint);

    QString authHeader("Bearer " + userJsonObject->value("idToken").toString());
    req.setRawHeader("Authorization", authHeader.toUtf8());
    manager->get(req);

    connect(this->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(getDocumentReply(QNetworkReply*)));
}

void Firebase::getDocumentReply(QNetworkReply *reply) {
    manager->disconnect();
    if(reply->error()) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();
        QString errString = jsonObject["error"].toObject()["message"].toString() + reply->errorString();
        qDebug() << "[Firebase]" << errString;
        emit requestError(errString);
    } else {
        QByteArray response = reply->readAll();

        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();

//        qDebug() << response;

        emit documentReceived(jsonObject);
    }
    m_ready = true;
}



void Firebase::download(const QString &downloadLink, const QString &savePath) {
    initManager();
    if (!this->ready()) return;
    m_ready = false;

    if (downloadLink.length() == 0 || savePath.length() == 0) {
        qDebug() << "[Firebase] Missing link and/or save path";
        return;
    }
    this->savePath = savePath;

    qDebug() << "[Firebase] Downloading" << downloadLink;

    QUrl downloadUrl(downloadLink);

    QNetworkRequest req(downloadUrl);

    QString authHeader("Bearer " + userJsonObject->value("idToken").toString());
    req.setRawHeader("Authorization", authHeader.toUtf8());

    manager->get(req);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(downloadReply(QNetworkReply*)));
}

void Firebase::downloadReply(QNetworkReply *reply) {
    manager->disconnect();

    if(reply->error()) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();
        QString errString = jsonObject["error"].toObject()["message"].toString() + reply->errorString();
        qDebug() << "[Firebase]" << errString;
        emit requestError("Unable to update");
        return;
    }

    qDebug() << "[Firebase] Writing to file" << savePath;

    QByteArray bytesReceived = reply->readAll();

    if (bytesReceived.length() < 100) {
        return;
    }

    QFile file(savePath);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file.write(bytesReceived);
    file.flush();
    file.close();

    reply->deleteLater();
    emit downloadCompleted(savePath);
    emit status("Updated");
    qDebug() << "[Firebase] Download completed";
    m_ready = true;
}

void Firebase::getAccountInfo() {
    initManager();
    if (!this->ready()) return;
    m_ready = false;

    QUrl endpoint("https://www.googleapis.com/identitytoolkit/v3/relyingparty/getAccountInfo?key=" + key);
    QNetworkRequest req(endpoint);

    req.setRawHeader("Content-Type","application/json");

    if (m_idToken.length() > 0) {
        QJsonObject jsonObject {
            {"idToken", m_idToken}
        };

        QJsonDocument jsonDoc(jsonObject);
        QByteArray jsonRequest = jsonDoc.toJson();
        QByteArray postDataSize = QByteArray::number(jsonRequest.size());

        req.setRawHeader("Content-Length", postDataSize);
        manager->post(req, jsonRequest);

        connect(this->manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(getAccountInfoReply(QNetworkReply*)));
    }

}

void Firebase::getAccountInfoReply(QNetworkReply *reply) {
    manager->disconnect();
    if(reply->error()) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();

        qDebug() << jsonObject["error"].toObject()["message"].toString();
    } else {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        static QJsonObject jsonObject = jsonDoc.object();

        userInfoJsonObject = &jsonObject;

//        qDebug() << jsonObject;


        emit userInfoReceived();

//        emit userInfoReceived(userInfoJsonObject);
//        qDebug() << jsonObject;

    }
//    manager->disconnect();

//    getLatestDownload();
    m_ready = true;
}
