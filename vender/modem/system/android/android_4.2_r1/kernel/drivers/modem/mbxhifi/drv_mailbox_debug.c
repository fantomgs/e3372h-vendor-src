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


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "drv_mailbox.h"
#include "drv_mailbox_cfg.h"
#include "drv_mailbox_debug.h"
#include "drv_mailbox_gut.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef CONFIG_HIFI

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
#undef  _MAILBOX_FILE_
#define _MAILBOX_FILE_   "dbg"
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*lint --e{713, 732, 737, 569}*/
/*****************************************************************************
  3 函数实现
*****************************************************************************/
#if (MAILBOX_LOG_LEVEL != MAILBOX_LOG_NONE)
/*****************************************************************************
 函 数 名  : mailbox_log_erro
 接口类型  : 对外接口
 功能描述  : 跨核邮箱的初始化总入口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功或者失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月24日
    作    者   : 莫南 00176101
    修改内容   : 新生成函数

 详细描述:
*****************************************************************************/
MAILBOX_EXTERN long mailbox_log_erro(
                unsigned int   err_no,
                unsigned int   param1,
                unsigned int   param2,
                unsigned long   line_no,
                char*           file_name)
{
    struct mb *mb      = &g_mailbox_handle;
    struct mb_log  *record = &mb->log_array[0];
    unsigned long log_out = MAILBOX_FALSE;
    char * erro_str = MAILBOX_NULL;

    /*1.记录最近错误轨迹*/
    record[mb->log_prob].erro_num = err_no;
    record[mb->log_prob].line    = line_no;
    record[mb->log_prob].param1  = param1;
    record[mb->log_prob].param2  = param2;
    record[mb->log_prob].file = (const char* )file_name;
    mb->log_prob = ((MAILBOX_ERRO_ARRAY_NUM - 1) == mb->log_prob) ? (0) :
                                                            (mb->log_prob + 1);

    /*2.根据告警等级打印。*/
#if (MAILBOX_LOG_LEVEL >= MAILBOX_LOG_CRITICAL)
    if ((err_no > MAILBOX_CRIT_RET_START) && (err_no < MAILBOX_CRIT_RET_END)) {
        erro_str = "mb Critical!";
        log_out = MAILBOX_TRUE;
        mailbox_out(("file_name %s line_no(0x%x)!"RT, file_name, (unsigned int)line_no));
        mailbox_assert(err_no);
    }
#endif

#if (MAILBOX_LOG_LEVEL >= MAILBOX_LOG_CRITICAL)
    if((err_no > MAILBOX_ERR_RET_START) && (err_no < MAILBOX_ERR_RET_END)) {
        erro_str = "mb error!";
        log_out = MAILBOX_TRUE;
    }
#endif

#if (MAILBOX_LOG_LEVEL >= MAILBOX_LOG_WARNING)
    if((err_no > MAILBOX_WARNING_RET_START) && (err_no < MAILBOX_WARNING_RET_END)) {
        erro_str = "mb warning!";
        log_out = MAILBOX_TRUE;
    }
#endif

#if (MAILBOX_LOG_LEVEL >= MAILBOX_LOG_INFO)
    if((err_no > MAILBOX_INFO_RET_START) && (err_no < MAILBOX_INFO_RET_END)) {
        erro_str = "mb info!";
        log_out = MAILBOX_TRUE;
    }
#endif

    if (MAILBOX_FULL == err_no) {
        mailbox_out(("mb(0x%x) full !"RT, param1));
        log_out = MAILBOX_TRUE;
    } else if (MAILBOX_NOT_READY == err_no) {
        mailbox_out(("remote mb(0x%x) not ready!"RT, param1));
        log_out = MAILBOX_TRUE;
    }

    if (MAILBOX_TRUE == log_out) {
        mailbox_out(("%s:0x%08x, param1:0x%08x, param2:0x%08x, (line:%d),(file:%s)"RT,
            erro_str, err_no,  param1,  param2, (int)line_no, file_name));
    }

    return err_no;
}
#endif

#ifdef MAILBOX_OPEN_MNTN
/*对总时间和最大单次时间进行统计*/
void mailbox_statistic_slice(
                struct mb_slice* slice,
                unsigned long mailcode,
                unsigned long threslhold,
                unsigned long erro_code
                )
{
    signed long slice_diff;
    unsigned long slice_end = mailbox_get_timestamp();
    unsigned long slice_start = slice->start;

    slice_diff = mailbox_get_slice_diff(slice_start, slice_end);
    if (slice_diff < 0) {
        return;
    }

    /*记录回调历史总耗时*/
    slice_end = slice->total;
    slice->total += slice_diff;
    if (slice_end > slice->total) {
        slice->overflow = MAILBOX_TRUE; /*记录计时溢出，数据无效*/
    }

    /*记录最大回调耗时*/
    if (slice_diff > slice->max) {
        slice->max = slice_diff;
        slice->code = mailcode;
    }

    if (slice_diff >= threslhold) {
        #ifndef _DRV_LLT_
        mailbox_logerro_p2(erro_code, slice_diff, mailcode);
        #endif
    }
}

/*记录中断响应，调度处理之前的时间点*/
void mailbox_record_sche_send(void *priv)
{
    struct mb_buff *mbuf = (struct mb_buff *)priv;
    mbuf->mntn.sche.start = mailbox_get_timestamp();
}

/*记录中断响应后，调度处理的时间点，并进行统计*/
void mailbox_record_sche_recv(void *priv)
{
    struct mb_buff *mbuf = (struct mb_buff *)priv;

    mailbox_statistic_slice(&mbuf->mntn.sche, mbuf->channel_id,
                    MAILBOX_MAIL_SCHE_TIME_LIMIT,
                    MAILBOX_WARNING_SCHE_TIME_OUT);
}

/*可维可测: 记录剩余空间最小值*/
void mailbox_record_send(
                struct mb_mntn* mntn,
                unsigned long mailcode,
                unsigned long time_stamp,
                unsigned long mail_addr)
{
    struct mb_queue *m_queue = &mntn->mbuff->mail_queue;
    unsigned long size_left = mailbox_queue_left(m_queue->rear, m_queue->front, m_queue->length);

    /*可维可测: 记录剩余空间最小值*/
    if (size_left < mntn->peak_traffic_left){
        mntn->peak_traffic_left = size_left;
    }
    /*更新可维可测信息. 写通道只有发送时间和Use ID*/
    mntn->track_array[mntn->track_prob].send_slice = time_stamp;
    mntn->track_array[mntn->track_prob].mail_addr  = mail_addr;
    mntn->track_array[mntn->track_prob].use_id     = mailbox_get_use_id(mailcode);
    mntn->track_prob = ((MAILBOX_RECORD_USEID_NUM - 1) == mntn->track_prob) ?
                                                   (0) : (mntn->track_prob + 1);
}

/*记录发送核IPC中断触发到接受核中断响应之间的时间*/
void mailbox_record_transport(
                struct mb_mntn* mntn,
                unsigned long mailcode,
                unsigned long write_slice,
                unsigned long read_slice,
                unsigned long mail_addr)
{
    struct mb_queue *m_queue = &mntn->mbuff->mail_queue;
    unsigned long  size_left = mailbox_queue_left(m_queue->rear, m_queue->front, m_queue->length);

    /*可维可测: 记录剩余空间最小值*/
    if (size_left < mntn->peak_traffic_left){
        mntn->peak_traffic_left = size_left;
    }

    if (size_left < (m_queue->length >> 3) ){
        mailbox_logerro_p2(MAILBOX_ERR_GUT_MAILBOX_RECEIVE_FULL, size_left, mailcode);
    }

    /*记录收到的邮件use id*/
    mntn->track_array[mntn->track_prob].use_id        =
                                   (unsigned short)mailbox_get_use_id(mailcode);
    mntn->track_array[mntn->track_prob].send_slice    =  write_slice;
    mntn->track_array[mntn->track_prob].recv_slice =  read_slice;
    mntn->track_array[mntn->track_prob].mail_addr = mail_addr;
    mntn->track_prob = ((MAILBOX_RECORD_USEID_NUM - 1)==mntn->track_prob) ?
                                                   (0) : (mntn->track_prob + 1);

    mntn->trans.start = write_slice;
    mailbox_statistic_slice(&mntn->trans, mailcode,
                MAILBOX_MAIL_TRANS_TIME_LIMIT,
                MAILBOX_WARNING_TRANSPORT_TIME_OUT);

}

/*记录响应核邮箱用户回调函数执行时间*/
void mailbox_record_receive(
                struct mb_mntn* mntn,
                unsigned long mailcode,
                unsigned long slice_start)
{
    mntn->deal.start = slice_start;
    mailbox_statistic_slice(&mntn->deal, mailcode,
                    MAILBOX_MAIL_DEAL_TIME_LIMIT,
                    MAILBOX_WARNING_RECEIVE_TIME_OUT);
}

/*清除某个邮箱通道的可维可测信息*/
void mailbox_clear_mntn( struct mb_mntn *mntn, long clear)
{
    struct mb_buff   *mbuff;
    if (clear) {
        mbuff = mntn->mbuff;
        mailbox_memset(mntn, 0x00, sizeof(struct mb_mntn));
        mntn->mbuff = mbuff;
        mntn->peak_traffic_left = MAILBOX_QUEUE_LEFT_INVALID;
    }
}

/*****************************************************************************/
/*以下是可维可测函数*/
/*****************************************************************************
 函 数 名  : mailbox_show_general
 功能描述  : 显示邮箱模块状态，可维可测接口
 输入参数  : struct mb_cfg *pChannelCfg  -- 邮件通道的配置信息
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月28日
    作    者   : 莫南 00176101
    修改内容   : 新生成函数

*****************************************************************************/
MAILBOX_LOCAL void mailbox_show_general(struct mb_cfg *cfg)
{
    struct mb_head   *pBoxHead   =  (struct mb_head*)cfg->head_addr;

    /*总体信息*/
    mailbox_out(("Max Id,      HeadAddr,    DataAddr,     DataSize,   IntSrcId"RT));
    mailbox_out(("0x%08x,  0x%08x,  0x%08x,   0x%08x, %04d"RT,(unsigned int)cfg->butt_id,
                    (unsigned int)cfg->head_addr, (unsigned int)cfg->data_addr,
                    (unsigned int)cfg->data_size, (unsigned int)cfg->int_src));
    mailbox_out(("Head information:"RT));

    /*打印此邮箱的邮箱头内容。*/
    mailbox_out(("Head Front: 0x%x (0x%08x)"RT,  (unsigned int)pBoxHead->ulFront,
                (unsigned int)(cfg->data_addr + (pBoxHead->ulFront * sizeof(unsigned long)))));
    mailbox_out(("Head Rear: 0x%x (0x%08x)"RT, (unsigned int)pBoxHead->ulRear,
                (unsigned int)(cfg->data_addr + (pBoxHead->ulRear * sizeof(unsigned long)))));
    mailbox_out(("Head Frontslice: 0x%x"RT, (unsigned int)pBoxHead->ulFrontslice));
    mailbox_out(("Head Rearslice: 0x%x"RT,  (unsigned int)pBoxHead->ulRearslice));
    mailbox_out((":-------------------------------------------------------------:"RT));

}

/*****************************************************************************
 函 数 名  : mailbox_show_receive
 功能描述  : 显示邮箱模块某个接收通道的状态，可维可测接口
 输入参数  : struct mb_link  *pBoxHandle  某个邮箱通道句柄
 输出参数  : 无
 返 回 值  : unsigned long
             MAILBOX_OK, 异常返回值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月28日
    作    者   : 莫南 00176101
    修改内容   : 新生成函数

*****************************************************************************/
MAILBOX_LOCAL void mailbox_show_receive(struct mb_buff *mbuf)
{
    struct mb_mntn      *mntn         =  &(mbuf->mntn);    /*此邮箱通道的可维可测数据*/
    struct mb_cb        *callback     =  mbuf->read_cb;    /*此邮箱通道的功能回调函数队列*/
    unsigned long        max_use      =  mailbox_get_use_id(mbuf->config->butt_id);
    unsigned long        i            =  0;

    mailbox_out((":---------------------------------------------:"RT));
    mailbox_out(("Receive info:"RT));

    /*打印挂接在此邮箱通道上的应用号及应用回调函数地址*/
    mailbox_out(("Mail Read Call Back show:"RT));
    mailbox_out(("Use Id,  Call Back,     User Handle"RT));
    while (i < max_use) {
        mailbox_out(("%d,       0x%08x,    0x%08x"RT, (int)i, (unsigned int)callback->func,
                                                (unsigned int)callback->handle));
        callback++;
        i++;
    }

    /*平均调度等待及最大调度等待时间*/
    if (MAILBOX_TRUE != mntn->sche.overflow) {
        mailbox_out(("Schedule Avg. slice:%4d, total:%d"RT,
      (int)((mbuf->seq_num + 1) ? (mntn->sche.total)/(mbuf->seq_num + 1) : 0), (int)mbuf->seq_num));
    } else {
        mailbox_out(("Schedule Avg. data overflow "RT));
    }
    mailbox_out(("Schedule Max. slice:%4d,  Use ID:0x%08x"RT, (int)(mntn->sche.max),
                                            (int)mntn->sche.code));

    /*平均传送及最大传送时间*/
    if (MAILBOX_TRUE != mntn->trans.overflow) {
        mailbox_out(("Transfers Avg. slice:%4d, total:%d"RT,(int)((mbuf->seq_num + 1) ?
                ((mntn->trans.total)/(mbuf->seq_num + 1)) : 0 ),  (int)mbuf->seq_num));
    } else {
        mailbox_out(("Transfers Max. data overflow"RT));
    }
    mailbox_out(("Transfers Max. slice:%4d,  Use ID:0x%08x"RT, (int)(mntn->trans.max),
                (int)(mntn->trans.code)));/*lint -e539*/

    /*平均处理及最大处理时间*/
    if (MAILBOX_TRUE != mntn->deal.overflow) {
        mailbox_out(("Call Back Avg. slice:%4d, total:%d"RT,
            (int)((mbuf->seq_num + 1) ? (mntn->deal.total)/(mbuf->seq_num + 1) : 0), (int)mbuf->seq_num ) );
    } else {
        mailbox_out(("Call Back Avg. data overflow"RT));
    }
    mailbox_out(("Call Back Max. slice:%4d,  Use ID:0x%08x"RT,
            (int)(mntn->deal.max), (int)mntn->deal.code));


    mailbox_out((":---------------------------------------------:"RT));
}

/*****************************************************************************
 函 数 名  : mailbox_show_detail
 功能描述  : 显示邮箱模块某个通道的详细传输状态，可维可测接口。
 输入参数  : struct mb_link *pBoxHandle  -- 某个邮箱通道句柄
 输出参数  : 无
 返 回 值  : unsigned long
             MAILBOX_OK, 异常返回值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月28日
    作    者   : 莫南 00176101
    修改内容   : 新生成函数

*****************************************************************************/
MAILBOX_LOCAL void mailbox_show_detail(struct mb *mb,
                struct mb_buff *mbuf,
                int clear)
{
    struct mb_mntn        *mntn         =  MAILBOX_NULL;  /*此邮箱通道的可维可测数据*/
    unsigned long          channel  =  mbuf->channel_id;
    struct mb_queue       *queue   =  &mbuf->mail_queue;
    struct mb_mail        *mail;
    unsigned long i;

    mailbox_out(("mail box show channel(0x%08x) information:"RT, (unsigned int)channel));

    /*总体信息*/
    mailbox_show_general(mbuf->config);
    mailbox_out((":---------------------------------------------:"RT));
    mntn = &(mbuf->mntn);
    /*显示此邮箱最近传输的几个use ID信息, 最近的显示在前面*/
    i = mntn->track_prob ;
    mailbox_out(("Latest transmit mails track:(%d total)"RT, (int)mbuf->seq_num));
    mailbox_out(("id   ,address     ,send slice   ,recv slice  ,diff slice"RT));
    do {
        i = ((0 == i) ? (MAILBOX_RECORD_USEID_NUM - 1) : (i - 1));
        mail = (struct mb_mail *)mntn->track_array[i].mail_addr;

        if (mail && (0 == mntn->track_array[i].recv_slice)) {
            mntn->track_array[i].recv_slice = mail->ulReadSlice;
        }
        mailbox_out(("%02d   ,0x%-8x  ,0x%-8x   ,0x%-8x  ,0x%-8x(%d)"RT,
            mntn->track_array[i].use_id,
        (unsigned int)mail, (unsigned int)mntn->track_array[i].send_slice,
                                  (unsigned int)mntn->track_array[i].recv_slice,
        (unsigned int)mailbox_get_slice_diff(mntn->track_array[i].send_slice,
                                               mntn->track_array[i].recv_slice),
        (unsigned int)mailbox_get_slice_diff(mntn->track_array[i].send_slice,
                                            mntn->track_array[i].recv_slice)));
    } while (i != (mntn->track_prob));

    if (mb->local_id == mailbox_get_dst_id(channel)) {
        /*如果是接收通道*/
        /*打印通道方向*/
        mailbox_out(("Receive Channel"RT));
    }

    /*打印通道方向*/
    if (mb->local_id == mailbox_get_src_id(channel)) {
        mailbox_out(("Send Channel: sem id(0x%08x)"RT,(unsigned int)mbuf->mutex));
    } else if (mb->local_id == mailbox_get_dst_id(channel)) {
        mailbox_out(("Receive Channel: sem id(0x%08x)"RT,(unsigned int)mbuf->mutex));
        mailbox_show_receive(mbuf);
    }

    if (MAILBOX_QUEUE_LEFT_INVALID == mntn->peak_traffic_left) {
        mntn->peak_traffic_left = queue->length;
    }
    mailbox_out(("Peak Traffic: %d%%, Peak: 0x%x, Total: 0x%x"RT,
        (int)(100*(queue->length - mntn->peak_traffic_left)/queue->length),
        (unsigned int)(queue->length - mntn->peak_traffic_left), (unsigned int)queue->length));

    mailbox_out((":------------------------------------------------:"RT));

    mailbox_clear_mntn(mntn, clear);
}

/*****************************************************************************
 函 数 名  : MAILBOX_ShowByHadnle
 功能描述  : 显示邮箱模块状态，可维可测接口
 输入参数  : unsigned long  MailChannel  -- 邮件通道的id号。
             unsigned long               -- 是否显示所有通道信息。
                             input                  效果
                    MailChannel |  IfShowAll    |  effective
            -----------------------------------------------------
                        not 0   |    1          |  Show all channel's detail information.
                    ChannelID   |    not 1      |  Show this channel's detail infotmation.
                    0           |    ANY        |  Show all channel's general information.
        not ChannelID or not 0  |    not 1      |  Invalid, no response.

 输出参数  : 无
 返 回 值  : unsigned long
             MAILBOX_OK, 异常返回值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月28日
    作    者   : 莫南 00176101
    修改内容   : 新生成函数

*****************************************************************************/
MAILBOX_EXTERN long mailbox_show(
                unsigned long    channel,
                unsigned long    show_flag)
{
    struct mb_cfg         *config     = &g_mailbox_global_cfg_tbl[0];
    struct mb_buff        *mbuf       = MAILBOX_NULL;
    struct mb_link        *send_tbl   = MAILBOX_NULL;    /*指向主结构体的发送通道数组基地址*/
    struct mb_link        *recv_tbl   = MAILBOX_NULL;    /*指向主结构体的接收通道数组基地址*/
    struct mb             *mb   = MAILBOX_NULL;
    struct mb_log         *record = MAILBOX_NULL;
    unsigned long          i;
    unsigned long          j;
    unsigned long          clear = MAILBOX_FALSE;

    mb =  mailbox_get_mb();
    if (MAILBOX_NULL == mb) {
        return (unsigned long)MAILBOX_ERRO;
    }

    if ( MAILBOX_SHOW_CLEAR & show_flag ) {
        clear = MAILBOX_TRUE;
    }

    if (MAILBOX_SHOW_ALL & show_flag) {
        /*Show all channel's general information.*/
        /*遍历显示所有的发送通道信息*/
        send_tbl = mb->send_tbl;
        for (i = 0; i < MAILBOX_CPUID_BUTT; i++) {
            if (MAILBOX_NULL != send_tbl[i].channel_buff) {
                mbuf  =   send_tbl[i].channel_buff;
                for (j = 0; j < send_tbl[i].carrier_butt; j++) {
                    mailbox_show_detail(mb, mbuf, clear);
                    mbuf++;
                }
            }
        }

        recv_tbl = mb->recv_tbl;
        /*遍历显示所有的接收通道信息*/
        for (i = 0; i < MAILBOX_CPUID_BUTT; i++) {
            if (MAILBOX_NULL != recv_tbl[i].channel_buff) {
                mbuf  =   recv_tbl[i].channel_buff;
                for (j = 0; j < recv_tbl[i].carrier_butt; j++) {
                    mailbox_show_detail(mb, mbuf, clear);
                    mbuf++;
                }
            }
        }
    } else {
        /*通道号指明，显示这个邮箱通道的详细信息*/
        mbuf = mailbox_get_channel_handle(mb, channel);

        if (MAILBOX_NULL != mbuf) {
            mailbox_show_detail(mb, mbuf, clear);
        } else {
            /*如果通道号没有指明，列出邮箱模块各通道的总体状态:*/
            mailbox_out(("mail box show global channel config:"RT));
            while (MAILBOX_MAILCODE_INVALID != config->butt_id) {
                mailbox_show_general(config);
                config++;
            }

            /*显示此邮箱最近几个错误信息, 最近的显示在前面*/
            i = mb->log_prob;
            record = &mb->log_array[0];
            mailbox_out(("Latest error log track:"RT));
            mailbox_out(("error num,   line num,   file name"RT));
            do {

               i = ((0 == i) ? (MAILBOX_ERRO_ARRAY_NUM - 1) : (i - 1));
               mailbox_out(("0x%-8x,  %-8d,  %-8s"RT,(unsigned int)record[i].erro_num,
                (int)record[i].line,  record[i].file));

            } while (i != (mb->log_prob));
             mailbox_out(("track end."RT));
        }
    }

    mailbox_out((":================================================:"RT));
    return MAILBOX_OK;
}
#else

MAILBOX_EXTERN long mailbox_show(
                unsigned long    channel,
                unsigned long    show_flag)
{
    return MAILBOX_OK;
}

#endif

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

