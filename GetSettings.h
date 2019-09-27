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
        return settings.value("version").toString();
    }

    QString installDir(QString const &app) {
        QSettings settings(org, app);
        return settings.value("installDir").toString();
    }


private:
    QString org = "Pixyl";
};

#endif // GETSETTINGS_H
