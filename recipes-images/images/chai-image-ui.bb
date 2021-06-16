inherit chai-image

TSLIB = " \
    tslib \
    tslib-calibrate \
    tslib-conf \
    tslib-dev \
    tslib-tests \
"

IMAGE_INSTALL_append = " \
    alsa-utils \
    htop \
    ${TSLIB} \
    x-window-example \
"

IMAGE_FEATURES += " \
    splash \
    x11-base \
"

KERNEL_EXTRA_INSTALL += "\
    kernel-modules \
    sunxi-mali \
"
