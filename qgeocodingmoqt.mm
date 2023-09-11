#include "qgeocodingmanagerenginemoqt.h"
#include "qgeocodereplymoqt.h"

#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoAddress>
#include <CoreLocation/CoreLocation.h>
#include <QDebug>
#include <QPointer>


namespace
{

void completeRequest(QGeoCodeReplyMoqt *geocodeReply,
                     NSArray<CLPlacemark *> *placemarks, NSError *error)
{
    if (error) {
        NSLog(@"Geocode failed with error: %@", error);
        QMetaObject::invokeMethod(geocodeReply, "onError", Qt::QueuedConnection,
                                  Q_ARG(QGeoCodeReply::Error, QGeoCodeReply::CommunicationError),
                                  Q_ARG(QString, "CLGeocoder error"));
        return;
    }

    if (placemarks && placemarks.count > 0) {
        QList<QGeoLocation> locations;
        for (unsigned i = 0; i < placemarks.count; ++i) {
            CLPlacemark *placemark = placemarks[i];

            QGeoLocation location;
            QGeoAddress address;
            address.setText(QString::fromNSString(placemark.name));
            address.setState(QString::fromNSString(placemark.administrativeArea)); // state, eg. CA
            address.setStreet(QString::fromNSString(placemark.thoroughfare)); // street name, eg. Infinite Loop
            address.setCountry(QString::fromNSString(placemark.country)); // eg. United States
            address.setCountryCode(QString::fromNSString(placemark.ISOcountryCode)); // eg. US
            address.setCity(QString::fromNSString(placemark.locality)); // city, eg. Cupertino
            address.setDistrict(QString::fromNSString(placemark.subLocality));// neighborhood, common name, eg. Mission District
            address.setPostalCode(QString::fromNSString(placemark.postalCode)); // zip code, eg. 95014
            //placemark.subAdministrativeArea; // county, eg. Santa Clara

            location.setCoordinate(QGeoCoordinate(placemark.location.coordinate.latitude,
                                                  placemark.location.coordinate.longitude));
            location.setAddress(address);

            locations.append(location);
        }
        QMetaObject::invokeMethod(geocodeReply, "onFinished", Qt::QueuedConnection,
                                  Q_ARG(QList<QGeoLocation>, locations));

    } else {
        NSLog(@"Geocode returned no placemarks");
        QMetaObject::invokeMethod(geocodeReply, "onError", Qt::QueuedConnection,
                                  Q_ARG(QGeoCodeReply::Error, QGeoCodeReply::CombinationError),
                                  Q_ARG(QString, "No placemarks"));
    }
}

}


QGeoCodeReply *QGeoCodingManagerEngineMoqt::geocode(const QString &address, int limit, int offset, const QGeoShape &bounds)
{
    Q_UNUSED(bounds)

    const QPointer<QGeoCodeReplyMoqt> geocodeReply = new QGeoCodeReplyMoqt(this, limit, offset);
    CLGeocoder *clgeocoder = [[CLGeocoder alloc] init];
    [clgeocoder geocodeAddressString:address.toNSString()
              completionHandler:^(NSArray<CLPlacemark *> *placemarks, NSError *error) {
                  completeRequest(geocodeReply, placemarks, error);
                  [clgeocoder release]; // ARC is disabled in Qt
              }];

    return geocodeReply;
}


QGeoCodeReply *QGeoCodingManagerEngineMoqt::reverseGeocode(const QGeoCoordinate &coordinate,
                                                                 const QGeoShape &bounds)
{
    Q_UNUSED(bounds)

    const QPointer<QGeoCodeReplyMoqt> geocodeReply = new QGeoCodeReplyMoqt(this, 1, 0);
    CLLocation *cllocation = [[CLLocation alloc]initWithLatitude:coordinate.latitude()
                                                       longitude:coordinate.longitude()];
    CLGeocoder *clgeocoder = [[CLGeocoder alloc] init];
    [clgeocoder reverseGeocodeLocation:cllocation
              completionHandler:^(NSArray<CLPlacemark *> *placemarks, NSError *error) {
                  completeRequest(geocodeReply, placemarks, error);
                  [cllocation release]; // ARC is disabled in Qt
                  [clgeocoder release]; // ARC is disabled in Qt
              }];

    return geocodeReply;
}
