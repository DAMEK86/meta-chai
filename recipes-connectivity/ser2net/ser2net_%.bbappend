FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://ser2net.conf \
    file://ser2net.service \
    file://init \
    "

FILES_${PN} += " \
    ${sysconfdir}/ser2net.conf \
    ${systemd_unitdir}/system/ser2net.service \
    "

do_install_append() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/ser2net.conf ${D}${sysconfdir}

    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${sysconfdir}/system
        install -m 0644 ${WORKDIR}/ser2net.service ${D}${systemd_unitdir}/system/
    else
        install -d ${D}${sysconfdir}/init.d
        install -m 0755 ${WORKDIR}/init  ${D}${sysconfdir}/init.d/ser2net
    fi
}

SYSTEMD_PACKAGES = "${PN}"
INITSCRIPT_PACKAGES = "${PN}"

INITSCRIPT_NAME = "ser2net"
INITSCRIPT_PARAMS = "defaults 90 10"

SYSTEMD_SERVICE_${PN} = "ser2net.service"
SYSTEMD_AUTO_ENABLE_${PN} = "enable"

inherit systemd update-rc.d