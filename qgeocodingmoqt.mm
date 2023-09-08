#include "qgeocodingmanagerenginemoqt.h"
#include "qgeocodereplymoqt.h"

#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoAddress>

#include <CoreLocation/CoreLocation.h>


namespace
{

void completeRequest(QGeoCodeReplyMoqt *geocodeReply,
                     NSArray<CLPlacemark *> *placemarks, NSError *error)
{
    if (error) {
        NSLog(@"Geocode failed with error: %@", error);
        geocodeReply->onError(QGeoCodeReply::CommunicationError, "CLGeocoder error");
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
        geocodeReply->onFinished(locations);

    } else {
        NSLog(@"Geocode returned no placemarks");
        geocodeReply->onError(QGeoCodeReply::CombinationError, "No placemarks");
    }
}

}


void QGeoCodingManagerEngineMoqt::initCLGeocoder()
{
    if (!m_clgeocoder) {
        m_clgeocoder = [[CLGeocoder alloc] init];
    }
}


QGeoCodingManagerEngineMoqt::~QGeoCodingManagerEngineMoqt()
{
    [m_clgeocoder release];
}


QGeoCodeReply *QGeoCodingManagerEngineMoqt::geocode(const QString &address, int limit, int offset, const QGeoShape &bounds)
{
    Q_UNUSED(bounds)

    initCLGeocoder();

    QGeoCodeReplyMoqt *geocodeReply = new QGeoCodeReplyMoqt(this, limit, offset);
    [m_clgeocoder geocodeAddressString:address.toNSString()
              completionHandler:^(NSArray<CLPlacemark *> *placemarks, NSError *error) {
                  completeRequest(geocodeReply, placemarks, error);
              }];

    return geocodeReply;
}


QGeoCodeReply *QGeoCodingManagerEngineMoqt::reverseGeocode(const QGeoCoordinate &coordinate,
                                                                 const QGeoShape &bounds)
{
    Q_UNUSED(bounds)

    initCLGeocoder();

    QGeoCodeReplyMoqt *geocodeReply = new QGeoCodeReplyMoqt(this, 1, 0);
    CLLocation *clLocation = [[CLLocation alloc]initWithLatitude:coordinate.latitude()
                                                       longitude:coordinate.longitude()];
    [m_clgeocoder reverseGeocodeLocation:clLocation
              completionHandler:^(NSArray<CLPlacemark *> *placemarks, NSError *error) {
                  completeRequest(geocodeReply, placemarks, error);
              }];

    return geocodeReply;
}
