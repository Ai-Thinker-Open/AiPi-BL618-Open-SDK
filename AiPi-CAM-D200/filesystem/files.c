#define __FILES_C_
#include "files.h"

//fatfs
FATFS fs;
__attribute((aligned(8))) static uint32_t workbuf[4096];
MKFS_PARM fs_para = {
    .fmt = FM_FAT32,     /* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
    .n_fat = 1,          /* Number of FATs */
    .align = 0,          /* Data area alignment (sector) */
    .n_root = 1,         /* Number of root directory entries */
    .au_size = 512 * 32, /* Cluster size (byte) */
};
static uint8_t is_fatfs_init_ok = 0;

int filesystem_init(void)
{
    FRESULT ret;

    board_sdh_gpio_init();

    fatfs_sdh_driver_register();

    ret = f_mount(&fs, "/sd", 1);
    // ret = FR_NO_FILESYSTEM;

    if (ret == FR_NO_FILESYSTEM) {
        printf("No filesystem yet, try to be formatted...\r\n");

        ret = f_mkfs("/sd", &fs_para, workbuf, sizeof(workbuf));

        if (ret != FR_OK) {
            printf("fail to make filesystem %d\r\n", ret);
            // _CALL_ERROR();
            return ret;
        }

        if (ret == FR_OK) {
            printf("done with formatting.\r\n");
            printf("first start to unmount.\r\n");
            ret = f_mount(NULL, "/sd", 1);
            printf("then start to remount.\r\n");
        }
    } else if (ret != FR_OK) {
        printf("fail to mount filesystem,error= %d\r\n", ret);
        printf("SD card might fail to initialise.\r\n");
        // _CALL_ERROR();
        return ret;
    } else {
        printf("Succeed to mount filesystem\r\n");
    }

    if (ret == FR_OK) {
        printf("FileSystem cluster size:%d-sectors (%d-Byte)\r\n", fs.csize, fs.csize * 512);
    }

    is_fatfs_init_ok = 1;
    return 0;
}

int take_photo(uint8_t *Buf, uint32_t len)
{
    FRESULT ret;
    FIL fnew;
    UINT fnum;
    // uint32_t time_node;
    static creat_file_cnt = 0;
#define PHOTO_NAME_LEN 30
    uint8_t file_name[PHOTO_NAME_LEN] = { 0 };
    uint8_t *pData;
    uint32_t i = 0;

    memset(file_name, 0, PHOTO_NAME_LEN);
    snprintf(file_name, PHOTO_NAME_LEN, "/sd/%d.jpg", creat_file_cnt);
    creat_file_cnt++;
    ret = f_open(&fnew, file_name, FA_CREATE_ALWAYS | FA_WRITE);
    // ret = f_open(&fnew, "/sd/test_file.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (ret == FR_OK) {
        // time_node = (uint32_t)bflb_mtimer_get_time_ms();
        /*write into file*/
        // ret = f_write(&fnew, Buf, len, &fnum);
        pData = Buf;
        while (i < len) {
            if ((len - i) < 10 * 1024) {
                ret = f_write(&fnew, pData, (len - i), &fnum);
                i += (len - i);
                // pData += (len-i);
            } else {
                ret = f_write(&fnew, pData, 10 * 1024, &fnum);
                i += 10 * 1024;
                pData += 10 * 1024;
            }
        }

        /* close file */
        ret |= f_close(&fnew);
        /* get time */
        // time_node = (uint32_t)bflb_mtimer_get_time_ms() - time_node;

        if (ret == FR_OK) {
            printf("Write Succeed! photo cnt:%d\r\n", creat_file_cnt - 1);
            printf("Write data size:%d Byte, written size:%d B\r\n", len, fnum);
            // printf("Time:%dms, Write Speed:%d KB/s \r\n", time_node, ((sizeof(RW_Buffer) * i) >> 10) * 1000 / time_node);
        } else {
            printf("Fail to write files(%d)\n", ret);
            return;
        }
    } else {
        printf("Fail to open or create files: %d.\r\n", ret);
        return;
    }
}
