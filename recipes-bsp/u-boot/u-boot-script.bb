LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

INHIBIT_DEFAULT_DEPS = "1"
DEPENDS = "u-boot-mkimage-native"

SRC_URI = " \
  file://boot.cmd \
"

UBOOT_ENV = "boot"
UBOOT_ENV_SUFFIX = "scr"
UBOOT_ENV_BINARY = "${UBOOT_ENV}.${UBOOT_ENV_SUFFIX}"

inherit deploy nopackages

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_mkimage() {
    mkimage -C none -A arm -T script -d ${WORKDIR}/boot.cmd ${B}/${UBOOT_ENV_BINARY}
}

addtask mkimage after do_compile before do_install

do_install() {
    install -Dm 0644 ${B}/${UBOOT_ENV_BINARY} ${D}/${UBOOT_ENV_BINARY}
}

do_deploy() {
    install -Dm 0644 ${D}/${UBOOT_ENV_BINARY} ${DEPLOYDIR}/${UBOOT_ENV_BINARY}-${MACHINE}-${PV}-${PR}
    cd ${DEPLOYDIR}
    rm -f ${UBOOT_ENV_BINARY}-${MACHINE}
    ln -sf ${UBOOT_ENV_BINARY}-${MACHINE}-${PV}-${PR} ${UBOOT_ENV_BINARY}
}

addtask deploy after do_install before do_build

PROVIDES += "u-boot-script"

PACKAGE_ARCH = "${MACHINE_ARCH}"