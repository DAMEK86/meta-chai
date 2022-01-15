HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
DESCRIPTION="Upstream's U-boot"
AUTHOR = "Andreas Rehn <rehn.andreas86@gmail.com>"

UBOOT_VERSION = "2022.01"

require u-boot-common.inc
require u-boot.inc

DEPENDS += "bc-native dtc-native python3-setuptools-native"

SRCREV = "d637294e264adfeb29f390dfc393106fd4d41b17"
