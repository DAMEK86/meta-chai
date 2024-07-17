FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " \
    file://boot.cmd \
"

SRC_URI:append:licheepizero-dock = " \
    file://004-add-zero-dock.dts.patch \
    ${@bb.utils.contains('BOOT_DEV', 'spinor', '\
        file://006-add-nor-flash-at-zero.dts.patch \
        file://007-config-add-LicheePi_Zero_Dock_nor.patch \
    ', '', d)} \
    ${@bb.utils.contains('BOOT_DEV', 'nand', '\
        file://008-licheepi-zero-add-spi-nand.patch \
        file://009-update-nand-driver.patch \
    ', '\
        file://003-add-zero-dock_defconfig.patch \
    ', d)} \
"
