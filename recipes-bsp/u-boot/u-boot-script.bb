LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

INHIBIT_DEFAULT_DEPS = "1"
DEPENDS = "u-boot-mkimage-native"

SRC_URI = " \
  file://boot.cmd \
"

UBOOT_ENV = "boot"
UBOOT_ENV_SUFFIX = "scr"

inherit deploy nopackages

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_mkimage() {
    mkimage -C none -A arm -T script -d ${WORKDIR}/boot.cmd ${B}/${UBOOT_ENV}.${UBOOT_ENV_SUFFIX}
}

addtask mkimage after do_compile before do_install

do_install() {
    install -Dm 0644 ${B}/boot.scr ${D}/boot.scr
}

do_deploy() {
    install -Dm 0644 ${D}/boot.scr ${DEPLOYDIR}/boot.scr-${MACHINE}-${PV}-${PR}
    cd ${DEPLOYDIR}
    rm -f boot.scr-${MACHINE}
    ln -sf boot.scr-${MACHINE}-${PV}-${PR} boot.scr-${MACHINE}
}

addtask deploy after do_install before do_build

PROVIDES += "u-boot-script"

PACKAGE_ARCH = "${MACHINE_ARCH}"