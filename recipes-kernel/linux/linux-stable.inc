SUMMARY = "Linux kernel"
SECTION = "kernel"
LICENSE = "GPLv2"

LIC_FILES_CHKSUM ?= "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

DEPENDS += "bc-native bison-native openssl-native util-linux-native xz-native rsync-native"

inherit kernel siteinfo
require recipes-kernel/linux/linux-yocto.inc

COMPATIBLE_MACHINE = "sun8i"
FILESEXTRAPATHS_prepend := "${THISDIR}/linux-stable:"

# Enable OABI compat for people stuck with obsolete userspace
ARM_KEEP_OABI ?= "0"

LOCALVERSION ?= ""
KCONFIG_MODE ?= "alldefconfig"
KMACHINE ?= "${MACHINE}"

# Pull in the devicetree files into the rootfs
#RDEPENDS_${KERNEL_PACKAGE_NAME}-base += "kernel-devicetree"

#KERNEL_CONFIG_COMMAND = "oe_runmake_call -C ${S} CC="${KERNEL_CC}" O=${B} olddefconfig"

KERNEL_EXTRA_ARGS += "LOADADDR=${UBOOT_ENTRYPOINT}"

S = "${WORKDIR}/git"

SRC_URI_append_sun8i = " \
    file://defconfig \
    file://001-modify-sun8i-v3s.dtsi.patch \
    file://002-add-original-lichee-pi-zero-lcd-display.dtsi.patch \
    file://003-add-original-lichee-pi-zero-lcd-touchscreen.dtsi.patch \
    file://004-modify-sun8i-v3s-licheepi-zero.dts.patch \
    file://005-modify-sun8i-v3s-licheepi-zero-dock.dts.patch \
    file://006-add-audio-codec-to-sun8i-v3s.dtsi.patch \
"

# Set a variable in .configure
# $1 - Configure variable to be set
# $2 - value [n/y/value]
kernel_configure_variable() {
    # Remove the config
    CONF_SED_SCRIPT="$CONF_SED_SCRIPT /CONFIG_$1[ =]/d;"
    if test "$2" = "n"
    then
        echo "# CONFIG_$1 is not set" >> ${B}/.config
    else
        echo "CONFIG_$1=$2" >> ${B}/.config
    fi
}

config_setup() {
    # From kernel.bbclass. Unfortunately, this is needed to support builds that
    # use devtool. The reason is as follows:
    #
    # - In devtool builds, externalsrc.bbclass gets inherited and sets a list of
    # SRCTREECOVEREDTASKS, which don't get run because they affect the source
    # tree and, when using devtool, we want the developer's changes to be the
    # single source of truth. kernel-yocto.bbclass adds do_kernel_configme to
    # SRCTREECOVEREDTASKS, so it doesn't run in a devtool build., In a normal
    # non-devtool build, do_kernel_configme creates ${B}.config.
    #
    # - Normally (e.g. in linux-yocto), it would be OK that do_kernel_configme
    # doesn't run, because the first few lines of do_configure in kernel.bbclass
    # populate ${B}.config from either ${S}.config (if it exists) for custom
    # developer changes, or otherwise from ${WORDIR}/defconfig.
    #
    # - In linux-raspberrypi, we add do_configure_prepend, which tweaks
    # ${B}.config. Since this runs *before* the kernel.bbclass do_configure,
    # ${B}.config doesn't yet exist and we hit an error. Thus we need to move
    # the logic from do_configure up to before our do_configure_prepend. Because
    # we are copying only a portion of do_configure and not the whole thing,
    # there is no clean way to do it using OE functionality, so we just
    # copy-and-paste.
    if [ "${S}" != "${B}" ] && [ -f "${S}/.config" ] && [ ! -f "${B}/.config" ]; then
        mv "${S}/.config" "${B}/.config"
    fi

    # Copy defconfig to .config if .config does not exist. This allows
    # recipes to manage the .config themselves in do_configure_prepend().
    if [ -f "${WORKDIR}/defconfig" ] && [ ! -f "${B}/.config" ]; then
        cp "${WORKDIR}/defconfig" "${B}/.config"
    fi
}

do_configure_prepend() {
    config_setup

    mv -f ${B}/.config ${B}/.config.patched
    CONF_SED_SCRIPT=""

    bbnote "${MACHINE_FEATURES}"

    kernel_configure_variable CMDLINE "\"${CMDLINE}\""

    kernel_configure_variable LOCALVERSION "\"${LOCALVERSION}\""
    kernel_configure_variable LOCALVERSION_AUTO n

    # /proc/config.gz
    kernel_configure_variable IKCONFIG y
    kernel_configure_variable IKCONFIG_PROC y

    kernel_configure_variable SYSFS_DEPRECATED n
    kernel_configure_variable SYSFS_DEPRECATED_V2 n
    kernel_configure_variable HOTPLUG y
    kernel_configure_variable UEVENT_HELPER_PATH \"\"
    kernel_configure_variable UNIX y
    kernel_configure_variable SYSFS y
    kernel_configure_variable PROC_FS y
    kernel_configure_variable TMPFS y
    kernel_configure_variable INOTIFY_USER y
    kernel_configure_variable SIGNALFD y
    kernel_configure_variable TMPFS_POSIX_ACL y
    kernel_configure_variable BLK_DEV_BSG y
    kernel_configure_variable DEVTMPFS y
    kernel_configure_variable DEVTMPFS_MOUNT y

    # Activate the configuration options for screen
    screen="${@bb.utils.contains("MACHINE_FEATURES", "screen", "1", "0", d)}"
    if [ "${screen}" = "1" ]; then
        kernel_configure_variable DRM y
        kernel_configure_variable DRM_SUN4I y
        kernel_configure_variable FB_SIMPLE y
        kernel_configure_variable FB_SSD1307 y
        kernel_configure_variable DRM_PANEL_SIMPLE y
        bbnote "screen cfg added"
    fi

    # Activate the configuration options for alsa
    alsa="${@bb.utils.contains("MACHINE_FEATURES", "alsa", "1", "0", d)}"
    if [ "${alsa}" = "1" ]; then
        kernel_configure_variable SOUND y
        kernel_configure_variable SND y
        kernel_configure_variable SND_SOC y
        kernel_configure_variable SND_SUN4I_CODEC y
        kernel_configure_variable SND_SUN8I_CODEC_ANALOG y
        bbnote "alsa cfg added"
    fi

    # Activate the configuration options for touchscreen
    touchscreen="${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", "1", "0", d)}"
    if [ "${touchscreen}" = "1" ]; then
        kernel_configure_variable INPUT_TOUCHSCREEN y
        kernel_configure_variable TOUCHSCREEN_NS2009 y
        kernel_configure_variable BACKLIGHT_PWM y
        kernel_configure_variable PWM_SUN4I y
        bbnote "touchscreen cfg added"
    fi

    # Keep this the last line
    # Remove all modified configs and add the rest to .config
    sed -e "${CONF_SED_SCRIPT}" < '${B}/.config.patched' >> '${B}/.config'
    rm -f ${B}/.config.patched

    yes '' | oe_runmake -C ${S} O=${B} oldconfig
}

do_configure_append() {
    if test -e scripts/Makefile.fwinst ; then
        sed -i -e "s:-m0644:-m 0644:g" scripts/Makefile.fwinst
    fi
}

do_install_append() {
    oe_runmake headers_install INSTALL_HDR_PATH=${D}${exec_prefix}/src/linux-${KERNEL_VERSION} ARCH=$ARCH
}

PACKAGES =+ "kernel-headers"
FILES_kernel-headers = "${exec_prefix}/src/linux*"


FILES_${KERNEL_PACKAGE_NAME}-base_append = " ${nonarch_base_libdir}/modules/${KERNEL_VERSION}/modules.builtin.modinfo"