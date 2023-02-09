# Usage

## spl_tool

### How to create `u-boot-spl.bin.normal.out`

Please refer to the [README.md](./spl_tool/README.md) under spl_tool directory


## uboot_its

### How to create `visionfive2_fw_payload.img`

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

## recovery

### How to recover bootloader

The SPL and U-Boot are stored inside the SPI flash on board. There may be situations where you accidentally emptied the flash or if the flash is damaged on your board. In these situations, it's better to recovery the bootloader. 

Before you recover the bootloader, please double check the boot mode jumpers(Switch_2) on your board has already been switched to UART mode(GPIO_1,GPIO_0: 1,1);

1. Configure the serial port settings: 115200bps;

2. Power up, you will see an output like this:

   ```
   CCCCCCCCCCCCCCCCCCCCC
   ```

3. Please transfer the recovery binary **`jh7110-recovery-20221205.bin`** by xmode and you will see recovery menu automaticlly;
   ```   
   JH7110 secondboot version: 221205-74596a9
   CPU freq: 1250MHz
   idcode: 0x1860C8
   ddr 0x00000000, 4M test
   ddr 0x00400000, 8M test
   DDR clk 2133M, size 8GB
   *********************************************************
   ****************** JH7110 program tool ******************
   *********************************************************
   0: update 2ndboot/SPL in flash
   1: update 2ndboot/SPL in emmc
   2: update fw_verif/uboot in flash
   3: update fw_verif/uboot in emmc
   4: update otp, caution!!!!
   5: exit
   NOTE: current xmodem receive buff = 0x40000000, 'load 0x********' to change.
   select the function to test:
   ```
4. Type **`0`** and press Enter to udpate SPL binary **`u-boot-spl.bin.normal.out`**;

5. Type **`2`** and press Enter to update U-Boot binary **`visionfive2_fw_payload.img`**;

6. Power off and switch back to Flash mode(GPIO_1,GPIO_0: 0,0);


