#include "qgeocodingmanagerenginemoqt.h"
#include "qgeocodereplymoqt.h"

#include <QtConcurrent>
#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoAddress>
#include <QtPositioning/QGeoShape>
#include <QtPositioning/QGeoRectangle>


namespace
{

#ifdef Q_OS_ANDROID

QString jaddressProperty(const QAndroidJniObject& jaddress, const char* name)
{
    QAndroidJniObject jstr = jaddress.callObjectMethod<jstring>(name);
    return jstr.toString();
}


void completeRequest(QGeoCodeReplyMoqt *geocodeReply, const QAndroidJniObject& jaddresses)
{
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        env->ExceptionClear();
    }

    if (!jaddresses.isValid() || jaddresses.callMethod<jboolean>("isEmpty")) {
        QMetaObject::invokeMethod(geocodeReply, "onError", Qt::QueuedConnection,
                                  Q_ARG(QGeoCodeReply::Error, QGeoCodeReply::CommunicationError),
                                  Q_ARG(QString, "Android geocoder returned empty list"));

    } else {
        QList<QGeoLocation> locations;
        for (int i = 0, size = jaddresses.callMethod<jint>("size"); i < size; ++i) {
            QAndroidJniObject jaddress = jaddresses.callObjectMethod("get", "(I)Ljava/lang/Object;", i);

            QGeoLocation location;
            QGeoAddress address;
            address.setText(jaddressProperty(jaddress, "getFeatureName"));
            address.setState(jaddressProperty(jaddress, "getAdminArea"));
            address.setStreet(jaddressProperty(jaddress, "getThoroughfare"));
            address.setCountry(jaddressProperty(jaddress, "getCountryName")); // eg. United States
            address.setCountryCode(jaddressProperty(jaddress, "getCountryCode")); // eg. US
            address.setCity(jaddressProperty(jaddress, "getLocality")); // city, eg. Cupertino
            address.setDistrict(jaddressProperty(jaddress, "getSubLocality"));// neighborhood, common name, eg. Mission District
            address.setPostalCode(jaddressProperty(jaddress, "getPostalCode")); // zip code, eg. 95014

            location.setCoordinate(QGeoCoordinate(jaddress.callMethod<jdouble>("getLatitude"),
                                                  jaddress.callMethod<jdouble>("getLongitude")));
            location.setAddress(address);
            locations.append(location);
        }
        QMetaObject::invokeMethod(geocodeReply, "onFinished", Qt::QueuedConnection,
                                  Q_ARG(QList<QGeoLocation>, locations));
    }
}

#endif

QList<QGeoLocation> buildTestData()
{
    QList<QGeoLocation> locations;
    QGeoLocation location;
    QGeoAddress address;
    address.setText("Eiffel tower, Paris");
    address.setState("Paris");
    address.setStreet("Eiffel tower");
    address.setCountry("France");
    address.setCountryCode("FR");
    address.setCity("Paris");

    location.setCoordinate(QGeoCoordinate(48.858, 2.294));
    const QGeoRectangle bb{QGeoCoordinate(48.859, 2.293),
                           QGeoCoordinate(48.857, 2.295)};
    location.setBoundingBox(bb);
    location.setAddress(address);

    locations.append(location);
    return locations;
}


QString addressToQuery(const QGeoAddress &address)
{
    return address.street() + QStringLiteral(",+") +
           address.district() + QStringLiteral(",+") +
           address.city() + QStringLiteral(",+") +
           address.state() + QStringLiteral(",+") +
           address.country();
}

}


QGeoCodingManagerEngineMoqt::QGeoCodingManagerEngineMoqt(const QVariantMap &parameters,
                                                         QGeoServiceProvider::Error *error,
                                                         QString *errorString)
    :   QGeoCodingManagerEngine(parameters)
{

    *error = QGeoServiceProvider::NoError;
    errorString->clear();
}


QGeoCodeReply *QGeoCodingManagerEngineMoqt::geocode(const QGeoAddress &address, const QGeoShape &bounds)
{
    return geocode(addressToQuery(address), 1, 0, bounds);
}


#ifndef Q_OS_IOS

#ifdef Q_OS_ANDROID

void QGeoCodingManagerEngineMoqt::initAndroidGeocoder()
{
    if (!m_androidGeocoder.isValid()) {
        m_androidGeocoder = QAndroidJniObject("android/location/Geocoder",
                            "(Landroid/content/Context;)V",
                            QtAndroid::androidContext().object());
    }
}


QGeoCodeReply *QGeoCodingManagerEngineMoqt::geocode(const QString &address, int limit, int offset, const QGeoShape &bounds)
{
    Q_UNUSED(bounds)

    initAndroidGeocoder();

    const QPointer<QGeoCodeReplyMoqt> geocodeReply = new QGeoCodeReplyMoqt(this, limit, offset);
    QtConcurrent::run([geocodeReply, address, androidGeocoder = m_androidGeocoder, limit] () {
        const QAndroidJniObject jaddresses = androidGeocoder.callObjectMethod("getFromLocationName", "(Ljava/lang/String;I)Ljava/util/List;",
                                                                              QAndroidJniObject::fromString(address).object<jstring>(), limit);
        completeRequest(geocodeReply, jaddresses);
    });

    return geocodeReply;
}


QGeoCodeReply *QGeoCodingManagerEngineMoqt::reverseGeocode(const QGeoCoordinate &coordinate,
                                                           const QGeoShape &bounds)
{
    Q_UNUSED(bounds)

    initAndroidGeocoder();

    const QPointer<QGeoCodeReplyMoqt> geocodeReply = new QGeoCodeReplyMoqt(this, 1, 0);
    QtConcurrent::run([geocodeReply, coordinate, androidGeocoder = m_androidGeocoder] () {
        const QAndroidJniObject jaddresses = androidGeocoder.callObjectMethod("getFromLocation", "(DDI)Ljava/util/List;",
                                                                             coordinate.latitude(), coordinate.longitude(), 1);
        completeRequest(geocodeReply, jaddresses);
    });

    return geocodeReply;
}

#else

QGeoCodeReply *QGeoCodingManagerEngineMoqt::geocode(const QString &address, int limit, int offset, const QGeoShape &bounds)
{
    Q_UNUSED(address)
    Q_UNUSED(bounds)

    QGeoCodeReplyMoqt *geocodeReply = new QGeoCodeReplyMoqt(this, limit, offset);
    geocodeReply->onFinished(buildTestData());
    return geocodeReply;
}


QGeoCodeReply *QGeoCodingManagerEngineMoqt::reverseGeocode(const QGeoCoordinate &coordinate,
                                                                 const QGeoShape &bounds)
{
    Q_UNUSED(coordinate)
    Q_UNUSED(bounds)

    QGeoCodeReplyMoqt *geocodeReply = new QGeoCodeReplyMoqt(this, 1, 0);
    geocodeReply->onFinished(buildTestData());
    return geocodeReply;
}

#endif

#endif

