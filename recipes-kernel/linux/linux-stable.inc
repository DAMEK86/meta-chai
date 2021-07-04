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
KERNEL_ENABLE_CGROUPS ?= "0"
KERNEL_ENABLE_NFS ?= "0"
KERNEL_ENABLE_SCSI ?= "0"
KERNEL_ENABLE_AXP20X ?= "0"
KERNEL_ENABLE_MTD ?= "1"

LOCALVERSION ?= ""
KCONFIG_MODE ?= "alldefconfig"
KMACHINE ?= "${MACHINE}"

# Pull in the devicetree files into the rootfs
#RDEPENDS_${KERNEL_PACKAGE_NAME}-base += "kernel-devicetree"

# Set the verbosity of kernel messages during runtime
# You can define CMDLINE_DEBUG in your local.conf or distro.conf to override this behaviour
CMDLINE_DEBUG ?= "loglevel=3"

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
kernel_conf_variable() {
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

    kernel_conf_variable CMDLINE "\"${CMDLINE} ${CMDLINE_DEBUG}\""

    kernel_conf_variable LOCALVERSION "\"${LOCALVERSION}\""
    kernel_conf_variable LOCALVERSION_AUTO n

    # /proc/config.gz
    # kernel_conf_variable IKCONFIG y
    # kernel_conf_variable IKCONFIG_PROC y

    kernel_conf_variable SYSFS_DEPRECATED n
    kernel_conf_variable SYSFS_DEPRECATED_V2 n
    kernel_conf_variable HOTPLUG y
    kernel_conf_variable UEVENT_HELPER y
    kernel_conf_variable UEVENT_HELPER_PATH \"\"
    kernel_conf_variable UNIX y
    kernel_conf_variable SYSFS y
    # kernel_conf_variable PROC_FS y
    kernel_conf_variable TMPFS y
    kernel_conf_variable INOTIFY_USER y
    kernel_conf_variable SIGNALFD y
    kernel_conf_variable TMPFS_POSIX_ACL y
    kernel_conf_variable BLK_DEV_BSG y
    kernel_conf_variable DEVTMPFS y
    kernel_conf_variable DEVTMPFS_MOUNT y
    # kernel_conf_variable DEBUG_FS y
    # kernel_conf_variable MEDIA_SUPPORT y
    # kernel_conf_variable FTRACE y
    # kernel_conf_variable MODULES y
    # kernel_conf_variable MODULE_UNLOAD y
    # kernel_conf_variable HIGHMEM y
    kernel_conf_variable PRINTK_TIME y

    kernel_conf_variable I2C y
    kernel_conf_variable I2C_MV64XXX y
    kernel_conf_variable I2C_CHARDEV y

    # Activate the configuration options for screen
    screen="${@bb.utils.contains("MACHINE_FEATURES", "screen", "1", "0", d)}"
    if [ "${screen}" = "1" ]; then
        kernel_conf_variable DRM y
        kernel_conf_variable DRM_SUN4I y
        kernel_conf_variable FB_SIMPLE y
        kernel_conf_variable FB_SSD1307 y
        kernel_conf_variable DRM_PANEL_SIMPLE y
        bbnote "screen cfg added"
    else
        kernel_conf_variable DRM n
        kernel_conf_variable FB n
    fi

    # Activate the configuration options for alsa
    alsa="${@bb.utils.contains("MACHINE_FEATURES", "alsa", "1", "0", d)}"
    if [ "${alsa}" = "1" ]; then
        kernel_conf_variable SOUND y
        kernel_conf_variable SND y
        kernel_conf_variable SND_SOC y
        kernel_conf_variable SND_SUN4I_CODEC y
        kernel_conf_variable SND_SUN8I_CODEC_ANALOG y
        bbnote "alsa cfg added"
    else
        kernel_conf_variable SOUND n
    fi

    # Activate the configuration options for touchscreen
    touchscreen="${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", "1", "0", d)}"
    if [ "${touchscreen}" = "1" ]; then
        kernel_conf_variable INPUT_TOUCHSCREEN y
        kernel_conf_variable TOUCHSCREEN_NS2009 y
        kernel_conf_variable BACKLIGHT_PWM y
        kernel_conf_variable PWM_SUN4I y
        bbnote "touchscreen cfg added"
    else
        kernel_conf_variable BACKLIGHT_CLASS_DEVICE n
        #kernel_conf_variable BACKLIGHT_PWM n
        kernel_conf_variable LCD_CLASS_DEVICE n
    fi

    # Newer inits like systemd need cgroup support
    if [ "${KERNEL_ENABLE_CGROUPS}" = "1" ]; then
        kernel_conf_variable CGROUP_SCHED y
        kernel_conf_variable CGROUPS y
        kernel_conf_variable CGROUP_NS y
        kernel_conf_variable CGROUP_FREEZER y
        kernel_conf_variable CGROUP_DEVICE y
        kernel_conf_variable CPUSETS y
        kernel_conf_variable PROC_PID_CPUSET y
        kernel_conf_variable CGROUP_CPUACCT y
        kernel_conf_variable RESOURCE_COUNTERS y
    fi

    if [ "${KERNEL_ENABLE_NFS}" = "1" ]; then
        kernel_conf_variable NFS_FS y
        kernel_conf_variable NFS_V3_ACL y
        kernel_conf_variable NFS_V4 y
        kernel_conf_variable ROOT_NFS y
    fi

    if [ "${KERNEL_ENABLE_SCSI}" = "1" ]; then
        kernel_conf_variable SCSI y
    fi

    if [ "${KERNEL_ENABLE_MTD}" = "1" ]; then
        kernel_conf_variable MTD y
        #kernel_conf_variable MTD_TESTS y
        kernel_conf_variable MTD_SWAP y
        kernel_conf_variable MTD_CFI y
        kernel_conf_variable MTD_JEDECPROBE y
        kernel_conf_variable MTD_CFI_INTELEXT y
        kernel_conf_variable MTD_CFI_AMDSTD y
        kernel_conf_variable MTD_CFI_STAA y
        kernel_conf_variable MTD_IMPA7 y
        kernel_conf_variable MTD_PLATRAM y
        kernel_conf_variable MTD_BLOCK2MTD y
        kernel_conf_variable MTD_SPI_NOR y
    fi

    if [ "${KERNEL_ENABLE_AXP20X}" = "1" ]; then
        kernel_conf_variable INPUT_AXP20X_PEK y
        kernel_conf_variable CHARGER_AXP20X y
        kernel_conf_variable BATTERY_AXP20X y
        kernel_conf_variable MFD_AXP20X_I2C y
        kernel_conf_variable MFD_AXP20X_RSB y
        kernel_conf_variable REGULATOR_AXP20X y
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