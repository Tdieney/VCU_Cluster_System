SUMMARY = "Qt Quick Instrument Cluster Application"
DESCRIPTION = "A Qt Quick application for an instrument cluster"
LICENSE = "CLOSED"

DEPENDS += "qtbase qtdeclarative qtquickcontrols2 qtgraphicaleffects"

inherit qmake5 systemd

SRC_URI += " \
    file://main.cpp \
    file://main.qml \
    file://qml.qrc \
    file://qtapp.pro \
    file://Images.qrc \
    file://Fonts.qrc \
    file://communication/canhandler.cpp \
    file://communication/canhandler.h \
    file://fonts/Aldrich-Regular.ttf \
    file://images/background.png \
    file://images/centre.png \
    file://images/arrow.png \
    file://images/high_beam.png \
    file://images/low_beam.png \
    file://images/parking_lights.png \
    file://images/hazard.png \
    file://images/left_arrow.png \
    file://images/right_arrow.png \
    file://io_configs/io_config.json \
    file://qtapp.service \
"

S = "${WORKDIR}"
B = "${WORKDIR}/build"

do_install() {
    # Install the Qt binary
    install -d ${D}/opt/qtapp
    install -m 0755 ${B}/qtapp ${D}/opt/qtapp/

    # Install systemd unit file
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/qtapp.service ${D}${systemd_system_unitdir}/qtapp.service

    # Install io_configs folder to /opt/qtapp/io_configs
    install -d ${D}/opt/qtapp/io_configs
    install -m 0644 ${WORKDIR}/io_configs/io_config.json ${D}/opt/qtapp/io_configs/
}

FILES:${PN} += "/opt/qtapp/qtapp"
FILES:${PN} += "${systemd_system_unitdir}/qtapp.service"
FILES:${PN} += "/opt/qtapp/io_configs/"
FILES:${PN} += "/opt/qtapp/io_configs/io_config.json"

SYSTEMD_SERVICE:${PN} = "qtapp.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

