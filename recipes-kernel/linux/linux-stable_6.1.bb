require linux-stable.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

KERNEL_CONFIG_COMMAND = "oe_runmake_call -C ${S} CC="${KERNEL_CC}" O=${B} olddefconfig"

COMPATIBLE_MACHINE = "(licheepizero|licheepizero-dock)"

PR:append = ".chai"

LINUX_VERSION = "6.1"
LINUX_VERSION_EXTENSION = "-chai"

FILESEXTRAPATHS:prepend := "${THISDIR}/linux-stable-5.15:"

S = "${WORKDIR}/git"

PV = "6.1"
SRCREV = "830b3c68c1fb1e9176028d02ef86f3cf76aa2476"
SRCREV_meta ?= "185bcfcbe480c742247d9117011794c69682914f"
SRC_URI:append = " \
    git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=meta;branch=yocto-6.1;destsuffix=${KMETA} \
    file://0001-arm-dts-sunxi-v3s-enable-audio-codec.patch \
    file://0002-arm-dts-sunxi-v3s-add-touchscreen-ns2009.patch \
    file://0003-arm-dts-sunxi-v3s-add-lcd_rgb666-and-pwm-pins.patch \
    file://002-add-original-lichee-pi-zero-lcd-display.dtsi.patch \
    file://0004-arm-dts-sunxi-v3s-add-LCD.patch \
    file://0005-ARM-dts-sun8i-v3s-Add-node-for-system-control.patch \
    file://0006-ARM-dts-sun8i-v3s-Add-video-engine-node.patch \
    file://0007-arm-dts-sun8i-v3s-enable-spi0_1.patch \
"
