#include "qgeoserviceproviderpluginmoqt.h"
#include "qgeocodingmanagerenginemoqt.h"


QGeoCodingManagerEngine *QGeoServiceProviderFactoryMoqt::createGeocodingManagerEngine(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    return new QGeoCodingManagerEngineMoqt(parameters, error, errorString);
}

