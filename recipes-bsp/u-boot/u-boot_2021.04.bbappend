FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append_sun8i = " \
    file://boot.cmd \
    file://001-add-ethernet-to-v3s.dtsi.patch \
    file://002-add-ethernet-alias-at-zero.dts.patch \
    file://ethernet.cfg \
"

SRC_URI_append_licheepizero-dock = " \
    file://003-add-zero-dock_defconfig.patch \
    file://004-add-zero-dock.dts.patch \
    file://005-enable-ethernet-at-zero-dock.patch \
    file://006-add-nor-flash-at-zero.dts.patch \
    file://nor-flash.cfg \
"

UBOOT_ENV = "boot"
UBOOT_ENV_SUFFIX = "scr"

do_compile_append() {
    ${B}/tools/mkimage -C none -A arm -T script -d ${WORKDIR}/boot.cmd ${WORKDIR}/${UBOOT_ENV_BINARY}
}