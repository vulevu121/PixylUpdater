#ifndef GETSETTINGS_H
#define GETSETTINGS_H

#include <QObject>
#include <QSettings>

class GetSettings : public QObject
{
    Q_OBJECT
public:
    explicit GetSettings(QObject *parent = nullptr);

signals:

public slots:
    QString version(QString const &app) {
        QSettings settings(org, app);
        QString version = settings.value("version").toString();
        version = version.left(version.indexOf(".")+2);
        return version;
    }

    QString exePath(QString const &app) {
        QSettings settings(org, app);
        return settings.value("exePath").toString();
    }

    void setVersion(QString const &app, QString const &ver) {
        QSettings settings(org, app);
        settings.setValue("version", ver);
    }



private:
    QString org = "Pixyl";
};

#endif // GETSETTINGS_H
