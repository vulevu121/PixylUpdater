#include "Firebase.h"

Firebase::Firebase(QObject *parent) : QObject(parent)
{

}

QString Firebase::idToken() {
    return m_idToken;
}

void Firebase::setIdToken(const QString &idToken) {
    if (idToken == m_idToken)
        return;
    m_idToken = idToken;
}

QString Firebase::refreshToken() {
    return m_idToken;
}

void Firebase::setRefreshToken(const QString &refreshToken) {
    if (refreshToken == m_refreshToken)
        return;
    m_idToken = refreshToken;
}



void Firebase::authenticate(const QString &user, const QString &password) {
    if (manager == nullptr) {
        manager = new QNetworkAccessManager(this);
    }

    qDebug() << "[Firebase] Authenticating";

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
    if(reply->error()) {
        qDebug() << "[Firebase]" << reply->error() << reply->errorString();
        if (reply->error() == QNetworkReply::HostNotFoundError) {
            emit authenticationError(reply->errorString());
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
                manager->disconnect();
                m_idToken = idToken;
                m_refreshToken = refreshToken;
//                getAccountInfo();
                getLatestDownload();
                emit authenticationSuccess("Login successful");
            }
        } else {
            emit authenticationError("Unable to retrieve ID token");
        }

    }


}

void Firebase::getUserData() {
    if (manager == nullptr) {
        manager = new QNetworkAccessManager(this);
    }
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
    manager->disconnect();
}

void Firebase::getLatestDownload() {
    if (manager == nullptr) {
        manager = new QNetworkAccessManager(this);
    }

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
//        qDebug() << jsonObject["fields"].toObject()["registration"].toObject()["stringValue"].toString();
        if (downloadLink.length() > 0) {
            qDebug() << "[Firebase] Link is" << downloadLink;
            downloadLatestRelease(downloadLink);
        }

    }
    manager->disconnect();



}

void Firebase::downloadLatestRelease(const QString &downloadLink) {
    if (manager == nullptr) {
        manager = new QNetworkAccessManager(this);
    }


    qDebug() << "[Firebase] Downloading" << downloadLink;
    QUrl downloadUrl(downloadLink);
    QNetworkRequest req(downloadUrl);

    manager->get(req);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
                        this, SLOT(downloadLatestReleaseReply(QNetworkReply*)));
}

void Firebase::downloadLatestReleaseReply(QNetworkReply *reply) {
    qDebug() << "Writing to file";
    QString filePath = "C:/Users/vulev/Documents/PixylBooth/PixylBooth_Update2.exe";
    QFile file(filePath);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    if(file.exists()) {
        file.write(reply->readAll());
        file.flush();
        file.close();
    }
    reply->deleteLater();
    qDebug() << "Download Completed!" << filePath;
    manager->disconnect();
}

void Firebase::getAccountInfo() {
    if (manager == nullptr) {
        manager = new QNetworkAccessManager(this);
    }

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
    manager->disconnect();

    getLatestDownload();
}
