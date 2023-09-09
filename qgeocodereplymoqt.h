#ifndef QGEOCODEREPLYMOQT_H
#define QGEOCODEREPLYMOQT_H

#include <QtLocation/QGeoCodeReply>

class QGeoCodingManagerEngineMoqt;

class QGeoCodeReplyMoqt : public QGeoCodeReply
{
    Q_OBJECT

public:
    QGeoCodeReplyMoqt(QGeoCodingManagerEngineMoqt *geocodingManager, int limit, int offset);
    ~QGeoCodeReplyMoqt() override;

public slots:
    void onError(Error error, QString errorString);
    void onFinished(QList<QGeoLocation> locations);

private:
    QGeoCodingManagerEngineMoqt* m_geocodingManager;
};

#endif // QGEOCODEREPLYORS_H
