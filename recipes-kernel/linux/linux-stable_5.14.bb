require linux-stable.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

KERNEL_CONFIG_COMMAND = "oe_runmake_call -C ${S} CC="${KERNEL_CC}" O=${B} olddefconfig"

COMPATIBLE_MACHINE = "(licheepizero|licheepizero-dock)"

PR_append = ".chai"

LINUX_VERSION = "5.14"
LINUX_VERSION_EXTENSION = "-chai"

FILESEXTRAPATHS_prepend := "${THISDIR}/linux-stable-${LINUX_VERSION}:"

S = "${WORKDIR}/git"

PV = "5.14.7"
SRCREV = "56c0ace445bd3aad9b2bee1e9d54cffe37f22205"
SRC_URI_append = " \
    file://0001-arm-dts-sunxi-v3s-enable-audio-codec.patch \
    file://0002-arm-dts-sunxi-v3s-add-touchscreen-ns2009.patch \
    file://0003-arm-dts-sunxi-v3s-add-lcd_rgb666-and-pwm-pins.patch \
    file://002-add-original-lichee-pi-zero-lcd-display.dtsi.patch \
    file://0004-arm-dts-sunxi-v3s-add-LCD.patch \
    file://0005-ARM-dts-sun8i-v3s-Add-node-for-system-control.patch \
    file://0006-ARM-dts-sun8i-v3s-Add-video-engine-node.patch \
"
