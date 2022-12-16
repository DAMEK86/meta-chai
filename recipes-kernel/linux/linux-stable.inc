SUMMARY = "Linux kernel"
SECTION = "kernel"
LICENSE = "GPLv2"

LIC_FILES_CHKSUM ?= "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

DEPENDS += "bc-native bison-native openssl-native util-linux-native xz-native rsync-native"

inherit kernel siteinfo
require recipes-kernel/linux/linux-yocto.inc

COMPATIBLE_MACHINE = "sun8i"
FILESEXTRAPATHS:prepend := "${THISDIR}/linux-stable:"

# Enable OABI compat for people stuck with obsolete userspace
ARM_KEEP_OABI ?= "0"
KERNEL_ENABLE_CGROUPS ?= "0"
KERNEL_ENABLE_NFS ?= "0"
KERNEL_ENABLE_SCSI ?= "0"
KERNEL_ENABLE_AXP20X ?= "0"
KERNEL_ENABLE_MTD ?= "1"
KERNEL_ENABLE_V4L2 ?= "0"

LOCALVERSION ?= ""
KCONFIG_MODE ?= "alldefconfig"
KMACHINE ?= "${MACHINE}"

# Pull in the devicetree files into the rootfs
RDEPENDS_${KERNEL_PACKAGE_NAME}-base = ""

# Set the verbosity of kernel messages during runtime
# You can define CMDLINE_DEBUG in your local.conf or distro.conf to override this behaviour
CMDLINE_DEBUG ?= "loglevel=3"

KERNEL_EXTRA_ARGS += "LOADADDR=${UBOOT_ENTRYPOINT}"

S = "${WORKDIR}/git"

KMETA = "kernel-meta"
KCONF_BSP_AUDIT_LEVEL = "1"

SRC_URI:append:sun8i = " \
    git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git;branch=linux-${LINUX_VERSION}.y \
    file://defconfig \
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
    # - In linux-raspberrypi, we add do_configure:prepend, which tweaks
    # ${B}.config. Since this runs *before* the kernel.bbclass do_configure,
    # ${B}.config doesn't yet exist and we hit an error. Thus we need to move
    # the logic from do_configure up to before our do_configure:prepend. Because
    # we are copying only a portion of do_configure and not the whole thing,
    # there is no clean way to do it using OE functionality, so we just
    # copy-and-paste.
    if [ "${S}" != "${B}" ] && [ -f "${S}/.config" ] && [ ! -f "${B}/.config" ]; then
        mv "${S}/.config" "${B}/.config"
    fi

    # Copy defconfig to .config if .config does not exist. This allows
    # recipes to manage the .config themselves in do_configure:prepend().
    if [ -f "${WORKDIR}/defconfig" ] && [ ! -f "${B}/.config" ]; then
        cp "${WORKDIR}/defconfig" "${B}/.config"
    fi
}

do_configure:prepend() {
    config_setup

    mv -f ${B}/.config ${B}/.config.patched
    CONF_SED_SCRIPT=""

    bbplain "machine features: ${MACHINE_FEATURES}"

    kernel_conf_variable CMDLINE "\"${CMDLINE} ${CMDLINE_DEBUG}\""

    kernel_conf_variable LOCALVERSION "\"${LOCALVERSION}\""
    kernel_conf_variable LOCALVERSION_AUTO n

    # /proc/config.gz
    kernel_conf_variable IKCONFIG y
    kernel_conf_variable IKCONFIG_PROC y

    kernel_conf_variable SYSFS_DEPRECATED n
    kernel_conf_variable SYSFS_DEPRECATED_V2 n
    kernel_conf_variable HOTPLUG y
    kernel_conf_variable UEVENT_HELPER y
    kernel_conf_variable UEVENT_HELPER_PATH \"\"
    kernel_conf_variable UNIX y
    kernel_conf_variable SYSFS y
    kernel_conf_variable PROC_FS y
    kernel_conf_variable TMPFS y
    kernel_conf_variable INOTIFY_USER y
    kernel_conf_variable SIGNALFD y
    kernel_conf_variable TMPFS_POSIX_ACL y
    kernel_conf_variable BLK_DEV_BSG y
    kernel_conf_variable DEVTMPFS y
    kernel_conf_variable DEVTMPFS_MOUNT y
    kernel_conf_variable DEBUG_FS y
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
        kernel_conf_variable DRM_PANEL_SIMPLE y
        kernel_conf_variable DRM_SUN8I_MIXER y
        kernel_conf_variable FB y
        kernel_conf_variable FB_SIMPLE y
        kernel_conf_variable FB_SSD1307 y
        kernel_conf_variable BACKLIGHT_PWM y
        kernel_conf_variable PWM_SUN4I y
        bbplain "screen cfg added"
    else
        kernel_conf_variable DRM n
        kernel_conf_variable FB n
    fi

    if [ "${KERNEL_ENABLE_V4L2}" = "1" ]; then
        kernel_conf_variable STAGING_MEDIA y
        kernel_conf_variable MEDIA_SUPPORT y
        kernel_conf_variable V4L_PLATFORM_DRIVERS y
        kernel_conf_variable VIDEO_SUNXI y
        kernel_conf_variable VIDEO_SUNXI_CEDRUS y
        kernel_conf_variable VIDEO_DEV y
        kernel_conf_variable MEDIA_SUPPORT_FILTER n
        kernel_conf_variable VIDEO_V4L2 y
    fi

    # Activate the configuration options for alsa
    alsa="${@bb.utils.contains("MACHINE_FEATURES", "alsa", "1", "0", d)}"
    if [ "${alsa}" = "1" ]; then
        kernel_conf_variable SOUND y
        kernel_conf_variable SND y
        kernel_conf_variable SND_SOC y
        kernel_conf_variable SND_SUN4I_CODEC y
        kernel_conf_variable SND_SUN8I_CODEC_ANALOG y
        bbplain "alsa cfg added"
    else
        kernel_conf_variable SOUND n
    fi

    # Activate the configuration options for touchscreen
    touchscreen="${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", "1", "0", d)}"
    if [ "${touchscreen}" = "1" ]; then
        kernel_conf_variable INPUT_TOUCHSCREEN y
        kernel_conf_variable TOUCHSCREEN_NS2009 y
        bbplain "touchscreen cfg added"
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

    spi_nand="${@bb.utils.contains("BOOT_DEV", "nand", "1", "0", d)}"
    if [ "${spi_nand}" = "1" ]; then
        kernel_conf_variable MTD_SPI_NAND=y
        kernel_conf_variable MTD_NAND_CORE=y
        kernel_conf_variable MTD_NAND_ECC=y
    fi

    spi_nor="${@bb.utils.contains("BOOT_DEV", "spinor", "1", "0", d)}"
    if [ "${spi_nor}" = "1" ]; then
        kernel_conf_variable MTD_SPI_NOR y
        kernel_conf_variable MTD_SPI_NOR_USE_4K_SECTORS y
    fi

    if [ "${KERNEL_ENABLE_MTD}" = "1" ]; then
        kernel_conf_variable MTD y

        kernel_conf_variable MTD_UBI y
        kernel_conf_variable MTD_UBI_WL_THRESHOLD=128
        kernel_conf_variable MTD_UBI_BEB_LIMIT=20
        kernel_conf_variable MTD_UBI_FASTMAP=y
        kernel_conf_variable MTD_UBI_BLOCK=n
        kernel_conf_variable MTD_UBI_GLUEBI=n
        kernel_conf_variable UBIFS_FS y
        kernel_conf_variable UBIFS_FS_ADVANCED_COMPR y
        kernel_conf_variable UBIFS_LZO=y

        kernel_conf_variable SQUASHFS y
        kernel_conf_variable SQUASHFS_FILE_DIRECT y
        kernel_conf_variable SQUASHFS_LZ4 y
        kernel_conf_variable OVERLAY_FS y
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

do_configure:append() {
    if test -e scripts/Makefile.fwinst ; then
        sed -i -e "s:-m0644:-m 0644:g" scripts/Makefile.fwinst
    fi
}

do_install:append() {
    oe_runmake headers_install INSTALL_HDR_PATH=${D}${exec_prefix}/src/linux-${KERNEL_VERSION} ARCH=$ARCH
}

PACKAGES =+ "kernel-headers"
FILES:kernel-headers = "${exec_prefix}/src/linux*"


FILES:${KERNEL_PACKAGE_NAME}-base:append = " ${nonarch_base_libdir}/modules/${KERNEL_VERSION}/modules.builtin.modinfo"