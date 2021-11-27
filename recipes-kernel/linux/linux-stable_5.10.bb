require linux-stable.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

KERNEL_CONFIG_COMMAND = "oe_runmake_call -C ${S} CC="${KERNEL_CC}" O=${B} olddefconfig"

COMPATIBLE_MACHINE = "(licheepizero|licheepizero-dock)"

PR_append = ".chai"

LINUX_VERSION = "5.10"
LINUX_VERSION_EXTENSION = "-chai"

FILESEXTRAPATHS_prepend := "${THISDIR}/linux-stable-${LINUX_VERSION}:"

S = "${WORKDIR}/git"

PV = "5.10.82"
SRCREV = "d5259a9ba6993a843278203323902bc0c049097e"
SRC_URI_append = " \
    file://001-modify-sun8i-v3s.dtsi.patch \
    file://002-add-original-lichee-pi-zero-lcd-display.dtsi.patch \
    file://003-add-original-lichee-pi-zero-lcd-touchscreen.dtsi.patch \
    file://004-modify-sun8i-v3s-licheepi-zero.dts.patch \
    file://005-modify-sun8i-v3s-licheepi-zero-dock.dts.patch \
    file://006-add-audio-codec-to-sun8i-v3s.dtsi.patch \
"
