# tools

## spl_tool

Use to create `u-boot-spl.bin.normal.out`

Input:

```
{U-BOOT_PATH}/spl/u-boot-spl.bin
```

Output:

```
u-boot-spl.bin.normal.out
```

For example:

```
./create_sbl {U-BOOT_PATH}/spl/u-boot-spl.bin 0x01010101
```



## visionfive2-uboot-fit-image.its

Use to create `visionfive2_fw_payload.img`

Input:

```
{U-BOOT_PATH}/tools/mkimage
{OPENSBI_PATH}/build/platform/generic/firmware/fw_payload.bin
visionfive2-uboot-fit-image.its
```

Output:

```
visionfive2_fw_payload.img
```

For example:

```
{U-BOOT_PATH}/tools/mkimage -f visionfive2-uboot-fit-image.its -A riscv -O u-boot -T firmware visionfive2_fw_payload.img
```

