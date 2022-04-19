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

```bash
setenv bootargs "console=ttyS0,115200 panic=5 rootwait ubi.mtd=1 ubi.block=0,rootfs ubi.block=0,data root=/dev/ubiblock0_2"
setenv bootcmd "sf probe 0 50000000; ubi part rootubi; ubi read ${kernel_addr_r} kernel; ubi read ${fdt_addr_r} dtb; ubi detach; bootz ${kernel_addr_r} - ${fdt_addr_r}"
run bootcmd
```

for a detailed description of nfsroot parameters see [kernel.org](https://www.kernel.org/doc/Documentation/filesystems/nfs/nfsroot.txt)

as a starting point, you can use *nfs-kernel-server*

as a reference have a look into [community.arm.com](https://community.arm.com/developer/tools-software/oss-platforms/w/docs/542/nfs-remote-network-userspace-using-u-boot) or
[linux-sunxi.org](http://linux-sunxi.org/How_to_boot_the_A10_or_A20_over_the_network#TFTP_booting)

## sunxi-tools / flash boot

sunxi-tools from github can be used for u-boot / kernel developing and nor-flash programming
__HINT:__ official prebuilt packages for ubuntu/debian seems to not working on ubuntu 20.x

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

for booting from flash

```bash
sudo ./sunxi-tools/sunxi-fel spiflash-write 0x0 u-boot-sunxi-with-spl.bin
```
