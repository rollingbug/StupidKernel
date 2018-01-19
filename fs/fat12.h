/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   fat12.h
 * File Desc:   function for fat12 file system operation
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */
#ifndef _FAT12_H_
#define _FAT12_H_

#include "../types.h"

#define FAT12_FS_DEVICE                     0           /* floppy disc */


#define FAT12_BOOT_SECTOR                   0
#define FAT12_FAT_SECTOR                    1

#define FAT12_FS_ID                         "  FAT12"
#define FAT12_READ                          0x01
#define FAT12_WRITE                         0x02

#define FAT12_PATH_DLM                      '/'         /* delimiter for fat12 file path */
#define FAT12_EXT_DLM                       '.'         /* delimiter for fat12 file extension name */
#define FAT12_DIR_DOT                       '.'

#define FAT12_FILENAME_LEN_MAX              8
#define FAT12_EXTNAME_LEN_MAX               3

#define FAT12_END                           0xFFF

#define FAT12_FDB_SIZE                      32
#define FAT12_FDB_READ_ONLY                 0x01
#define FAT12_FDB_HIDDEN                    0x02
#define FAT12_FDB_SYSTEM                    0x04
#define FAT12_FDB_VOLUMN                    0x08
#define FAT12_FDB_LONG_FILENAME             0x0F
#define FAT12_FDB_DIRECTORY                 0x10
#define FAT12_FDB_ARCHIVE                   0x20

#define FAT12_FDB_UNUSE                     0x00
#define FAT12_FDB_DELETED                   0xE5

/*
 * convert cluster index to sector num,
 * sector num = fat12 boot sector size + total fat sector size
 *            + root fdb sector size + cluster offset
 */
#define FAT12_CLUSTER_TO_SECTOR(p_fat12_fs_info, cluster_idx)                               \
            (                                                                               \
                1                                                                           \
                + p_fat12_fs_info->fat_num * p_fat12_fs_info->fat_sector_size               \
                + p_fat12_fs_info->fdb_sector_size                                          \
                + (cluster_idx - 2) * p_fat12_fs_info->sectors_per_cluster                  \
             )

#define FAT12_IS_FDB_TYPE_MATCH(p_fdb, path_type)                                           \
            (                                                                               \
                (path_type == FAT12_PATH_DIR && p_fdb->attribute == FAT12_FDB_DIRECTORY) || \
                (path_type == FAT12_PATH_FILE && p_fdb->attribute != FAT12_FDB_DIRECTORY)   \
            )

enum PATH_TYPE{
    FAT12_PATH_UNKNOW = 0,
    FAT12_PATH_DIR,
    FAT12_PATH_FILE
};

/* extended parameter */
typedef struct fat12_ext_param{
    INT8U phy_drive_num;
    INT8U reserved;
    INT8U signature;
    INT32U volume_id;
    INT8U volume_label[11];
    INT8U file_system_type[8];
}__attribute__((packed)) FAT12_EXT_PARAM;

/* header of fat12 */
typedef struct fat12_hdr{
    INT8U jump[3];
    INT8U oem_name[8];
    INT16U bytes_per_sector;
    INT8U sectors_per_cluster;
    INT16U resv_sectors_num;
    INT8U fats;
    INT16U root_dir_entries;
    INT16U total_sectors_num;
    INT8U media_desc;
    INT16U sectors_per_fat;
    INT16U sectors_per_track;
    INT16U head_num;
    INT16U hidden_sectors_num;
    INT32U fat32_total_sectors_num;
    struct fat12_ext_param ext;
}__attribute__((packed)) FAT12_HDR;

typedef struct fat12_fdb{
    INT8U name[8];
    INT8U extension[3];
    INT8U attribute;
    INT8U reserved[10];
    INT16U time;
    INT16U date;
    INT16U start_cluster;
    INT32U file_length;
}__attribute__ ((packed)) FAT12_FDB;

/* long file name */
typedef struct fat12_lfn{
    INT8U end:4;
    INT8U order:4;
    INT8U name1[10];
    INT8U attribute;
    INT8U entry_type;
    INT8U checksum;
    INT8U name2[12];
    INT8U reserved[2];
    INT8U name3[4];
}__attribute__ ((packed)) FAT12_LFN;

typedef struct fat12_fs_info{
    INT16U bytes_per_sector;
    INT8U sectors_per_cluster;
    INT32U fat_start_sector;
    INT32U fat_sector_size;
    INT8U fat_num;
    INT8U *fat_data;
    INT32U fdb_start_sector;
    INT32U fdb_sector_size;
    INT32U fdb_root_entry_num;
    FAT12_FDB *root_fdb_data;
}FAT12_FS_INFO;

typedef struct far12_fs_file{
    INT32U dir_cluster_idx;
    INT32U fdb_idx;
    struct fat12_fdb fdb;
}FAT12_FS_FILE;

typedef struct fat12_fs_list{
    INT8S *p_name;
    struct far12_fs_file file;
    struct fat12_fs_list *p_next;
}FAT12_FS_LIST;

INT8S fat12_init();

#endif
