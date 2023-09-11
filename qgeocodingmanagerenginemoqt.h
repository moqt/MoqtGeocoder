#ifndef QGEOCODINGMANAGERENGINEMOQT_H
#define QGEOCODINGMANAGERENGINEMOQT_H

#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QGeoCodingManagerEngine>


#ifdef Q_OS_ANDROID
#include <QtAndroidExtras>
#endif


class QGeoCodingManagerEngineMoqt : public QGeoCodingManagerEngine
{
    Q_OBJECT

public:
    QGeoCodingManagerEngineMoqt(const QVariantMap &parameters, QGeoServiceProvider::Error *error,
                               QString *errorString);

    QGeoCodeReply *geocode(const QGeoAddress &address, const QGeoShape &bounds) override;
    QGeoCodeReply *geocode(const QString &address, int limit, int offset,
                           const QGeoShape &bounds) override;
    QGeoCodeReply *reverseGeocode(const QGeoCoordinate &coordinate,
                                  const QGeoShape &bounds) override;

private:
#ifdef Q_OS_ANDROID
    QAndroidJniObject m_androidGeocoder;

    void initAndroidGeocoder();
#endif

    Q_DISABLE_COPY_MOVE(QGeoCodingManagerEngineMoqt)
};

#endif // QGEOCODINGMANAGERENGINEOrs_H
