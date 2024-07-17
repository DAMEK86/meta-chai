FILESEXTRAPATHS:prepend := "${THISDIR}/${BP}:"

SRC_URI += "file://mount-data.sh"

do_install:append() {
	install -m 755 ${WORKDIR}/*.sh ${D}${sysconfdir}/init.d
	update-rc.d -r ${D} mount-data.sh start 03 S .
}

PACKAGE_ARCH = "${MACHINE_ARCH}"