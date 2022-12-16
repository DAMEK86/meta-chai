FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " \
    file://boot.cmd \
    file://001-add-ethernet-to-v3s.dtsi.patch \
"

SRC_URI:append:licheepizero-dock = " \
    file://004-add-zero-dock.dts.patch \
    file://005-enable-ethernet-at-zero-dock.patch \
    ${@bb.utils.contains('BOOT_DEV', 'spinor', '\
        file://006-add-nor-flash-at-zero.dts.patch \
        file://007-config-add-LicheePi_Zero_Dock_nor.patch \
    ', '', d)} \
    ${@bb.utils.contains('BOOT_DEV', 'nand', '\
        file://008-licheepi-zero-add-spi-nand.patch \
    ', '\
        file://003-add-zero-dock_defconfig.patch \
    ', d)} \
"
