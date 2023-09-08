# Moqt plugin for QtLocation
QML API to access platform geocoders

***Tested with Qt 5.15.2***

## Supported platforms:
- Android
- iOS


## Supported features:
- Online geocoding
- Reverse geocoding


## Integration
- Build the project to install plugin
- Add the reference in your app pro file 
```
include(MoqtGeocoderPlugin/qt_plugin_qtgeoservices_moqt.pri)
```

## Declare plugin in QML
```
    property Plugin pluginMoqt: Plugin {
        name: 'moqt'
    }    
```

## Available parameters:
