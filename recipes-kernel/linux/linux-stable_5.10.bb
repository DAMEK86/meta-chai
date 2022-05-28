require linux-stable.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

KERNEL_CONFIG_COMMAND = "oe_runmake_call -C ${S} CC="${KERNEL_CC}" O=${B} olddefconfig"

COMPATIBLE_MACHINE = "(licheepizero|licheepizero-dock)"

PR:append = ".chai"

LINUX_VERSION = "5.10"
LINUX_VERSION_EXTENSION = "-chai"

FILESEXTRAPATHS:prepend := "${THISDIR}/linux-stable-${LINUX_VERSION}:"

S = "${WORKDIR}/git"

PV = "5.10.108"
SRCREV = "9940314ebfc61cb7bc7fca4a0deed2f27fdefd11"
SRC_URI:append = " \
    file://001-modify-sun8i-v3s.dtsi.patch \
    file://002-add-original-lichee-pi-zero-lcd-display.dtsi.patch \
    file://003-add-original-lichee-pi-zero-lcd-touchscreen.dtsi.patch \
    file://004-modify-sun8i-v3s-licheepi-zero.dts.patch \
    file://005-modify-sun8i-v3s-licheepi-zero-dock.dts.patch \
    file://006-add-audio-codec-to-sun8i-v3s.dtsi.patch \
"
