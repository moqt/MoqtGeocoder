#ifndef QGEOCODINGMANAGERENGINEMOQT_H
#define QGEOCODINGMANAGERENGINEMOQT_H

#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QGeoCodingManagerEngine>


#ifdef Q_OS_ANDROID
#include <QtAndroidExtras>
#endif

#ifdef Q_OS_IOS
#ifdef __OBJC__
@class CLGeocoder;
typedef CLGeocoder * CLGeocoderPtr;
#else
typedef void * CLGeocoderPtr;
#endif
#endif


class QGeoCodingManagerEngineMoqt : public QGeoCodingManagerEngine
{
    Q_OBJECT

public:
    QGeoCodingManagerEngineMoqt(const QVariantMap &parameters, QGeoServiceProvider::Error *error,
                               QString *errorString);

#ifdef Q_OS_IOS
    ~QGeoCodingManagerEngineMoqt() override;
#endif

    QGeoCodeReply *geocode(const QGeoAddress &address, const QGeoShape &bounds) override;
    QGeoCodeReply *geocode(const QString &address, int limit, int offset,
                           const QGeoShape &bounds) override;
    QGeoCodeReply *reverseGeocode(const QGeoCoordinate &coordinate,
                                  const QGeoShape &bounds) override;

private:
#ifdef Q_OS_IOS
    CLGeocoderPtr m_clgeocoder = nullptr;

    void initCLGeocoder();
#endif

#ifdef Q_OS_ANDROID
    QAndroidJniObject m_androidGeocoder;

    void initAndroidGeocoder();
#endif

    Q_DISABLE_COPY_MOVE(QGeoCodingManagerEngineMoqt)
};

#endif // QGEOCODINGMANAGERENGINEOrs_H
