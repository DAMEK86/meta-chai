require linux-stable.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

KERNEL_CONFIG_COMMAND = "oe_runmake_call -C ${S} CC="${KERNEL_CC}" O=${B} olddefconfig"

COMPATIBLE_MACHINE = "(licheepizero|licheepizero-dock)"

PR:append = ".chai"

LINUX_VERSION = "5.15"
LINUX_VERSION_EXTENSION = "-chai"

FILESEXTRAPATHS:prepend := "${THISDIR}/linux-stable-${LINUX_VERSION}:"

S = "${WORKDIR}/git"

PV = "5.15.80"
SRCREV = "71e496bd338221709b180b60ba419fa542c2b320"
SRCREV_meta ?= "fcf48627ea549df12be5d651521fc97a01b1986c"
SRC_URI:append = " \
    git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=meta;branch=yocto-5.15;destsuffix=${KMETA} \
    file://0001-arm-dts-sunxi-v3s-enable-audio-codec.patch \
    file://0002-arm-dts-sunxi-v3s-add-touchscreen-ns2009.patch \
    file://0003-arm-dts-sunxi-v3s-add-lcd_rgb666-and-pwm-pins.patch \
    file://002-add-original-lichee-pi-zero-lcd-display.dtsi.patch \
    file://0004-arm-dts-sunxi-v3s-add-LCD.patch \
    file://0005-ARM-dts-sun8i-v3s-Add-node-for-system-control.patch \
    file://0006-ARM-dts-sun8i-v3s-Add-video-engine-node.patch \
    ${@bb.utils.contains('BOOT_DEV', 'spinor', '\
        file://0007-arm-dts-sun8i-v3s-enable-spi0.patch \
    ', '', d)} \
    ${@bb.utils.contains('BOOT_DEV', 'nand', '\
        file://0008-arm-dts-sun8i-v3s-enable-spi0-nand.patch \
    ', '', d)} \
"
