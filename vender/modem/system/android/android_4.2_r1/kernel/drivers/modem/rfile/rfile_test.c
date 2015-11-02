 /*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and 
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may 
 * *    be used to endorse or promote products derived from this software 
 * *    without specific prior written permission.
 * 
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include "rfile_balong.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef __KERNEL__

#define rfile_print_info        printk

#else /* __VXWORKS__ */

#define rfile_print_info        printf

#endif /* end of __KERNEL__ */


/*************************************************************************
 函 数 名   : rfile_test_001
 功能描述   : 新建目录-->新建文件-->写入内容-->读取内容-->关闭文件-->获取文件状态
*************************************************************************/
s32 rfile_test_001(void)
{
    s32 ret;
    s8 *dirpar = "/rfiletest001";
    s8 *filepath = "/rfiletest001/rfile001.txt";
    s8 *wrbuf = "1234567890abcdefghijklmnopqrstuvwxyz";
    s8 *rfilewrtmp = NULL;
    s8 *rfilerdtmp = NULL;
    s8 rdbuf[64] = {0};
    s32 pfile;
    struct rfile_stat_stru buf = {0};

    rfile_print_info("[%s] start =======================================.\n\n", __FUNCTION__);
    rfile_print_info("test for : mkdir-->fopen-->fseek-->fwrite-->fwrite-->");
    rfile_print_info("ftell-->fseek-->ftell-->fread-->fread-->fclose-->stat.\n\n");

    rfilewrtmp = Rfile_Malloc(8888);
    memset(rfilewrtmp, 'x', 8888);

    rfilerdtmp = Rfile_Malloc(8888);
    memset(rfilerdtmp, 0, 8888);

    ret = bsp_mkdir(dirpar, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_001_fail_1;
    }

    pfile = bsp_open(filepath, (RFILE_CREAT|RFILE_RDWR), 0755);
    if(pfile < 0)
    {
        rfile_print_info("!!!!!!!! bsp_open failed.\n");

        goto rfile_test_001_fail_1;
    }

    ret = bsp_lseek(pfile, 0, SEEK_SET);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 1 failed .\n");

        goto rfile_test_001_fail_2;
    }

    ret = bsp_write(pfile, wrbuf, strlen(wrbuf));
    if(ret <= 0)
    {
        rfile_print_info("!!!!!!!! BSP_fwrite 1 failed .\n");

        goto rfile_test_001_fail_2;
    }

    ret = bsp_write(pfile, rfilewrtmp, 8888);
    if(ret <= 0)
    {
        rfile_print_info("!!!!!!!! BSP_fwrite 2 failed .\n");

        goto rfile_test_001_fail_2;
    }

    rfile_print_info("BSP_fwrite %d bytes.\n", ret);

    for(ret = 0; ret < 28; ret++)
    {
        rfile_print_info("-0x%x-", rfilewrtmp[ret]);
    }

    rfile_print_info(".\n");

    for(ret = 8880; ret < 8888; ret++)
    {
        rfile_print_info("-0x%x-", rfilewrtmp[ret]);
    }

    rfile_print_info(".\n");

    ret = bsp_tell(pfile);
    if(ret < 0)
    {
        rfile_print_info("!!!!!!!! BSP_ftell 1 failed 0x%x.\n", ret);
    }
    else
    {
        rfile_print_info("!!!!!!!! BSP_ftell 1 success 0x%x .\n", ret);
    }

    ret = bsp_lseek(pfile, 0, SEEK_SET);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_fseek 2 failed .\n");
    }

    ret = bsp_tell(pfile);
    if(ret < 0)
    {
        rfile_print_info("!!!!!!!! BSP_ftell 2 failed 0x%x.\n", ret);
    }
    else
    {
        rfile_print_info("!!!!!!!! BSP_ftell 2 success 0x%x .\n", ret);
    }

    ret = bsp_read(pfile, rdbuf, 64);
    if(ret <= 0)
    {
        rfile_print_info("!!!!!!!! BSP_fread 1 failed .\n");

        goto rfile_test_001_fail_2;
    }

    ret = bsp_read(pfile, rfilerdtmp, 8888);
    if(ret <= 0)
    {
        rfile_print_info("!!!!!!!! BSP_fread 2 failed .\n");

        goto rfile_test_001_fail_2;
    }

    rfile_print_info("BSP_fread %d bytes.\n", ret);

    for(ret = 0; ret < 28; ret++)
    {
        rfile_print_info(" 0x%x ", rfilerdtmp[ret]);
    }

    rfile_print_info(".\n");

    for(ret = 8880; ret < 8888; ret++)
    {
        rfile_print_info(" 0x%x ", rfilerdtmp[ret]);
    }

    rfile_print_info(".\n");

    ret = bsp_close(pfile);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_close failed.\n");

        goto rfile_test_001_fail_1;
    }

    ret = bsp_stat(filepath, &buf);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_stat failed.\n");

        goto rfile_test_001_fail_1;
    }
    else
    {
        rfile_print_info("BSP_stat : buf.mtime.tv_sec %d, buf.mtime.tv_nsec %d, buf.blksize %d\n",
            (int)buf.mtime.tv_sec, (int)buf.mtime.tv_nsec, (int)buf.blksize);
    }

    rfile_print_info("[%s] success.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return 0;

rfile_test_001_fail_2:
    bsp_close(pfile);

rfile_test_001_fail_1:

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return -1;
}


/*************************************************************************
 函 数 名   : rfile_test_001_01
 功能描述   : 删除文件-->删除目录
*************************************************************************/
s32 rfile_test_001_01(void)
{
    s32 ret;
    s8 *dirpar = "/rfiletest001";
    s8 *filepath = "/rfiletest001/rfile001.txt";

    rfile_print_info("[%s] start =======================================.\n\n", __FUNCTION__);
    rfile_print_info("test for : remove-->rmdir.\n\n");

    ret = bsp_remove(filepath);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_remove failed.\n");

        goto rfile_test_001_01_fail_1;
    }

    ret = bsp_rmdir(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! bsp_rmdir failed.\n");

        goto rfile_test_001_01_fail_1;
    }

    rfile_print_info("[%s] success.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return 0;

rfile_test_001_01_fail_1:

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return -1;
}



/*************************************************************************
 函 数 名   : rfile_test_002
 功能描述   : 新建目录-->新建目录-->打开目录-->读目录-->关闭目录
*************************************************************************/
s32 rfile_test_002(void)
{
    s32 ret;
    s8 *dirpar = "/rfiletest002";
    s8 *dirsub1 = "/rfiletest002/subdir1";
    s8 *dirsub2 = "/rfiletest002/subdir2";
    s32 dir;
    s8 data[1024] = {0};
    s32 i;
    RFILE_DIRENT_STRU *pstDirent;

    rfile_print_info("[%s] start =======================================.\n\n", __FUNCTION__);
    rfile_print_info("test for : mkdir-->mksubdir1-->mksubdir2-->opendir-->readdir-->closedir.\n\n");

    ret = bsp_mkdir(dirpar, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_002_fail_1;
    }

    ret = bsp_mkdir(dirsub1, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_002_fail_1;
    }

    ret = bsp_mkdir(dirsub2, 0660);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_mkdir failed.\n");

        goto rfile_test_002_fail_1;
    }

    dir = bsp_opendir(dirpar);
    if(dir < 0)
    {
        rfile_print_info("!!!!!!!! BSP_opendir failed.\n");

        goto rfile_test_002_fail_1;
    }

    ret = bsp_readdir(dir, data, 1024);
    if(ret <= 0)
    {
        rfile_print_info("!!!!!!!! BSP_readdir failed.\n");
        goto rfile_test_002_fail_2;
    }
    else
    {
        for(i=0; i<ret; )
        {
            pstDirent = (RFILE_DIRENT_STRU*)(data + i);
            i += pstDirent->d_reclen;

            rfile_print_info("pst->d_name=%s!\n",pstDirent->d_name);
        }

    }

    ret = bsp_closedir(dir);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_closedir failed.\n");

        goto rfile_test_002_fail_1;
    }

    rfile_print_info("[%s] success.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return 0;

rfile_test_002_fail_2:
    ret = bsp_closedir(dir);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_closedir failed.\n");
    }

rfile_test_002_fail_1:

    rfile_print_info("[%s] failure.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return -1;
}


/*************************************************************************
 函 数 名   : rfile_test_002_01
 功能描述   : 删除目录-->删除目录
*************************************************************************/
s32 rfile_test_002_01(void)
{
    s32 ret;
    s8 *dirpar = "/rfiletest002";
    s8 *dirsub1 = "/rfiletest002/subdir1";
    s8 *dirsub2 = "/rfiletest002/subdir2";

    rfile_print_info("[%s] start =======================================.\n\n", __FUNCTION__);
    rfile_print_info("test for : rmsubdir1-->rmsubdir2-->rmdir.\n\n");

    ret = bsp_rmdir(dirsub1);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_rmdir failed.\n");

        goto rfile_test_002_01_fail_1;
    }

    ret = bsp_rmdir(dirsub2);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_rmdir failed.\n");

        goto rfile_test_002_01_fail_1;
    }

    ret = bsp_rmdir(dirpar);
    if(BSP_OK != ret)
    {
        rfile_print_info("!!!!!!!! BSP_rmdir failed.\n");

        goto rfile_test_002_01_fail_1;
    }

    rfile_print_info("[%s] success.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return 0;

rfile_test_002_01_fail_1:

    rfile_print_info("[%s] failure.\n", __FUNCTION__);

    rfile_print_info("[%s] end =======================================.\n", __FUNCTION__);

    return -1;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


