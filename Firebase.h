#ifndef FIREBASE_H
#define FIREBASE_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QFile>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

class Firebase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString idToken READ idToken WRITE setIdToken)
    Q_PROPERTY(QString refreshToken READ refreshToken WRITE setRefreshToken)

public:
    explicit Firebase(QObject *parent = nullptr);

private:
    QNetworkAccessManager *manager = nullptr;
    QJsonObject *userJsonObject;
    QJsonObject *userInfoJsonObject;
    QString key = "AIzaSyBKWMrLRUNoPpZ9fMshmA3ZD19Wbujk9wU";
    QString m_idToken;
    QString m_refreshToken;
    QString idToken();
    void setIdToken(const QString &idToken);
    QString refreshToken();
    void setRefreshToken(const QString &refreshToken);

signals:
//    void userNotAuthenticated(const QString &msg);
//    void userAuthenticated(QJsonObject userJsonObject);
//    void userInfoReceived(QJsonObject userInfoJsonObject);
//    void userAuthenticated();
    void userInfoReceived();
    void authenticationError(const QString &msg);
    void authenticationSuccess(const QString &msg);
    void requestError(const QString &msg);
    void status(const QString &msg);
    void fieldsReceived(QJsonObject json);

public slots:
    void authenticate(const QString &user, const QString &password);
    void getUserData();
    void getLatestDownload();
    void getAccountInfo();
    void downloadLatestRelease(const QString &downloadLink);
    void getFields(const QString &path);

private slots:
    void authenticateReply(QNetworkReply *reply);
    void getAccountInfoReply(QNetworkReply *reply);
    void getUserDataReply(QNetworkReply *reply);
    void getLatestDownloadReply(QNetworkReply *reply);
    void downloadLatestReleaseReply(QNetworkReply *reply);
    void getFieldsReply(QNetworkReply *reply);
};

#endif // FIREBASE_H
