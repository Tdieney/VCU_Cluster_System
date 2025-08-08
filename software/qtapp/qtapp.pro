QT += quick

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        communication/canhandler.cpp \
        main.cpp

HEADERS += communication/canhandler.h

RESOURCES += qml.qrc \
    Configs.qrc \
    Fonts.qrc \
    Images.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    communication/canhandler.h

DISTFILES +=

json.path = $$OUT_PWD/configs
json.files = $$PWD/configs/io_config.json

INSTALLS += json
