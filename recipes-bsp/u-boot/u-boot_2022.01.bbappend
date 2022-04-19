FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " \
    file://001-add-ethernet-to-v3s.dtsi.patch \
"

SRC_URI_append_licheepizero-dock = " \
    file://003-add-zero-dock_defconfig.patch \
    file://004-add-zero-dock.dts.patch \
    file://005-enable-ethernet-at-zero-dock.patch \
    file://006-add-nor-flash-at-zero.dts.patch \
    file://007-config-add-LicheePi_Zero_Dock_nor.patch \
"
