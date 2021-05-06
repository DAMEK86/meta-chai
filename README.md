# meta-chai

yocto layer for lichee-pi zero (Allwinner V3s) based on mainline u-boot 2021.xx and kernel 5.x

for easy start
```bash
repo init -u git@github.com:DAMEK86/lichee-manifests.git
repo sync
```

## How to use

For instance if you like to build variant `licheepizero-dock`, you can start with the following bash script:

```bash
export TEMPLATECONF=$PWD/sources/meta-chai/conf/variant/licheepizero-dock
source sources/poky/oe-init-build-env build

bitbake chai-image
```

## how to use zero-dock mic

First of all, the v3s audio coded has no MIC1 bias. Instead, it looks to me like an differential input. I was able to get the mic working by changing the MIC setting from single ended to Left/Right within `alsamixer`.

![ALSA settings](./docu/assets/alsa-settings.png)

For easy verification, use the _chai-image-audio_ with the alsa-tools. Just plugin your headphone, setup ALSA like the picture above and run `alsaloop` and verify the microphone 😎️

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
