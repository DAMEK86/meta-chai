SECTION = "x11/libs"
SUMMARY = "x11 application autostart"
DESCRIPTION = \
"This installs a /usr/bin/x-window-manager script. The script will start the \
one and only application X_APPLICATION as the last step of the \
xserver-nodm-init X initialization. \
If the script ever returns, X will be killed."

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit allarch

RDEPENDS_${PN} += "${INITIAL_APP_PKGS} xserver-nodm-init"

S = "${WORKDIR}"

SRC_URI = " \
    file://x-window-manager.in \
"

do_compile () {
    sed -e "s:@PATH@:${INITIAL_PATH}:" -e "s:@APP@:${X_APPLICATION}:" x-window-manager.in > x-window-manager
}

do_install () {
    install -d ${D}/${bindir} ${D}${sysconfdir}/X11/Xsession.d
    install -m 0755 ${S}/x-window-manager ${D}/${bindir}
}

# make sure xinput_calibrator is only started once
pkg_postinst_${PN}() {
    rm -f /etc/xdg/autostart/xinput_calibrator.desktop
}