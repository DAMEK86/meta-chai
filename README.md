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

For easy verification, use the _chai-audio-image_ with the alsa-tools. Just plugin your headphone, setup ALSA like the picture above and run `alsaloop` and verify the microphone 😎️

## shutdown / reboot

The Zero and Zero-Dock uses an __EA3036__ PMIC with routed __enable__ pins to 5V. As a result, the power consumption will not drop to _zero_ on a `shutdown`. A possible workaround could be using a pin to drive a _self-holding circuit_.  
`reboot` instead works as expected.

## boot from flash

```bash
sudo ./sunxi-tools/sunxi-fel spiflash-write 0x0e0000 sun8i-v3s-licheepi-zero-dock-licheepizero-dock.dtb
sudo ./sunxi-tools/sunxi-fel spiflash-write 0x100000 zImage-licheepizero-dock.bin
sudo ./sunxi-tools/sunxi-fel uboot u-boot-sunxi-with-spl.bin
# or write spl to 0x0
# sudo ./sunxi-tools/sunxi-fel spiflash-write 0x0 u-boot-sunxi-with-spl.bin
```
