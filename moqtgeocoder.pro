TARGET = qtgeoservices_moqt

QT += location-private positioning-private network concurrent

QT_FOR_CONFIG += location-private
qtConfig(location-labs-plugin): DEFINES += LOCATIONLABS


HEADERS += \
    qgeoserviceproviderpluginmoqt.h \
    qgeocodingmanagerenginemoqt.h \
    qgeocodereplymoqt.h

SOURCES += \
    qgeoserviceproviderpluginmoqt.cpp \
    qgeocodingmanagerenginemoqt.cpp \
    qgeocodereplymoqt.cpp

OTHER_FILES += \
    moqtgeocoder_plugin.json

ios {

OBJECTIVE_SOURCES += \
    qgeocodingmoqt.mm \

}

android {
    QT += androidextras concurrent
}



PLUGIN_TYPE = geoservices
PLUGIN_CLASS_NAME = QGeoServiceProviderFactoryMoqt
load(qt_plugin)

DESTDIR = $$[QT_INSTALL_PLUGINS]/$$PLUGIN_TYPE

