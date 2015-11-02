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
#ifndef __KEY_BALONG_H__
#define __KEY_BALONG_H__

#include <product_config.h>

#if (FEATURE_ON == MBB_DLOAD_SDUP)
#define KEY_UP   0  //按键抬起
#define KEY_DOWN 1  //按键按下

#define KEY_DETECT_TIME_2000MS   (2000)  /*SD卡升级组合键按下时间长度*/
#define KEY_DETECT_TIME_500MS    (500)   /*SD卡升级触发时双击键检测时间*/
#define DOUBLE_CLK_KEY  (KEY_F24)       /*SD卡升级触发时需要双击的键*/
#endif

typedef enum
{
    INVALID_KEY,
    POWER_KEY,
    MENU_KEY,
    RESET_KEY,
    WIFI_KEY,
    BUTT_KEY
}KEY_ENUM;

/******************************************************************************
*  Function:  key_int_disable
*  Description: 假开机或假关机时去使能WPS键和Reset键的gpio中断
*  Called by:   
*  Input: item  :
*  Output:None
*  Return:None
*  Note  :      
*  History:     1. Created by l00225826.
********************************************************************************/
void key_int_disable(KEY_ENUM key);

/******************************************************************************
*  Function:  key_int_enable
*  Description: 假开机或假关机时使能WPS键和Reset键的gpio中断
*  Called by:   
*  Input: item  :
*  Output:None
*  Return:None
*  Note  :      
*  History:     1. Created by l00225826.
********************************************************************************/
void key_int_enable(KEY_ENUM key);

#endif