# Print and set boot source
itest.b *0x28 == 0x00 && echo "U-boot loaded from SD" && rootdev="/dev/mmcblk0p2";
itest.b *0x28 == 0x02 && echo "U-boot loaded from eMMC or secondary SD" && setenv rootdev "/dev/mmcblk1p2";
itest.b *0x28 == 0x03 && echo "U-boot loaded from SPI" && rootdev="31:03"

setenv bootargs console=${console} root=${rootdev} rootwait panic=10 ${extra}

load mmc 0:1 ${kernel_addr_r} fitImage || load mmc 0:1 ${kernel_addr_r} boot/fitImage
bootm ${kernel_addr_r}