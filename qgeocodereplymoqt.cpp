#include "qgeocodereplymoqt.h"
#include "qgeocodingmanagerenginemoqt.h"

#include <QDebug>


QGeoCodeReplyMoqt::QGeoCodeReplyMoqt(QGeoCodingManagerEngineMoqt *geocodingManager, int limit, int offset)
    : QGeoCodeReply(geocodingManager), m_geocodingManager(geocodingManager)
{
    setLimit(limit);
    setOffset(offset);
}


QGeoCodeReplyMoqt::~QGeoCodeReplyMoqt()
{
    // qDebug() << "QGeoCodeReplyMoqt::~QGeoCodeReplyMoqt()" << this;
}


void QGeoCodeReplyMoqt::onError(QGeoCodeReplyMoqt::Error error, QString errorString)
{
    setError(error, errorString);
    emit m_geocodingManager->error(this, error, errorString);
}


void QGeoCodeReplyMoqt::onFinished(QList<QGeoLocation> locations)
{
    setLocations(locations);
    setFinished(true);
    emit m_geocodingManager->finished(this);
}



