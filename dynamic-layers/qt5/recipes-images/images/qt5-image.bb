DESCRIPTION = "A QT5 image with wifi support application for Lichee Pi Zero Zero-Dock Boards"
LICENSE = "MIT"

NETWORK_APP = " \
    openssh openssh-keygen openssh-sftp-server \
"

IMAGE_LINGUAS = "de-de"

inherit core-image

IMAGE_FEATURES += " \
    splash \
"

SYSTEM_TOOLS_INSTALL = " \
    i2c-tools \
    memtester \
    sysbench \
    tzdata \
    devmem2 \
    rsync \
    minicom \
"

AUDIO_TOOLS = " \
    alsa-utils \
"

QT_TOOLS = " \
    liberation-fonts \
    qtbase \
    qtbase-plugins \
    qtserialport \
    qt5-env \
"

QT_DEV_TOOLS = " \
    qtbase-dev \
    qtbase-mkspecs \
    qtbase-tools \
    qtserialport-dev \
    qtserialport-mkspecs \
    qtquickcontrols-qmlplugins \
"

KERNEL_EXTRA_INSTALL = " \
    kernel-devicetree \
    kernel-modules \
    sunxi-mali \
 "

DEV_TOOLS_INSTALL = " \
    mtd-utils \
"

NETWORK_TOOLS_INSTALL = " \
"


UTILITIES_INSTALL = " \
    coreutils \
    gdbserver \
    mtd-utils \
    ldd \
    libstdc++ \
    libstdc++-dev \
    openssh-sftp \
    ppp \
    tzdata \
"

TSLIB = " \
    tslib \
    tslib-calibrate \
    tslib-conf \
    tslib-dev \
    tslib-tests \
"

IMAGE_INSTALL += " \
  ${QT_DEV_TOOLS} \
  ${DEV_TOOLS_INSTALL} \
  ${SYSTEM_TOOLS_INSTALL} \
  ${UTILITIES_INSTALL} \
  ${NETWORK_APP} \
  ${QT_TOOLS} \
  ${KERNEL_EXTRA_INSTALL} \
  ${TSLIB} \
  ${AUDIO_TOOLS} \
  qt-simple-calc \
"

EXTRA_IMAGE_FEATURES = "debug-tweaks"

#Always add cmake to sdk
TOOLCHAIN_HOST_TASK_append = " nativesdk-cmake"

PACKAGECONFIG_DISTRO_append_pn_qtbase = " linuxfb tslib "
PACKAGECONFIG_FONTS_append_pn-qtbase = " fontconfig"

IMAGE_INSTALL_append = " qtbase qtbase-plugins "
