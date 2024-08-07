# Bouffalolab LHAL Release v1.3.0

Change Log Since v1.2.0 Release

- [fix][efuse]fix bflb_ef_ctrl_write_common_trim putreg32 bug
- [fix]fix efuse trim read/write en address range error
- [fix][ef_ctrl] fix bflb_ef_ctrl_write_direct when pword=NULL, need to check program
- [fix][efuse] fix return para for bflb_efuse_get_chipid
- [fix] fix romapi return and input
- [update][cam] move some configs into feature control
- [update][config] add bl628 config
- [update][common] remove crc32 table
- [fix] remove some flash cfg for bl808 boot2
- [update] remove platform dma reg
- [fix][auadc] fix romapi name
- [update][adc] add adc clear fifo cmd
- [fix][adc] remove adc channel 11 in bl616
- [fix][trng] add lock for random
- [refactor] remove platform dma
- [feat] add process corner interface
- [fix][clock] fix build for bl628
- [fix][config] remove bl616 dma ch4~7
- [fix][auadc] Remove unsupported analog channels of AUADC.
- [fix][gpio] delete api of 32bits write to GPIO, because GPIO mode is set/clr
- [fix][dac] fix dac offset in bl602
- [update][gpio] add check for pin
- [fix][lhal] fix dma tc int mask

# Bouffalolab LHAL Release v1.2.0

Change Log Since v1.1.0 Release

- [update] change device string with macros
- [fix] modify flash cfg for reduce bl606p/bl808 boot2 code size
- [update] add assert for init api
- [update] fix  BFLG to BFLB, add callapi content
- [feat] add bflb name macros
- [update] add romapi macros for later chips
- [update] assert when get name fail
- [update][uart] add uart tx done api
- [fix][I2C] fix subaddr var as uint32_t
- [update][uart] add uart tx flush api
- [update][i2c] add I2C_M_WRITE macro
- [fix][flash]fix flash chip erase timeout value overflow issu
- [update][flash] update puya 1M-32M,XTX 8M-16M flash for bl602/bl702/bl702l/bl616
- [update][irq] patch for iot sdk
- [update][flash] make bflb_flash_set_cmds private
- [feat] add flash security register api

# Bouffalolab LHAL Release v1.1.0

Change Log Since v1.0.0 Release

- [fix][I2C] add set/get timing command, and fix default timing
- [fix][usb] rename BLFB to BFLB
- [fix][mjpeg] fix warning
- [update][sf_cfg] adjust api position
- [update][mjpeg] move mjpeg const array into global
- [update][sec] use const regbase for trng
- [update][adc] update adc convert by yongjin
- [update] update adc trim by yongjin
- [update][ef_ctrl] replace strlen api with bflb_ef_ctrl_strlen
- [fix][acomp] fix acomp return type
- [fix][ef_ctrl] fix memcmp to arch_memcmp
- [fix][efuse] rename bflb_ef_ctrl_get_device_info to bflb_efuse_get_device_info
- [refactor][aes] refactor aes hwkey config api
- [fix][efuse] make bflb_efuse_device_info_type common for all chips
- [fix][sf_ctrl]fix flash config magic macro not in tcm section issue
- [update][flash] config flash 2line by user
- [fix] add wdt and reduce flash cfg to reduce code size for boot2
- [fix] fix sf_ctrl_cmds_cfg and sf_ctrl_cfg_type
- [update][flash] add gt25q64a/gt25q16b flash cfg
- [update][flash] because winb_16jv cfg changed, update winb_16dv flash cfg
- [fix][config] remove get id api
- [feat][flash] add config for flash 2line
- [update][efuse] remove aes read write api
- [update][flash] add PY25Q16HB/PY25Q32HB/SK25E032 flash cfg
- [doc][rtc] add comment for rtc
- [feat][rtc] add utc time suppport
- [update]update hbn section
- [fix][dma] fix dma2 arr index
- [update][flash] update flash GT25Q32A config

# Bouffalolab LHAL Release v1.0.0

first release