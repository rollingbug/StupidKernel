/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   fat12.c
 * File Desc:   function for fat12 file system operation
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */

#include "fat12.h"
#include "../kernel.h"
#include "../drivers/floppy.h"

INT8U fat12_fs_device;

FAT12_FS_INFO *p_fat12_main_fs_info;

/* get fat12 format header */
INT8S fat12_get_info(FAT12_HDR *p_fat12_info)
{
    INT8U buffer[512];

    if(floppy_read(fat12_fs_device, FAT12_BOOT_SECTOR, 1, buffer) == -1)
        return -1;

    memcpy(p_fat12_info, buffer, sizeof(FAT12_HDR));

    return 0;
}

/* check file system type */
INT8S fat12_check_type(FAT12_HDR *p_fat12_info)
{
    INT8S is_match;

    is_match = memcmp((void *)&(p_fat12_info->ext.file_system_type),
                      (void *)FAT12_FS_ID,
                      sizeof(FAT12_FS_ID) - 1);

    if(is_match == 0)
        return 0;
    else
        return -1;
}

/* access floppy fat table */
INT8S fat12_rw_fat_table(FAT12_FS_INFO *p_fat12_fs_info, INT8U rw_mode)
{
    if(p_fat12_fs_info == NULL)
        return -1;
    if(p_fat12_fs_info->fat_data == NULL)
        return -1;

    if(rw_mode == FAT12_READ)
        return floppy_read(fat12_fs_device, p_fat12_fs_info->fat_start_sector, p_fat12_fs_info->fat_sector_size, p_fat12_fs_info->fat_data);
    else if(rw_mode == FAT12_WRITE)
        return floppy_write(fat12_fs_device, p_fat12_fs_info->fat_start_sector, p_fat12_fs_info->fat_sector_size, p_fat12_fs_info->fat_data);

    return -1;
}

/* access floppy fdb table */
INT8S fat12_rw_root_fdb(FAT12_FS_INFO *p_fat12_fs_info, INT8U rw_mode)
{
    if(p_fat12_fs_info == NULL)
        return -1;
    if(p_fat12_fs_info->root_fdb_data == NULL)
        return -1;

    if(rw_mode == FAT12_READ)
        return floppy_read(fat12_fs_device, p_fat12_fs_info->fdb_start_sector, p_fat12_fs_info->fdb_sector_size, (void *)p_fat12_fs_info->root_fdb_data);
    else if(rw_mode == FAT12_WRITE)
        return floppy_write(fat12_fs_device, p_fat12_fs_info->fdb_start_sector, p_fat12_fs_info->fdb_sector_size, (void *)p_fat12_fs_info->root_fdb_data);

    return -1;
}

/* access floppy cluster */
INT8S fat12_rw_cluster(FAT12_FS_INFO *p_fat12_fs_info, INT8U rw_mode, INT32U cluster_idx, void *p_buffer)
{
    INT32U sector_num;

    if(p_fat12_fs_info == NULL)
        return -1;
    if(p_buffer == NULL)
        return -1;
    if(cluster_idx < 2)
        return -1;

    sector_num = FAT12_CLUSTER_TO_SECTOR(p_fat12_fs_info, cluster_idx);

    if(rw_mode == FAT12_READ)
        return floppy_read(fat12_fs_device, sector_num , p_fat12_fs_info->sectors_per_cluster, p_buffer);
    else if(rw_mode == FAT12_WRITE)
        return floppy_write(fat12_fs_device, sector_num, p_fat12_fs_info->sectors_per_cluster, p_buffer);

    return -1;
}

/* get next fat index */
INT16S fat12_get_next_fat(FAT12_FS_INFO *p_fat12_fs_info, INT32U fat_idx)
{
    INT32U fat_offset;
    INT16U fat_data;

    if(p_fat12_fs_info == NULL)
        return -1;

    fat_offset = fat_idx / 2 * 3;
    if(fat_offset + 1 > p_fat12_fs_info->fat_sector_size * p_fat12_fs_info->bytes_per_sector)
        return -1;

    if(fat_idx % 2 == 0)
        fat_data = *((INT16U *)(&p_fat12_fs_info->fat_data[fat_offset])) & 0x0FFF;
    else
        fat_data = *((INT16U *)(&p_fat12_fs_info->fat_data[fat_offset + 1])) >> 4;

    return fat_data;
}

/* get length of filename and extension name */
INT8S fat12_get_filename_len(INT8S *p_filename, INT32U *p_name_len, INT32U *p_ext_len)
{
    INT8U is_ext;

    if(p_filename == NULL)
        return -1;
    if(p_name_len == NULL)
        return -1;
    if(p_ext_len == NULL)
        return -1;

    *p_name_len = 0;
    *p_ext_len = 0;
    is_ext = FALSE;

    /* check '.' and '..' */
    if(p_filename[0] == FAT12_DIR_DOT){
        if(p_filename[1] == '\0'){
            *p_name_len = 1;
            return 0;
        }
        else if(p_filename[1] == FAT12_DIR_DOT && p_filename[2] == '\0'){
            *p_name_len = 2;
            return 0;
        }
    }

    while(*p_filename != '\0'){

        if(*p_filename == FAT12_EXT_DLM)
            is_ext = TRUE;
        else if(is_ext == TRUE)
            (*p_ext_len)++;
        else
            (*p_name_len)++;

        p_filename++;
    }

    return 0;
}

/* file system path parser */
INT8U fat12_path_parser(INT8S **p_path, INT8S **p_name)
{
    INT32U name_size;
    INT8U path_type;

    if(p_path == NULL || *p_path == NULL || p_name == NULL)
        return FAT12_PATH_UNKNOW;

    /* skip the file path dilimiter */
    while(**p_path == FAT12_PATH_DLM)
        (*p_path)++;

    *p_name = *p_path;
    name_size = 0;

    /* get the filename length */
    while(**p_path != FAT12_PATH_DLM && **p_path != '\0'){
        (*p_path)++;
        name_size++;
    }

    /* determine file path type */
    if(name_size == 0)
        path_type = FAT12_PATH_UNKNOW;
    else if(**p_path == FAT12_PATH_DLM)
        path_type = FAT12_PATH_DIR;
    else
        path_type = FAT12_PATH_FILE;

    if(**p_path != '\0'){
        **p_path = '\0';
        (*p_path)++;
    }

    return path_type;
}

/* convert LFN struct to array format */
INT8U fat12_longname_convert(FAT12_LFN *p_lfn, INT8S *p_buffer)
{
    INT8S *lfn_name[3];
    INT8U *p_name;
    INT8U lfn_name_len[3];
    INT8U final_name_len;
    INT8U tmp_cnt;
    INT8U tmp_idx;
    INT8U is_end;

    if(p_lfn == NULL)
        return 0;
    if(p_buffer == NULL)
        return 0;

    lfn_name[0] = p_lfn->name1;
    lfn_name[1] = p_lfn->name2;
    lfn_name[2] = p_lfn->name3;

    lfn_name_len[0] = 10;
    lfn_name_len[1] = 12;
    lfn_name_len[2] = 4;

    is_end = FALSE;
    tmp_cnt = 0;
    final_name_len = 0;
    while(tmp_cnt < 3){
        p_name = lfn_name[tmp_cnt];

        for(tmp_idx = 0; tmp_idx < lfn_name_len[tmp_cnt]; tmp_idx++){
            if(p_name[tmp_idx] == 0xFF){
                is_end = TRUE;
                break;
            }
            if(p_name[tmp_idx] != '\0')
                p_buffer[final_name_len++] = p_name[tmp_idx];
        }

        if(is_end == TRUE)
            break;

        tmp_cnt++;
    }

    p_buffer[final_name_len] = '\0';

    return final_name_len;
}

/* compare FDB filename */
INT8S fat12_cmp_filename(FAT12_FDB *p_fdb, INT8S *p_name)
{
    INT32U name_len;
    INT32U ext_len;
    INT8U is_long_filename;
    INT8U is_match;
    INT8S cmp_name_buf[26 + 1];
    INT32U cmp_len;
    FAT12_LFN *p_lfn;

    if(p_fdb == NULL)
        return FALSE;
    if(p_name == NULL)
        return FALSE;

    /* get file name and extension name */
    fat12_get_filename_len(p_name, &name_len, &ext_len);
    if(name_len == 0)
        return FALSE;

    /* determine whether the fine name is long name format */
    if(name_len > FAT12_FILENAME_LEN_MAX || ext_len > FAT12_EXTNAME_LEN_MAX)
        is_long_filename = TRUE;
    else
        is_long_filename = FALSE;

    is_match = FALSE;

    /* long file name compare */
    if(is_long_filename == TRUE){

        /* the full long file name length = name len + ext len + ext delimiter len */
        if(ext_len != 0)
            name_len += (ext_len + 1);

        /* get long file name descriptor */
        p_lfn = (FAT12_LFN *)p_fdb - 1;

        /* now start to scan and compare each long file name block */
        while(p_lfn->attribute == FAT12_FDB_LONG_FILENAME){
            is_match = TRUE;

            cmp_len = fat12_longname_convert(p_lfn, cmp_name_buf);

            if(name_len < cmp_len){
                is_match = FALSE;
                break;
            }

            if(memcmp(p_name, cmp_name_buf, cmp_len) != 0){
                is_match = FALSE;
                break;
            }

            if(p_lfn->end == 0x4)
                break;

            p_name += cmp_len;
            name_len -= cmp_len;
            p_lfn--;
        }

        /* although the file name compare is finished, but we still need to check file name length */
        if(name_len != 0)
            is_match = FALSE;
    }
    /* standard file name compare */
    else{
        /* convert file name to upcase */
        p_name = (INT8S *)strupr(p_name);

        /* compare file name */
        if(memcmp(p_name, p_fdb->name, name_len) == 0)
            is_match = TRUE;
        else
            is_match = FALSE;

        /* compare extension name */
        if(ext_len != 0 && is_match == TRUE){

            /* skip filename and delimiter */
            p_name += (name_len + 1);
            if(memcmp(p_name, p_fdb->extension, ext_len) != 0)
                is_match = FALSE;
        }
    }

    return is_match;
}

/* get full file name from fdb */
INT8S fat12_get_file_name(FAT12_FDB *p_fdb, INT8S *p_name)
{
    FAT12_LFN *p_lfn;
    INT32U name_len;
    INT8S name_buf[26 + 1];
    INT8S *p_tmp;
    INT8U tmp_idx;

    if(p_fdb == NULL || p_name == NULL)
        return -1;

    p_lfn = (FAT12_LFN*)(p_fdb - 1);

    /* get standard name */
    if(p_lfn->attribute != FAT12_FDB_LONG_FILENAME){
        name_len = 0;

        tmp_idx = 0;
        p_tmp = p_fdb->name;
        while(*p_tmp != ' ' && tmp_idx < FAT12_FILENAME_LEN_MAX){
            *p_name++ = tolower(*p_tmp++);
            tmp_idx++;
        }

        if(p_fdb->attribute != FAT12_FDB_DIRECTORY){
            *p_name++ = FAT12_EXT_DLM;

            tmp_idx = 0;
            p_tmp = p_fdb->extension;
            while(*p_tmp != ' ' && tmp_idx < FAT12_EXTNAME_LEN_MAX){
                *p_name++ = tolower(*p_tmp++);
                tmp_idx++;
            }
        }
    }
    /* get long name */
    else{
        while(p_lfn->attribute == FAT12_FDB_LONG_FILENAME){

            name_len = fat12_longname_convert(p_lfn, name_buf);
            memcpy(p_name, name_buf, name_len);

            if(p_lfn->end == 0x4)
                break;

            p_name += name_len;
            p_lfn--;
        }
    }

    *p_name = '\0';

    return 0;
}

/* search fdb by file path */
INT8S fat12_search_fdb(FAT12_FS_INFO *p_fat12_fs_info, INT8S *p_path, INT32U curt_dir_idx, FAT12_FS_FILE *p_match_file)
{
    INT8U path_type;
    INT8U is_match;
    INT8U *p_cluster;
    FAT12_FDB *p_fdb;
    INT32U fdb_idx;
    INT32U fdb_max;
    INT16S curt_fat;
    INT8S *p_name;

    if(p_fat12_fs_info == NULL)
        return -1;
    if(p_path == NULL)
        return -1;
    if(p_match_file == NULL)
        return -1;

    curt_fat = curt_dir_idx;

    /* allocate memory for cluster */
    p_cluster = (INT8U *)vmalloc(p_fat12_fs_info->bytes_per_sector * p_fat12_fs_info->sectors_per_cluster);
    if(p_cluster == NULL)
        return -1;

    /* start from root dir or not? */
    if(curt_dir_idx < 2 || p_path[0] == FAT12_PATH_DLM){
        curt_dir_idx = 0;               /* root dir */
        p_fdb = p_fat12_fs_info->root_fdb_data;
    }
    else{
        /* load current dir entry cluster */
        if(fat12_rw_cluster(p_fat12_fs_info, FAT12_READ, curt_dir_idx, p_cluster) == -1){
            kfree(p_cluster);
            return -1;
        }

        p_fdb = (FAT12_FDB *)p_cluster;
    }

    /* start to extract file path and search fdb */
    while((path_type = fat12_path_parser(&p_path, &p_name)) != FAT12_PATH_UNKNOW){

        is_match = FALSE;

        /* if user specific local dir path or previous dir when current position is in root dir,
            then setup is_match to true and continue to scan next path */
        if(p_name[0] == FAT12_DIR_DOT){
            if(p_name[1] == '\0'){
                is_match = TRUE;
                continue;
            }
            else if(p_name[1] == FAT12_DIR_DOT && p_name[2] == '\0' && p_fdb->start_cluster < 2){
                is_match = TRUE;
                continue;
            }
        }

        /* scan from root dir or sub dir? */
        if(p_fdb->start_cluster < 2){
            fdb_max = p_fat12_fs_info->fdb_root_entry_num;
            p_fdb = p_fat12_fs_info->root_fdb_data;
        }
        else{
            fdb_max = p_fat12_fs_info->bytes_per_sector * p_fat12_fs_info->sectors_per_cluster / sizeof(FAT12_FDB);
            curt_dir_idx = p_fdb->start_cluster;

            /* load next cluster of dir */
            if(fat12_rw_cluster(p_fat12_fs_info, FAT12_READ, curt_dir_idx, p_cluster) == -1){
                vfree(p_cluster);
                return -1;
            }

            p_fdb = (FAT12_FDB *)p_cluster;
        }

        /* scan fdb */
        fdb_idx = 0;
        while(fdb_idx < fdb_max){

            /* is this the end of FDBs */
            if(p_fdb->name[0] == FAT12_FDB_UNUSE)
                break;

            /* compare fdb */
            if(p_fdb->name[0] != FAT12_FDB_DELETED && p_fdb->attribute != FAT12_FDB_LONG_FILENAME){
                if(FAT12_IS_FDB_TYPE_MATCH(p_fdb, path_type) == TRUE){
                    if(fat12_cmp_filename(p_fdb, p_name) == TRUE){
                        is_match = TRUE;
                        break;
                    }
                }
            }

            /* load next dir cluster */
            fdb_idx++;
            if(fdb_idx == fdb_max  && p_fdb->start_cluster >= 2){
                curt_fat = fat12_get_next_fat(p_fat12_fs_info, curt_fat);

                if(curt_fat != FAT12_END){
                    if(fat12_rw_cluster(p_fat12_fs_info, FAT12_READ, curt_fat, p_cluster) == -1){
                        vfree(p_cluster);
                        return -1;
                    }

                    fdb_max += p_fat12_fs_info->bytes_per_sector * p_fat12_fs_info->sectors_per_cluster / sizeof(FAT12_FDB);
                    p_fdb = (FAT12_FDB *)p_cluster;
                }
            }
            else
                p_fdb++;
        }

        if(is_match == FALSE)
            break;
    }

    if(is_match == TRUE){
        p_match_file->dir_cluster_idx = p_fdb->start_cluster;
        p_match_file->fdb_idx = fdb_idx;
        memcpy(&p_match_file->fdb, p_fdb, sizeof(FAT12_FDB));
    }

    vfree(p_cluster);

    return is_match;
}

/* release file list info */
void fat12_free_fs_list(FAT12_FS_LIST *p_list)
{
    FAT12_FS_LIST *p_tmp;

    while(p_list != NULL){
        p_tmp = p_list;
        p_list = p_list->p_next;

        vfree(p_tmp->p_name);
        vfree(p_tmp);
    }
}

/* get file info list of current dir  */
FAT12_FS_LIST *fat12_get_fs_list(FAT12_FS_INFO *p_fat12_fs_info, INT8S *p_path, INT32U curt_dir_idx)
{
    FAT12_FDB *p_fdb;
    FAT12_FS_FILE file;
    FAT12_FS_LIST *p_node;
    FAT12_FS_LIST *p_list_tail;
    FAT12_FS_LIST *p_list_head;
    INT16S curt_fat;
    INT8U is_root;
    INT32U fdb_idx;
    INT32U fdb_max;
    INT8U is_fail;
    INT8U *p_cluster;
    INT8S name_buf[22 * 15 + 1];        /* long file name buffer */

    if(p_fat12_fs_info == NULL)
        return NULL;
    if(p_path == NULL)
        return NULL;

    /* allocate memory for cluster */
    p_cluster = (INT8U *)vmalloc(p_fat12_fs_info->bytes_per_sector * p_fat12_fs_info->sectors_per_cluster);

    if(p_cluster == NULL)
        return NULL;

    /* root dir list */
    if(curt_dir_idx < 2)
        curt_dir_idx = 0;

    if((curt_dir_idx == 0 && p_path[0] == '\0') || p_path[0] == FAT12_PATH_DLM){
        curt_dir_idx = 0;
        p_fdb = p_fat12_fs_info->root_fdb_data;
        fdb_max = p_fat12_fs_info->fdb_root_entry_num;
        is_root = TRUE;
    }
    else{
        if(p_path[0] != '\0'){
            if(fat12_search_fdb(p_fat12_fs_info, p_path, curt_dir_idx, &file) != TRUE)
                return NULL;

            curt_dir_idx = file.fdb.start_cluster;
        }

        /* load dir cluster */
        if(fat12_rw_cluster(p_fat12_fs_info, FAT12_READ, curt_dir_idx, p_cluster) == -1){
            vfree(p_cluster);
            return NULL;
        }

        curt_fat = curt_dir_idx;
        p_fdb = (FAT12_FDB *)p_cluster;
        fdb_max = p_fat12_fs_info->bytes_per_sector * p_fat12_fs_info->sectors_per_cluster / sizeof(FAT12_FDB);

        is_root = FALSE;
    }

    p_list_tail = NULL;
    p_list_head = NULL;
    p_node = NULL;

    /* start to scan dir */
    is_fail = FALSE;
    fdb_idx = 0;
    while(fdb_idx < fdb_max){

        if(p_fdb->name[0] != FAT12_FDB_DELETED && p_fdb->attribute != FAT12_FDB_LONG_FILENAME){

            if(p_fdb->name[0] == FAT12_FDB_UNUSE)
                break;

            if(fat12_get_file_name(p_fdb, name_buf) == 0){

                p_node = (FAT12_FS_LIST *)vmalloc(sizeof(FAT12_FS_LIST));
                if(p_node == NULL){
                    is_fail = TRUE;
                    break;
                }

                p_node->p_name = (INT8S *)vmalloc(strlen(name_buf) + 1);
                if(p_node->p_name == NULL){
                    vfree(p_node);
                    is_fail = TRUE;
                    break;
                }

                memcpy(p_node->p_name, name_buf, strlen(name_buf) + 1);
                p_node->file.dir_cluster_idx = curt_dir_idx;
                p_node->file.fdb_idx = fdb_idx;
                memcpy(&p_node->file.fdb, p_fdb, sizeof(FAT12_FDB));
                p_node->p_next = NULL;

                /* add to list */
                if(p_list_head == NULL){
                    p_list_head = p_node;
                    p_list_tail = p_list_head;
                }
                else{
                    p_list_tail->p_next = p_node;
                    p_list_tail = p_list_tail->p_next;
                }
            }
        }

        p_fdb++;
        fdb_idx++;

        if(fdb_idx == fdb_max && is_root == FALSE){
            curt_fat = fat12_get_next_fat(p_fat12_fs_info, curt_fat);

            if(curt_fat != FAT12_END){
                /* load next cluster of dir */
                if(fat12_rw_cluster(p_fat12_fs_info, FAT12_READ, curt_fat, p_cluster) == -1){
                    is_fail = TRUE;
                    break;
                }

                p_fdb = (FAT12_FDB *)p_cluster;
                fdb_max += p_fat12_fs_info->bytes_per_sector * p_fat12_fs_info->sectors_per_cluster / sizeof(FAT12_FDB);;
            }
        }
    }

    vfree(p_cluster);

    if(is_fail == TRUE){
        fat12_free_fs_list(p_list_head);

        return NULL;
    }

    return p_list_head;
}

INT8S fat12_init()
{
    FAT12_HDR fat12_info;
    INT8U fs_create_success;

    fat12_fs_device = FAT12_FS_DEVICE;
    p_fat12_main_fs_info = NULL;
    fs_create_success = TRUE;

    /* get fat12 format header */
    if(fat12_get_info(&fat12_info) == -1){
        KERNEL_MSG(FALSE, TRUE, "cannot get file system type!\n");
        return -1;
    }

    /* check format type */
    if(fat12_check_type(&fat12_info) == -1){
        KERNEL_MSG(FALSE, TRUE, "file system type not match!\n");
        return -1;
    }

    /* initial file system information */
    p_fat12_main_fs_info = (FAT12_FS_INFO *)vmalloc(sizeof(FAT12_FS_INFO));
    if(p_fat12_main_fs_info == NULL){
        KERNEL_MSG(FALSE, TRUE, "fail to allocate memory for file system info!\n");
        return -1;
    }

    p_fat12_main_fs_info->bytes_per_sector = fat12_info.bytes_per_sector;
    p_fat12_main_fs_info->fat_start_sector = fat12_info.resv_sectors_num;
    p_fat12_main_fs_info->fat_sector_size = fat12_info.sectors_per_fat;
    p_fat12_main_fs_info->fat_num = fat12_info.fats;
    p_fat12_main_fs_info->fat_data = NULL;
    p_fat12_main_fs_info->sectors_per_cluster = fat12_info.sectors_per_cluster;
    p_fat12_main_fs_info->fdb_start_sector = p_fat12_main_fs_info->fat_start_sector + (p_fat12_main_fs_info->fat_sector_size * p_fat12_main_fs_info->fat_num);
    p_fat12_main_fs_info->fdb_sector_size = (fat12_info.root_dir_entries * FAT12_FDB_SIZE) / p_fat12_main_fs_info->bytes_per_sector;
    p_fat12_main_fs_info->fdb_root_entry_num = fat12_info.root_dir_entries;
    p_fat12_main_fs_info->root_fdb_data = NULL;

    /* allocate memory for fat and load fat from floppy */
    p_fat12_main_fs_info->fat_data = (INT8U *)vmalloc(p_fat12_main_fs_info->fat_sector_size * p_fat12_main_fs_info->bytes_per_sector);
    if(p_fat12_main_fs_info->fat_data == NULL){
        fs_create_success = FALSE;
        KERNEL_MSG(FALSE, TRUE, "fail to allocate memory for fat info!\n");
    }
    else if(fat12_rw_fat_table(p_fat12_main_fs_info, FAT12_READ) == -1){
        fs_create_success = FALSE;
        KERNEL_MSG(FALSE, TRUE, "fail to read fat info!\n");
    }

    /* allocate memory for fdb and load fdb from floppy */
    p_fat12_main_fs_info->root_fdb_data = (FAT12_FDB *)vmalloc(p_fat12_main_fs_info->fdb_sector_size * p_fat12_main_fs_info->bytes_per_sector);
    if(p_fat12_main_fs_info->root_fdb_data == NULL){
        fs_create_success = FALSE;
        KERNEL_MSG(FALSE, TRUE, "fail to allocate memory for fdb info!\n");
    }
    else if(fat12_rw_root_fdb(p_fat12_main_fs_info, FAT12_READ) == -1){
        fs_create_success = FALSE;
        KERNEL_MSG(FALSE, TRUE, "fail to read fdb info!\n");
    }

    /* if we cannot create file system, then release the allocated memory */
    if(fs_create_success == FALSE){
        if(p_fat12_main_fs_info->fat_data != NULL)
            vfree(p_fat12_main_fs_info->fat_data);
        if(p_fat12_main_fs_info->root_fdb_data != NULL)
            vfree(p_fat12_main_fs_info->root_fdb_data);

        vfree(p_fat12_main_fs_info);
        p_fat12_main_fs_info = NULL;

        return -1;
    }

    KERNEL_MSG(FALSE, TRUE, "create fat12 file system success ...\n");


/************************** test code **************************/
    FAT12_FS_FILE file, file2;
    INT8S result;
    INT8S path[] = "/boot/grub/../../boot/grub/../.././../boot/";

    console_printf("searching %s \n", path);
    result = fat12_search_fdb(p_fat12_main_fs_info, path, 0, &file);
    console_printf("\nresult = %d\n", result);
    //kprintf("name = %s\n", file.fdb.name);
    //kprintf("dir cluster = %d\n", file.dir_cluster_idx);
    //kprintf("fdb idx = %d\n", file.fdb_idx);
    //kprintf("fdb start cluster = %d\n", file.fdb.start_cluster);

/************************** test code **************************/
    FAT12_FS_LIST *p_list;

    p_list = fat12_get_fs_list(p_fat12_main_fs_info, "", file.fdb.start_cluster);
    console_printf("searching %s\n", path);
    while(p_list != NULL){
        console_printf("%s ", p_list->p_name);
        p_list = p_list->p_next;
    }
    console_printf("\n\n");

    p_list = fat12_get_fs_list(p_fat12_main_fs_info, "/", 0);
    console_printf("searching /\n");
    while(p_list != NULL){
        console_printf("%s ", p_list->p_name);
        p_list = p_list->p_next;
    }
    console_printf("\n\n");

/*
    p_list = fat12_get_fs_list(p_fs_info, "/", file.fdb.start_cluster);
    kprintf("searching /\n");
    while(p_list != NULL){
        kprintf("%s ", p_list->p_name);
        p_list = p_list->p_next;
    }
    kprintf("\n");

    p_list = fat12_get_fs_list(p_fs_info, "/test/", file.fdb.start_cluster);
    kprintf("searching /test/\n");
    while(p_list != NULL){
        kprintf("%s ", p_list->p_name);
        p_list = p_list->p_next;
    }
    kprintf("\n");
*/
    return 0;
}
