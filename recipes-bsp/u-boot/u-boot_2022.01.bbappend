FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append:sun8i = " \
    file://001-add-ethernet-to-v3s.dtsi.patch \
"

SRC_URI:append:licheepizero-dock = " \
    file://003-add-zero-dock_defconfig.patch \
    file://004-add-zero-dock.dts.patch \
    file://005-enable-ethernet-at-zero-dock.patch \
    file://006-add-nor-flash-at-zero.dts.patch \
    file://nor-flash.cfg \
"

do_deploy[depends] += "u-boot-script:do_deploy"