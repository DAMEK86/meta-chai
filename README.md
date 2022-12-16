# meta-chai

yocto layer for lichee-pi zero (Allwinner V3s) based on mainline u-boot 2021.xx and kernel 5.x

for easy start

```bash
repo init -u git@github.com:DAMEK86/lichee-manifests.git
repo sync
```

## How to use

see [meta-nightowl](https://github.com/DAMEK86/meta-nightowl/blob/dunfell/README.md)

## enable/disable kernel features

depending on the enabled machine features, kernel features are added.

- alsa - sound and codec are baked in
- screen - FB are backed in
- touchscreen - PWM and Touchscreen for NS2009 (onboard) is backed in

you can add machine features by edit `conf/local.conf`
just use `MACHINE_FEATURES_append` or `MACHINE_FEATURES_remove`

## shutdown / reboot

The Zero and Zero-Dock uses an __EA3036__ PMIC with routed __enable__ pins to 5V. As a result, the power consumption will not drop to _zero_ on a `shutdown`. A possible workaround could be using a pin to drive a _self-holding circuit_.
`reboot` instead works as expected.

## u-boot / network boot

### tftp

on startup you can download kernel and corresponding device-tree over tftp

```bash
setenv ipaddr <licheeip>
tftp 0x42000000 <serverip>:zImage; tftp 0x43000000 <serverip>:sun8i-v3s-licheepi-zero-dock.dtb; bootz 0x42000000 - 0x43000000"
```

HINT: u-boot reads unique mac from chip

as a starting point, you can use this simple script to host a local tftp server in docker

```bash
docker run --rm -it -p 69:69/udp \
    -v ${PWD}/zImage:/tftpboot/zImage:ro \
    -v ${PWD}/sun8i-v3s-licheepi-zero-dock.dtb:/tftpboot/sun8i-v3s-licheepi-zero-dock.dtb:ro \
    darkautism/k8s-tftp
```

this image is based on a tiny golang implementation and works very well for my purposes, see [darkautism/k8s-tftp](https://github.com/darkautism/k8s-tftp)

### network boot / nfsroot

to enable network boot, you need to enable kernel features by adding `KERNEL_ENABLE_NFS = "1"` to your ____local.conf__.
after this, your kernel shall be able to mount a nfs as rootfs

use the kernel cmdline to tell the kernel we want to use a network rootfs. I used the following script in u-boot cmd for my testing:

```bash
setenv bootargs "console=ttyS0,115200 root=/dev/nfs ip=192.168.5.78:192.168.5.80:192.168.5.80:255.255.255.0:licheepizero-dock:eth0 nfsroot=192.168.5.80:/export,tcp,v3 rootwait panic=2 debug"
setenv origbootcmd "$bootcmd"
setenv ipaddr 192.168.5.78
setenv bootcmd "tftp 0x42000000 192.168.5.80:zImage; tftp 0x43000000 192.168.5.80:sun8i-v3s-licheepi-zero-dock.dtb; bootz 0x42000000 - 0x43000000"
run bootcmd
```

for a detailed description of nfsroot parameters see [kernel.org](https://www.kernel.org/doc/Documentation/filesystems/nfs/nfsroot.txt)

as a starting point, you can use *nfs-kernel-server*

as a reference have a look into [community.arm.com](https://community.arm.com/developer/tools-software/oss-platforms/w/docs/542/nfs-remote-network-userspace-using-u-boot) or
[linux-sunxi.org](http://linux-sunxi.org/How_to_boot_the_A10_or_A20_over_the_network#TFTP_booting)

## sunxi-tools / flash boot

sunxi-tools from github can be used for u-boot / kernel developing and nor-flash programming
__HINT:__ official prebuilt packages for ubuntu/debian seems to not working on ubuntu 20.x

## dev.rules
```bash
/etc/udev/rules.d/50-sunxi-fel.rules

SUBSYSTEMS=="usb", ATTRS{idVendor}=="1f3a", ATTRS{idProduct}=="efe8", MODE:="0666"
```

### how to build sunxi-tools

```bash
sudo apt install libfdt-dev libusb-1.0-0-dev
git clone git@github.com:linux-sunxi/sunxi-tools.git
cd sunxi-tools
make
```

### fel boot

```bash
sudo sunxi-tools/sunxi-fel -v uboot u-boot-sunxi-with-spl.bin \
    write 0x42000000 zImage \
    write 0x43000000 sun8i-v3s-licheepi-zero-dock.dtb \
    write 0x43100000 boot.scr
```

### program flash and boot from fel / flash

write kernel, dtb to flash and start uboot from fel cli

```bash
sudo ./sunxi-tools/sunxi-fel \
    spiflash-write 0x0e0000 sun8i-v3s-licheepi-zero-dock-licheepizero-dock.dtb \
    spiflash-write 0x100000 zImage-licheepizero-dock.bin
sudo ./sunxi-tools/sunxi-fel uboot u-boot-sunxi-with-spl.bin
```

### flashing ubi image

TODO: fix `/bin/sh: can't access tty; job control turned off` and use overlayfs  
if using tiny distro

```bash
# offset from u-boot 0x80000 (524288/1024=512K)
sunxi-fel -v -p spiflash-write 0x80000 smart-grid-image-licheepizero-dock.ubi
```

for booting from flash

```bash
sudo ./sunxi-tools/sunxi-fel spiflash-write 0x0 u-boot-sunxi-with-spl.bin
```

### erasing 16 MB flash

```bash
sf probe
sf erase 0x0 0x1000000
```

# using NAND instead of NOR Flash

After some research, i found out that nand boot from spi isn't in u-boot.  
However i found [Benedikt-Alexander Mokroß openwrt port](https://github.com/bamkrs/openwrt/tree/dolphinpi-spinand) to Allwinners V3s SoC, which was a huge burst for this part.

__Steps to bringup SPI-NAND:__
1) build SD-Card image and burn image to SD-Card.
2) enable `BOOT_DEV = "nand"` and build image again.
3) copy `<imagename>.ubi` to SD-Card
4) (TODO until know) perare u-boot for NAND load by running u-boot-spi-nand.sh. e.g. `./u-boot-spi-nand.sh mtd0.bin u-boot-sunxi-with-spl.bin-licheepizero-dock 2048 128`
5) copy `mtd0.bin` to SD-Card
6) boot SD-Card

__linux based__

```sh
# erase uboot nand section
mtd_debug erase /dev/mtd0 0x0 0x100000
# write nand
FILE=$1
mtd_debug write /dev/mtd0 0x0 $(ls -l ${FILE} | awk '{ print $5}') ${FILE}

# write ubi to mtd1
ubiformat /dev/mtd1 -f <ubi file>
```

__u-boot based__

```sh
# erase uboot nand section
mtd erase uboot
# write nand
load mmc 0:1 ${loadaddr} <u-boot-sunxi-with-spl.bin (pepared for nand)>
mtd write uboot ${loadaddr}
```
