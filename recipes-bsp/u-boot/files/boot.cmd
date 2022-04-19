# Print and set boot source
itest.b *0x28 == 0x00 && echo "U-boot loaded from SD" && rootdev="/dev/mmcblk0p2";
itest.b *0x28 == 0x02 && echo "U-boot loaded from eMMC or secondary SD" && setenv rootdev "/dev/mmcblk1p2";
# TODO: use ubifs and define needed partions
itest.b *0x28 == 0x03 && echo "U-boot loaded from SPI" && rootdev="31:03"

setenv bootargs console=${console} root=${rootdev} rootwait panic=10 ${extra}

# TODO: fel mode can load dtb and kernel to desired address
# TODO: handle boot from nor-flash (sf probe)
load mmc 0:1 ${fdt_addr_r} ${fdtfile} || load mmc 0:1 ${fdt_addr_r} boot/${fdtfile}
load mmc 0:1 ${kernel_addr_r} zImage || load mmc 0:1 ${kernel_addr_r} boot/zImage
bootz ${kernel_addr_r} - ${fdt_addr_r} || bootm ${kernel_addr_r} - ${fdt_addr_r}
