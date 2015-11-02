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


#ifdef __cplusplus
extern "C"
{
#endif
/*lint --e{527,529,533,537,752}*/
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/types.h>
#include "bsp_leds.h"
#include "bsp_regulator.h"
#include "bsp_pmu.h"
#include "bsp_nvim.h"
#include "bsp_icc.h"
#include "bsp_softtimer.h"
#include "leds_balong.h"

#if (FEATURE_ON == MBB_LED)
#include <linux/gpio.h>
static DEFINE_MUTEX(g_led_kernelset_lock);    /*led�ں˿��ƽӿ�������*/
#define long_time    5000000    /*dr�����Ĵ���ֵ*/
#define current    3000    /*��λuA��������Ϊ3mA*/

extern int g_dr_brightness[5];     /*��ǰ�����ȱ�ʶ*/
struct regulator *dr_regulator[5];    /*����regulatorȫ�ֱ���*/
static int g_dr_id = 0;    /*ȫ�ֱ���dr����id*/

static int g_set_flag = 0;    /*�ӿڵ��ñ�ʶλ*/
static int g_ledready_flag = 0;    /*�ڵ������ʶλ*/


#define DR_NUM 5    /*dr����5��*/
#define GPIO_NUM 6 /*�洢GPIO�Ƶ����鶨Ϊ6*/

struct led_kernelset_date g_gpioled_date_in[GPIO_NUM] = 
{
    [0] = {
        .flag = 0,
    },
    [1] = {
        .flag = 0,
    },
    [2] = {        /*�Ʊ�������־*/
        .flag = 0,
    },
    [3] = {        /*�Ʊ�������־*/
        .flag = 0,
    },
    [4] = {        /*�Ʊ�������־*/
        .flag = 0,
    },
    [5] = {        /*�Ʊ�������־*/
        .flag = 0,
    }
};
struct led_kernelset_date g_drled_date_in[DR_NUM] = 
{
    [0] = {
        .flag = 0,
    },
    [1] = {
        .flag = 0,
    },
    [2] = {        /*�Ʊ�������־*/
        .flag = 0,
    },
    [3] = {        /*�Ʊ�������־*/
        .flag = 0,
    },
    [4] = {        /*�Ʊ�������־*/
        .flag = 0,
    }
};
#endif

/*lint -e553*/ 
#if (FEATURE_ON == MBB_COMMON)
#define en_NV_Item_HUAWEI_PCCW_HS_HSPA_BLUE 50032
#endif
/*lint +e553*/ 
struct softtimer_list led_softtimer;                /* soft timer */
struct nv_led g_nv_led;                             /* store nv */
LED_CONTROL_NV_STRU g_led_state_str_lte[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_OM_CONFIG; /* ��ͬ��״̬��Ӧ�����Ʒ�����������led nvһ�� */

/*lint -e553*/ 
#if (FEATURE_ON == MBB_WPG_COMMON)
LED_CONTROL_NV_STRU g_led_HongKong_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_HONGKONG_CONFIG;
LED_CONTROL_NV_STRU g_led_Russia_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_RUSSIA_CONFIG;
LED_CONTROL_NV_STRU g_led_Vodafone_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_VODAFONE_CONFIG;
LED_CONTROL_NV_STRU g_led_ChinaUnion_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_CHINAUNION_CONFIG;
LED_CONTROL_NV_STRU g_led_FranceSFR_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_FRANCE_CONFIG;
LED_CONTROL_NV_STRU g_led_IndiaBSNL_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_INDIA_CONFIG;
LED_CONTROL_NV_STRU g_led_HolandKPN_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_HOLAND_CONFIG;
LED_CONTROL_NV_STRU g_led_SoftBank_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_OM_CONFIG;
LED_CONTROL_NV_STRU g_led_Emobile_StateStr[LED_LIGHT_STATE_MAX][LED_CONFIG_MAX_LTE] = LED_OM_CONFIG;
#endif
/*lint +e553*/ 
unsigned int led_debug_level;                       /* for LED_TRACE level */

extern struct led_tled_arg g_arg;    

static int __devinit balong_led_probe(struct platform_device *dev);
static int __devexit balong_led_remove(struct platform_device *pdev);
int register_led_by_nr(struct balong_led_device *led, int nr, struct led_platform_data *pdata);

/* LED resources */
struct led_info balong_led[] = {
    [0] = {
        .name = LED1_NAME,
    },
    [1] = {
        .name = LED2_NAME,
    },
    [2] = {
        .name = LED3_NAME,
    },
    [3] = {
        .name = LED4_NAME,
    },
    [4] = {
        .name = LED5_NAME,
    }
};

/* LED platform data */
 struct led_platform_data led_plat_data = {
    .num_leds   = ARRAY_SIZE(balong_led),
    .leds       = (struct led_info *)balong_led,
};

/* LED platform device */
 struct platform_device balong_led_dev = {
    .name	= "balong_led",
	.id	    = -1,
	.dev 	= {
		.platform_data	= &led_plat_data,
	}
};

unsigned long us2ms(unsigned long us)
{
    return us / 1000;
}

/*lint -save -e413 -e613*/
/************************************************************************
 * Name         : limit_to_brightness
 * Function     : limit to brightness
 * Arguments
 *      input   : @led_id  - led id
 *              : @limit   - limit value
 *      output  : null
 *
 * Return       : brightness
 * Decription   : null
 ************************************************************************/
 int limit_to_brightness(int led_id, int limit)
{
    int brightness, limit_max;

    limit_max = bsp_dr_list_current(led_id, DR_CUR_NUMS - 1);
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] led id %d, limit %d, limit max %d\n", __FUNCTION__, led_id, limit, limit_max);

    brightness = limit * LED_FULL / limit_max;  //DEBUG
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] led id %d, brightness %d\n", __FUNCTION__, led_id, brightness);

    return brightness;
}

/************************************************************************
 * Name         : brightness_to_limit
 * Function     : brightness to limit
 * Arguments
 *      input   : @led_id         - led id
 *              : @led_brightness - brightness
 *      output  : null
 *
 * Return       : limit
 * Decription   : ������������С��������Ϊ(LED_FULL + 1)��������ݴ˽�led_brightnessת��Ϊlimit,
 *                �ٸ���DR�ɽ��ܵ�8��������λ��ѡ����ӽ���һ����λ��
 ************************************************************************/
 int brightness_to_limit(int led_id, int led_brightness)
{
    unsigned i;
    int limit, limit_min, limit_max, limit_bigger, limit_smaller;

    /* compute limit */
    if(LED_OFF == led_brightness)
    {
        return 0;
    }
    else
    {
        limit_min = bsp_dr_list_current(led_id, 0);
        limit_max = bsp_dr_list_current(led_id, DR_CUR_NUMS - 1);

        limit = led_brightness * limit_max / LED_FULL; //DEBUG
        LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] led id %d, brightness %d, limit min %d, limit max %d, limit %d\n",
            __FUNCTION__, led_id, led_brightness, limit_min,limit_max, limit);
    }

    /* find nearest limit level to use */
    for(i = 0; i < DR_CUR_NUMS; i++)
    {
        limit_bigger = bsp_dr_list_current(led_id, i);
        if(limit < limit_bigger)
        {
            break;
        }
    }
    limit_smaller = (0 == i) ? 0 : bsp_dr_list_current(led_id, i - 1);
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] limit smaller %d, limit bigger %d\n", __FUNCTION__, limit_smaller, limit_bigger);

    /* find nearest limit level, if limit is just in the middle of limit_bigger and limit_smaller, chose limit_bigger */
    limit = ((limit - limit_smaller) < (limit_bigger - limit)) ? limit_smaller : limit_bigger;
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] limit level %d\n", __FUNCTION__, limit);

    return limit;
}


/************************************************************************
 * Name         : name_to_id
 * Function     : get id of led_cdev
 * Arguments
 *      input   : @led_cdev - which led
 *      output  : null
 *
 * Return       : led id
 * Decription   : null
 ************************************************************************/
 int name_to_id(struct led_classdev *led_cdev)
{
    /* reject null */
    if(!led_cdev || !led_cdev->name)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        return LED_ERROR;
    }

    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] led name %s\n", __FUNCTION__, led_cdev->name);

    if(!strcmp(led_cdev->name, LED1_NAME))
    {
        return LED1_ID;
    }
    else if(!strcmp(led_cdev->name, LED2_NAME))
    {
        return LED2_ID;
    }
    else if(!strcmp(led_cdev->name, LED3_NAME))
    {
        return LED3_ID;
    }
    else if(!strcmp(led_cdev->name, LED4_NAME))
    {
        return LED4_ID;
    }
    else
    {
        return LED5_ID;
    }

}

/************************************************************************
 * Name         : set_bre_time_valid
 * Function     : set breath time aviled
 * Arguments
 *      input   : @led_id   - led id
 *                @bre_time_enum - Ҫ���õĲ�������
 *                @p_breathtime_ms - Ҫ���õĲ���ֵ
 *      output  : @p_breathtime_ms - Ҫ���õĲ���ֵ
 *
 * Return       : default breath on time(ms)
 * Decription   : Ĭ��ֵ�˴��ο�Hi6551�û��ֲ��Ӧ�Ĵ�����Ĭ��ֵ
 ************************************************************************/
int set_bre_time_valid(int led_id, dr_bre_time_e bre_time_enum, unsigned long *p_breathtime_ms)
{
    unsigned long value_num, selector;
    unsigned long temp_bigger = 0, temp_smaller, breath_time;
    int ret = LED_ERROR;

    /* ������ */
    if(!p_breathtime_ms)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        goto ERRO;
    }

    value_num = bsp_dr_bre_time_selectors_get((dr_id_e)led_id, bre_time_enum);
    if(BSP_PMU_PARA_ERROR == value_num)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%d get %d value num failed\n", __FUNCTION__, led_id, bre_time_enum);
        goto ERRO;
    }

    /* ѡ����õĵ�λֵ */
    breath_time = *p_breathtime_ms;
    for(selector = 0; selector < value_num; selector++)
    {
        temp_bigger = bsp_dr_bre_time_list((dr_id_e)led_id, bre_time_enum, selector);
        if(BSP_PMU_PARA_ERROR == temp_bigger)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%d get breath time failed\n", __FUNCTION__, led_id);
            goto ERRO;
        }
        if(temp_bigger >= breath_time)
        {
            LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]%d bigger breath time %d\n", __FUNCTION__, led_id, temp_bigger);
            break;
        }        
    }

    if(value_num == selector)   /* �����ֵ�������λ����ʹ�����λ */
    {
        LED_TRACE(LED_DEBUG_LEVEL(WARNING),"[%s]%d breath time is larger than max, breath_time %d, max value %d\n",
            __FUNCTION__, led_id, breath_time, temp_bigger);
        *p_breathtime_ms = temp_bigger;
    }
    else if(0 == selector)      /* �����ֵС����С��λ����ʹ����С��λ */
    {
        LED_TRACE(LED_DEBUG_LEVEL(WARNING),"[%s]%d breath time is samller than min, breath_time %d, min value %d\n",
            __FUNCTION__, led_id, breath_time, temp_bigger);
        *p_breathtime_ms = temp_bigger;
    }
    else                        /* �����ֵ�������λ����С��λ֮�䣬ѡ������ĵ�λ�����ǡλ���������м䣬ʹ�ý�С��λ */
    {
        temp_smaller = bsp_dr_bre_time_list((dr_id_e)led_id, bre_time_enum, selector - 1);
        if(BSP_PMU_PARA_ERROR == temp_smaller)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%d get breath time failed\n", __FUNCTION__, led_id);
            goto ERRO;
        }
        *p_breathtime_ms = ((temp_bigger - breath_time) > (breath_time - temp_smaller) ? temp_smaller : temp_bigger);
        LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]%d breath time bigger %d, smaller %d\n", 
            __FUNCTION__, led_id, temp_bigger, temp_smaller);
        LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]%d breath time %d, choice %d\n", 
            __FUNCTION__, led_id, breath_time, *p_breathtime_ms);
    }

    return LED_OK;
ERRO:
    return ret;   
}

/************************************************************************
 * Name         : set_bre_mode
 * Function     : set breath mode
 * Arguments
 *      input   : @led_id   - led id
 *
 * Return       : 0 - success; else - failed
 * Decription   : DR1/2��DR3/4/5�ĺ���ģʽ���ò�һ��
 ************************************************************************/
int set_bre_mode(int led_id)
{
    int ret = LED_ERROR;
    
    switch(led_id)
    {        
        case LED1_ID:
        case LED2_ID:
            ret = bsp_dr_set_mode(led_id, PMU_DRS_MODE_BRE_FLASH);
            break;
        case LED3_ID:
        case LED4_ID:
        case LED5_ID:
            ret = bsp_dr_set_mode(led_id, PMU_DRS_MODE_BRE);
            break;
        default:
            break;
    };
    
    return ret;
}
/************************************************************************
 * Name         : balong_brightness_set
 * Function     : set brightness
 * Arguments
 *      input   : @led_cdev - which led
 *                @brightness - brightness, should between 0~255
 *      output  : null
 *
 * Return       : null
 * Decription   : ������������������ɿ���ʹ���������
 ************************************************************************/
void balong_led_brightness_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
    struct balong_led_device *led_dev = container_of(led_cdev, struct balong_led_device, cdev);
    int led_id, limit, ret;

    /* argument check */
    if((!led_cdev) || (!led_dev) || (!led_dev->pdata))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        return;
    }

    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] %s, brightness %d\n", __FUNCTION__, led_cdev->name, brightness);

    mutex_lock(&led_dev->pdata->mlock);

    /* brightness can't be too big or too small */
    if(brightness >= LED_FULL)
    {
        brightness = LED_FULL;
    }

    /* get id of led */
    led_id = name_to_id(led_cdev);

    /* save brightness */
    led_cdev->brightness = brightness;
    
    /* set limit */
    limit = brightness_to_limit(led_id, brightness);
    if(0 == limit)
    {
        /* if enabled, disable */
        if(regulator_is_enabled(led_dev->pdata->pregulator))
        {
            ret = regulator_disable(led_dev->pdata->pregulator);
            if(ret)
            {
                LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] %s regulator_disable failed, ret = %d\n", 
                    __FUNCTION__, led_cdev->name, ret);
                goto EXIT;
            }
        }
        goto EXIT;
    }
    regulator_set_current_limit(led_dev->pdata->pregulator, limit, limit);
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s] %s set brightness %d limit %d\n", 
        __FUNCTION__, led_cdev->name, brightness, limit);

    /* enable led */
    if(!regulator_is_enabled(led_dev->pdata->pregulator))
    {
        ret = regulator_enable(led_dev->pdata->pregulator);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] %s regulator_enable failed, ret = %d\n", 
                __FUNCTION__, led_cdev->name, ret);
            goto EXIT;
        }
    }

EXIT:
    mutex_unlock(&led_dev->pdata->mlock);
    return;
}

/************************************************************************
 * Name         : brightness_get
 * Function     : get brightness
 * Arguments
 *      input   : @led_cdev - which led
 *      output  : null
 *
 * Return       : brightness
 * Decription   : null
 ************************************************************************/
/*lint -save -e82, -e110, -e533*/
enum led_brightness balong_led_brightness_get(struct led_classdev *led_cdev)
{/*lint !e64*/
    /* argument check */
    if(!led_cdev)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        return (enum led_brightness)LED_ERROR;  
    }
    
    return (enum led_brightness)led_cdev->brightness;   
}
/*lint -restore*/
/************************************************************************
 * Name         : balong_blink_set
 * Function     : set blink
 * Arguments
 *      input   : @led_cdev - which led
 *              : @delay_on - delay time when led is on
 *              : @delay_off - delay time when led is off
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
int balong_led_blink_set(struct led_classdev *led_cdev, unsigned long *delay_on, unsigned long *delay_off)
{
    int led_id, ret;
    unsigned long local_delayon, local_delayoff;
    DR_FLA_TIME dr_fla_time;
    struct balong_led_device *led_dev = container_of(led_cdev, struct balong_led_device, cdev);

    /* argument check */
    if((!led_cdev) || (!led_dev) || (!led_dev->pdata))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        return LED_ERROR;
    }

    mutex_lock(&led_dev->pdata->mlock);

    /* get id of led */
    led_id = name_to_id(led_cdev);

    /* if null, set LED_VALUE_INVALID */
    if(!delay_on)
    {
        local_delayon = LED_VALUE_INVALID;
        delay_on = &local_delayon;
    }    

    if(!delay_off)
    {        
        local_delayoff = LED_VALUE_INVALID;
        delay_off = &local_delayoff;
    }
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]%s blink feriod %d, blink on %d\n", 
        __FUNCTION__, led_cdev->name, *delay_on + *delay_off, *delay_on);

    /* set blink */
    dr_fla_time.fla_off_us = *delay_off;
    dr_fla_time.fla_on_us = *delay_on;
    ret = bsp_dr_fla_time_set(&dr_fla_time);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set flash time failed, ret = %d.\n", 
            __FUNCTION__, led_cdev->name, ret);
        goto EXIT;
    }
    
    /* set blink mode */
    ret = bsp_dr_set_mode(led_id, PMU_DRS_MODE_FLA_FLASH);      
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set flash mode failed, ret = %d.\n", 
            __FUNCTION__, led_cdev->name, ret);
        goto EXIT;
    }    

    mutex_unlock(&led_dev->pdata->mlock);
    return LED_OK;
    
EXIT:
    mutex_unlock(&led_dev->pdata->mlock);
    return LED_ERROR;
}

/************************************************************************
 * Name         : balong_led_breath_set
 * Function     : set
 * Arguments
 *      input   : @led_cdev - which led
 *              : @full_on - delay time(us) when led is on
 *              : @full_off - delay time(us) when led is off
 *              : @fade_on - delay time(us) when led is breathing from off to on
 *              : @fade_off - delay time(us) when led is breathing form on to off
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
int balong_led_breath_set(struct led_classdev *led_cdev, unsigned long *full_on, unsigned long *full_off, unsigned long *fade_on, unsigned long *fade_off)
{/*lint !e64*/
    DR_BRE_TIME dr_bre_time;
    int led_id, ret;
    unsigned long local_fullon, local_fulloff, local_breon, local_breoff;
    struct balong_led_device *led_dev = container_of(led_cdev, struct balong_led_device, cdev);

    /* argument check */
    if((!led_cdev) || (!led_dev) || (!led_dev->pdata))
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] arguments is NULL\n", __FUNCTION__);
        return LED_ERROR;
    }

    mutex_lock(&led_dev->pdata->mlock);

    /* get id of led */
    led_id = name_to_id(led_cdev);

    /* if null, set default value */
    if(!full_on)
    {
        local_fullon = LED_VALUE_INVALID;
        full_on = &local_fullon;
    }
    else
    {
        local_fullon = us2ms(*full_on);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_ON_MS, &local_fullon);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath time valid failed, ret = %d.\n", 
                __FUNCTION__, led_cdev->name, ret);
            goto EXIT;
        }
    }

    if(!full_off)
    {
        local_fulloff = LED_VALUE_INVALID;
        full_off = &local_fulloff;
    }
    else
    {
        local_fulloff = us2ms(*full_off);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_OFF_MS, &local_fulloff);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath time valid failed, ret = %d.\n", 
                __FUNCTION__, led_cdev->name, ret);
            goto EXIT;
        }
    }
    
    if(!fade_on)
    {
        local_breon = LED_VALUE_INVALID;
        fade_on = &local_breon;
    }
    else
    {
        local_breon = us2ms(*fade_on);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_RISE_MS, &local_breon);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath time valid failed, ret = %d.\n", 
                __FUNCTION__, led_cdev->name, ret);
            goto EXIT;
        }
    }
    
    if(!fade_off)
    {
        local_breoff = LED_VALUE_INVALID;
        fade_off = &local_breoff;
    }
    else
    {
        local_breoff = us2ms(*fade_off);
        ret = set_bre_time_valid(led_id, PMU_DRS_BRE_FALL_MS, &local_breoff);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath time valid failed, ret = %d.\n", 
                __FUNCTION__, led_cdev->name, ret);
            goto EXIT;
        }
    }

    /* set time */
    dr_bre_time.bre_fall_ms = local_breoff;
    dr_bre_time.bre_off_ms = local_fulloff;
    dr_bre_time.bre_on_ms = local_fullon;
    dr_bre_time.bre_rise_ms = local_breon;
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]%d full_on %d, full_off %d, fade_on %d, fade_off %d\n",
        __FUNCTION__, led_id, dr_bre_time.bre_on_ms, dr_bre_time.bre_off_ms, dr_bre_time.bre_rise_ms, dr_bre_time.bre_fall_ms);
    
    ret = bsp_dr_bre_time_set((dr_id_e)led_id, &dr_bre_time);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath time failed, ret = %d.\n", 
            __FUNCTION__, led_cdev->name, ret);
        goto EXIT;
    }

    /* set breath mode */
    ret = set_bre_mode(led_id);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]%s ERROR: set breath mode, ret = %d.\n", 
            __FUNCTION__, led_cdev->name, ret);
        goto EXIT;
    }

    mutex_unlock(&led_dev->pdata->mlock);
    return LED_OK;
    
EXIT:
    mutex_unlock(&led_dev->pdata->mlock);
    return LED_ERROR;

}
/************************************************************************
 * Name         : register_led_by_nr
 * Function     : register led by number
 * Arguments
 *      input   : nr - LED id, from 0 to 4
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
int register_led_by_nr(struct balong_led_device *led, int nr, struct led_platform_data *pdata)
{/*lint !e64*/
    /*lint -save -e409*/ 
    led[nr].pdata->led_breath_set      = balong_led_breath_set;
	led[nr].cdev.brightness_set        = balong_led_brightness_set;
    led[nr].cdev.brightness_get        = balong_led_brightness_get;
    led[nr].cdev.blink_set             = balong_led_blink_set;
	led[nr].cdev.name                  = pdata->leds[nr].name;

    /* regist regulator device */
    led[nr].pdata->pregulator = regulator_get(NULL, led[nr].cdev.name);

#if (FEATURE_ON == MBB_COMMON)
    dr_regulator[nr] = led[nr].pdata->pregulator;    /*��ֵregulatorȫ�ֱ���*/
#endif
    
    if (IS_ERR(led[nr].pdata->pregulator))
    {
        pr_err("[%s] led%d get regulator failed\n", __FUNCTION__, nr + 1);
        return LED_ERROR;
    }

    /*lint -save -e539*/ 
	led[nr].cdev.default_trigger       = pdata->leds[nr].default_trigger;
	led[nr].cdev.flags                 = 0;
    /*lint -restore*/  
    
    /* mutex lock init */
    mutex_init(&led[nr].pdata->mlock);
    return LED_OK;
    /*lint -restore*/  
}

/************************************************************************
 * Name         : balong_led_probe
 * Function     : probe when init
 * Arguments
 *      input   : @pdev - platform device
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
static int __devinit balong_led_probe(struct platform_device *dev)
{
    struct led_platform_data *pdata = dev->dev.platform_data;
	struct balong_led_device *led   = NULL;
	int ret = LED_OK;
    unsigned int i;
	led = (struct balong_led_device *)kzalloc(sizeof(struct balong_led_device) * ARRAY_SIZE(balong_led), GFP_KERNEL);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] get buffer failed\n", __FUNCTION__);
        goto ERRO;
    }    

    /* get pdata, register led and register classdev for every led device */
    for(i = 0; i < (int)ARRAY_SIZE(balong_led); i++)
    {
        led[i].pdata = (struct balong_led_platdata *)kzalloc(sizeof(struct balong_led_platdata), GFP_KERNEL);
        if(!led[i].pdata)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] get pdata buffer failed\n", __FUNCTION__);
            goto ERRO;
        }

        /* regulator register failed is possible, as not all DRs are for LED */
        ret = register_led_by_nr(led, (int)i, pdata);
        if(!ret)
        {
        	ret = led_classdev_register(&dev->dev, &(led[i].cdev));
            if(ret)
            {
                LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]led%d sysfs register failed\n", __FUNCTION__, i + 1);
            }
        }

        if(LED3_ID <= i + 1)
        {
            /* set start delay */
            ret = bsp_dr_start_delay_set((dr_id_e)(i + 1), 0);
            if(ret)
            {
                LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]set led%d start delay ERROR, ret = %d.\n", 
                    __FUNCTION__, i + 1, ret);
                goto ERRO;
            }        
        }        
    }
            
    /* fastbootʹ�ܼĴ���֮�󣬺�˼regulator�ӿ��ж�ʧ����Ҫ�ڴ���enable regulator*/
    /*�رտ����Ѿ�ʹ�ܵ�DR��*/
#ifdef BSP_CONFIG_BOARD_E5
    /*E5��̬�ڿ���ʱ������DR4���Ƶ�bat_led:green��*/
   
    ret = regulator_enable(led[3].pdata->pregulator);
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] regulator_enable failed, ret = %d\n", __FUNCTION__, ret);
    }
#endif

    /* save "led" as private data */
    /*lint -save -e539*/ 
	platform_set_drvdata(dev, led);
    /*lint -restore*/ 

#if (FEATURE_ON == MBB_LED)

    printk(KERN_ERR "[%s] before change, g_ledready_flag = %d . \n", __func__, g_ledready_flag);

    g_ledready_flag = 1;    /*�ڵ����*/
    
    if(1 == g_set_flag)    /*�����ڵ����ǰ�ĺ����ӿڵ���*/
    {
        for(i = 0; i < DR_NUM; i++)
        {
            if(1 == g_drled_date_in[i].flag)
            {
                (void)led_kernel_status_set(g_drled_date_in[i].name, g_drled_date_in[i].statu);
            }            
        }
        
        for(i = 0; i < GPIO_NUM; i++)
        {
            if(1 == g_gpioled_date_in[i].flag)
            {
                (void)led_kernel_status_set(g_gpioled_date_in[i].name, g_gpioled_date_in[i].statu);
            }
        }
    }
#endif

    /* timer create for three-color-led */
	led_softtimer.func = (softtimer_func)do_led_threecolor_flush;
	led_softtimer.para = 0;                              
	led_softtimer.timeout = 0;                        /* ��ʱ���ȣ���λms */
	led_softtimer.wake_type = SOFTTIMER_WAKE;
    
	if (bsp_softtimer_create(&led_softtimer))
	{
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s]ERROR: softtimer create failed\n", __FUNCTION__);
		goto ERRO;
	}  
    
    /* read NV and store for three-color-led*/    
    if(g_nv_led.g_already_read != LED_NV_ALREADY_READ)
    {
    	ret = (int)bsp_nvm_read(NV_ID_DRV_LED_CONTROL,(u8 *)g_nv_led.g_led_state_str_om, LED_LIGHT_STATE_MAX * LED_CONFIG_MAX_LTE * sizeof(LED_CONTROL_NV_STRU));
    	if(NV_OK != ret)
    	{
    		LED_TRACE(LED_DEBUG_LEVEL(ERROR), "[%s]WARNING: read nv failed, use default, ret = %d!\n", __FUNCTION__, ret);

            /* ��nvʧ�ܣ�����Ĭ��ֵ */
            memcpy(g_nv_led.g_led_state_str_om, g_led_state_str_lte, 
                LED_LIGHT_STATE_MAX * LED_CONFIG_MAX_LTE * sizeof(LED_CONTROL_NV_STRU));
    	}
    }    

/*lint -e553*/ 
#if (FEATURE_ON == MBB_WPG_COMMON)
    /*��NV50032�ӻ�ȡ���Ƶķ���*/
    unsigned long ulLEDStatus = DEF;
    if(NV_OK != bsp_nvm_read(en_NV_Item_HUAWEI_PCCW_HS_HSPA_BLUE,&ulLEDStatus,sizeof(unsigned long)))
    {
        /*NV��ȡʧ�ܣ���ʹ��Ĭ�����Ʒ���*/
        ulLEDStatus = DEF;
    }

    /*����NV��ȡֵ��ʹ�ò�ͬ���������÷���*/
    switch(ulLEDStatus)
    {
        case(DEF):/*Ĭ������*/
        {
            break;
        }

        case(HONGKONG):/*���PCCW*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_HongKong_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(RUSSIA):/*����˹MTS*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_Russia_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(VODAFONE):/*Vodafone*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_Vodafone_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(CHINAUNION):/*�й���ͨ*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_ChinaUnion_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(SFR):/*����SFR*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_FranceSFR_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(BSNL):/*ӡ��BSNL*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_IndiaBSNL_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(KPN):/*����KPN*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_HolandKPN_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(SOFTBANK):/*�ձ�����*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_SoftBank_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        case(EMOBILE):/*�ձ�EM*/
        {
            memcpy(g_nv_led.g_led_state_str_om,g_led_Emobile_StateStr,sizeof(g_led_state_str_lte));
            break;
        }

        default:
        {
            break;
        }
    }
#endif
/*lint +e553*/ 

    /* icc register for three-color-led*/
    ret = bsp_icc_event_register(LED_ICC_CHN_ID, led_threecolor_flush, NULL, NULL, NULL);
	if(ret != LED_OK)
	{
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] register icc event failed, ret = 0x%x\n", __FUNCTION__, ret);
        goto ERRO;
	}
    
    LED_TRACE(LED_DEBUG_LEVEL(INFO),"[%s]: succeed!!!!!register icc callback\n", __FUNCTION__);

    return ret;
    
ERRO:
    for(i = 0; i < ARRAY_SIZE(balong_led); i++)
    {
        if(!led)
        {
            continue;
        }
    
        if(led[i].pdata->pregulator)
        {
            kfree(led[i].pdata->pregulator);
            led[i].pdata->pregulator = NULL;
        }
        if(led[i].pdata)
        {
            kfree(led[i].pdata);
            led[i].pdata = NULL;
        }
    }
    if(led)
    {
        kfree(led);
        led = NULL;
    }
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS),"LED init failed\n");
    return LED_ERROR; /*lint !e438*/
}

/************************************************************************
 * Name         : balong_led_remove
 * Function     : remove
 * Arguments
 *      input   : @pdev - platform device
 *      output  : null
 *
 * Return       : 0 - success
 *              : else - error
 * Decription   : null
 ************************************************************************/
static int __devexit balong_led_remove(struct platform_device *pdev)
{
    int i;
    struct balong_led_device *led = platform_get_drvdata(pdev);
    if(!led)
    {
        return LED_ERROR;
    }
    
    if(&led->cdev)
    {
        led_classdev_unregister(&led->cdev);
    }

    for(i = 0; i < (int)(ARRAY_SIZE(balong_led)); i++)
    {
        if(!led[i].pdata)
        {
            continue;
        }
        
        regulator_put(led[i].pdata->pregulator);
        led_classdev_unregister(&(led[i].cdev));
        if(led[i].pdata->pregulator)
        {
            kfree(led[i].pdata->pregulator);
            led[i].pdata->pregulator = NULL;
        }
        if(led[i].pdata)
        {
            kfree(led[i].pdata);
            led[i].pdata = NULL;
        }
    }

    kfree(led);
    led = NULL;

    return LED_OK; /*lint !e438*/
}

/*****************************************************************************
 �� �� ��  : drv_led_flash
 ��������  : ��ɫ�����ã����ϲ��ṩ�Ľӿ�
 �������  : status����ɫ�Ƶ�״̬
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
int drv_led_flash(unsigned long state) 
{
    int ret = LED_ERROR;
	struct balong_led_device *led = NULL;
    led = (struct balong_led_device *)platform_get_drvdata(&balong_led_dev);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ALWAYS),"get balong led device failed.\n");
        return LED_ERROR;
    }    

    g_arg.new_state = (unsigned char)state;
    g_arg.ctl = MNTN_LED_STATUS_FLUSH;    

    ret = do_led_threecolor_flush();
    if(ret)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR), "[%s]three color led flush failed, ret = %d!\n", __FUNCTION__, ret);
        return LED_ERROR;
    }

    return LED_OK;
}


/*************************************************************************/
/* for power manager */
#ifdef CONFIG_PM
  int balong_led_prepare(struct device *dev)
{
    /* ok, return */
    return 0;
}

  void balong_led_complete(struct device *dev)
{
    /* ok, return */
    return;
}

  int balong_led_suspend(struct device *dev)
{
    /* ok, return */
    return 0;
}

  int balong_led_resume(struct device *dev)
{
    /* ok, return */
    return 0;
}

 const struct dev_pm_ops balong_led_dev_pm_ops =
{
    .prepare    =   balong_led_prepare,
    .complete   =   balong_led_complete,
    .suspend    =   balong_led_suspend,
    .resume     =   balong_led_resume,
};

#define BALONG_LED_PM_OPS (&balong_led_dev_pm_ops)
#else
#define BALONG_LED_PM_OPS NULL
#endif

/* LED platform driver */
 struct platform_driver balong_led_drv = {
	.probe		= balong_led_probe,
	.remove		= balong_led_remove,
	.driver		= {
		.name		= "balong_led",
		.owner		= THIS_MODULE,
	    .bus        = &platform_bus_type,
		.pm         = BALONG_LED_PM_OPS,
	},
};

#if (FEATURE_ON == MBB_LED_DR)
/************************************************************************
 * Name         : led_regulator_dr_enable
 * Function     :
 * Arguments
 *      input   : led_id:0~4
 *      output  : null
 *
 * Return       : 0 - ok
 *                else - fail
 * Decription   : enable dr regulator
 ************************************************************************/
int led_dr_regulator_enable(int led_id)
{
    int ret = LED_ERROR;
    struct balong_led_device *led = NULL;

    led_id = led_id - 1;
    if(0 > led_id || led_id > 4)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] led_id = %d, dr index error.\n", __FUNCTION__, led_id);
        return LED_ERROR;
    }
    
    led = (struct balong_led_device *)platform_get_drvdata(&balong_led_dev);
    if(!led)
    {
        LED_TRACE(LED_DEBUG_LEVEL(ALWAYS),"get balong led device failed.\n");
        return LED_ERROR;
    }

    /* enable led */
    if(regulator_is_enabled(led[led_id].pdata->pregulator))
    {
        ret = regulator_enable(led[led_id].pdata->pregulator);
        if(ret)
        {
            LED_TRACE(LED_DEBUG_LEVEL(ERROR),"[%s] regulator_enable failed, ret = %d\n", __FUNCTION__, ret);
            return LED_ERROR;
        }
    }
    
    return LED_OK;
}
EXPORT_SYMBOL(led_dr_regulator_enable);
#endif

/*******************************************************************************
 * FUNC NAME:
 * balong_led_init() - register dirver and device for led driver
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * Linux standard driver initialization interface.
 *
 * CALL FUNC:
 *
 ********************************************************************************/
int __init bsp_led_init(void)
{
    int result;

    result = platform_driver_register(&balong_led_drv);
    if (result < 0)
    {
        return result;
    }

    result = platform_device_register(&balong_led_dev);
    if (result < 0)
    {
        platform_driver_unregister(&balong_led_drv);
        return result;
    }

    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS),"LED init OK\n");

    return result;
}

/*******************************************************************************
 * FUNC NAME:
 * led_module_exit() - unregister dirver for led driver
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * Linux standard driver de_initialization interface.
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
static void __exit bsp_led_exit (void)
{
    platform_driver_unregister(&balong_led_drv);
    platform_device_unregister(&balong_led_dev);
}

/*****************************************************************************/

module_init(bsp_led_init);   /*lint !e19 */
module_exit(bsp_led_exit);       /*lint !e19 */

/************************************************************************/
/*
 * Just for DEBUG
 * ���ƴ�ӡ����
 */
void led_set_debug_level(void)
{
    led_debug_level = LED_DEBUG_ALL;
}

void led_get_debug_level(void)
{
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS), "INFO    0x%08x\n", (unsigned int)LED_DEBUG_INFO);
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS), "WARNING 0x%08x\n", (unsigned int)LED_DEBUG_WARNING);
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS), "ERROR   0x%08x\n", (unsigned int)LED_DEBUG_ERROR);
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS), "ALWAYS  0x%08x\n", (unsigned int)LED_DEBUG_ALWAYS);
    LED_TRACE(LED_DEBUG_LEVEL(ALWAYS), "Current level 0x%08x\n", led_debug_level);
} 

#if(FEATURE_ON == MBB_LED_DR)
/************************************************************************
 *����ԭ�� �� static int change_name(const char *name)
 *����     �� ���ݲ�����led����ѡ��dr����id
 *����     ��������led����
 *���     �� ��
 *����ֵ   �� �ɹ�0��ʧ��-1
 *�޸���ʷ     :
   ��    ��   : 20140729
   �޸�����   : �����ɺ���
*************************************************************************/
static int change_name(const char *name)
{
    /* reject null */
    if(NULL == name)
    {
        printk(KERN_ERR "[%s] name is null. \n", __FUNCTION__);
        return  - 1;
    }

    if(!strncmp(name, LED_NODE_1, strlen(LED_NODE_1)))
    {
        g_dr_id = PMU_DR01;
        return 0;
    }
    else if(!strncmp(name, LED_NODE_2, strlen(LED_NODE_2)))
    {
        g_dr_id = PMU_DR02;
        return 0;
    }
    else if(!strncmp(name, LED_NODE_3, strlen(LED_NODE_3)))
    {
        g_dr_id = PMU_DR03;
        return 0;
    }
    else if(!strncmp(name, LED_NODE_4, strlen(LED_NODE_4)))
    {
        g_dr_id = PMU_DR04;
        return 0;
    }
    else if(!strncmp(name, LED_NODE_5, strlen(LED_NODE_5)))
    {
        g_dr_id = PMU_DR05;
        return 0;
    }
    else
    {
        printk(KERN_ERR "[%s] match name is fail. \n", __FUNCTION__);
        return  -1;
    }
    
}


/************************************************************************
 *����ԭ�� �� static void dr_led_switch(int numb, int onoff)
 *����     �� dr����led��״̬�л�����
 *����     ����Ӧ�����±�int numb, ״̬ int onoff
 *���     �� ��
 *����ֵ   �� 
 *�޸���ʷ     :
   ��    ��   : 20140729
   �޸�����   : �����ɺ���
*************************************************************************/
static int dr_led_switch(int numb, int onoff)
{
    int ret = 0;
    DR_BRE_TIME dr_bre_time_st;
 
    ret = change_name(led_dr[numb].name);
    if(0 != ret)
    {
        return -1;
    }

    g_dr_brightness[g_dr_id - 1] = -1;        /*����ֵ��λ*/
    /* ���ú���ģʽ */
    ret = set_bre_mode(g_dr_id);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set breath mode, ret = %d, dr = %d.\n", __FUNCTION__, ret, g_dr_id);
        return -1;
    }

    /* ���ú�����ʱ�� �˴�ֻ���ó���ʱ�䣬���ɿ��ؿ���*/
    dr_bre_time_st.bre_fall_ms = 0;
    dr_bre_time_st.bre_off_ms = 0;
    dr_bre_time_st.bre_on_ms = (unsigned int)(long_time);
    dr_bre_time_st.bre_rise_ms = 0;

    ret = set_bre_time_valid(g_dr_id, PMU_DRS_BRE_FALL_MS, &dr_bre_time_st.bre_fall_ms);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set bre_fall_ms  valid failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, g_dr_id);
        return -1;
    }
    ret = set_bre_time_valid(g_dr_id, PMU_DRS_BRE_OFF_MS, &dr_bre_time_st.bre_off_ms);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set bre_off_ms  valid failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, g_dr_id);
        return -1;
    }
    ret = set_bre_time_valid(g_dr_id, PMU_DRS_BRE_ON_MS, &dr_bre_time_st.bre_on_ms);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set bre_on_ms  valid failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, g_dr_id);
        return -1;
    }
    ret = set_bre_time_valid(g_dr_id, PMU_DRS_BRE_RISE_MS, &dr_bre_time_st.bre_rise_ms);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set bre_rise_ms  valid failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, g_dr_id);
        return -1;
    }

    ret = bsp_dr_bre_time_set((dr_id_e)(g_dr_id), &dr_bre_time_st);
    if(ret)
    {
        printk(KERN_ERR "[%s] ERROR: set breath time failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, g_dr_id);
        return -1;
    }

    /*�ر�*/
    if(0 == onoff)
    {
        /* if enabled, disable */
        if(regulator_is_enabled(dr_regulator[g_dr_id - 1]))
        {
            ret = regulator_disable(dr_regulator[g_dr_id - 1]);
            if(ret)
            {
                printk(KERN_ERR "[%s] regulator_disable failed, ret = %d, dr = %d.\n", __FUNCTION__, ret, g_dr_id);
                return -1;
            }
        }
        
        return 0;
    }
    
    /*���õ�������Ϊ3mA*/
    regulator_set_current_limit(dr_regulator[g_dr_id - 1], current, current);

    /* ���� */
    if(!regulator_is_enabled(dr_regulator[g_dr_id - 1]))
    {
        ret = regulator_enable(dr_regulator[g_dr_id - 1]);
        if(ret)
        {
            printk(KERN_ERR "[%s] regulator_enable failed, ret = %d, dr = %d\n", __FUNCTION__, ret, g_dr_id);
            return -1;
        }
    }

    return 0;
}
#endif

#if(FEATURE_ON == MBB_LED_GPIO)
/************************************************************************
 *����ԭ�� �� static void gpio_led_switch(int numb, int onoff)
 *����     �� gpio����led��״̬�л�����
 *����     ����Ӧ�����±�int numb, ״̬ int onoff
 *���     �� ��
 *����ֵ   �� 
 *�޸���ʷ     :
   ��    ��   : 20140723
   �޸�����   : �����ɺ���
*************************************************************************/
static void gpio_led_switch(int numb, int onoff)
{    
    (void)gpio_direction_output(gpio_exp_leds_config[numb].gpio, onoff);   
 
    return;
}
#endif

#if (FEATURE_ON == MBB_LED)
/************************************************************************
 *����ԭ�� �� int led_kernel_status_set(char* led_name, int onoff)
 *����     �� �ṩkernel�׶� ��led�ƿ��ƽӿ�
 *����     ��Ҫ���Ƶ�led������, ״̬ int onoff
 *���     �� ��
 *����ֵ   �� �ɹ�0��ʧ��-1
 *�޸���ʷ     :
   ��    ��   : 20140729
   �޸�����   : �����ɺ���
*************************************************************************/
int led_kernel_status_set(char* led_name, int onoff)
{
    int i = 0;
    int num = 0;
    int ret = 0;
    
    printk(KERN_ERR "[%s] input led_name is %s ,onoff = %d ,g_set_flag = %d. \n", __func__, led_name, onoff,g_set_flag);
    
    if(NULL == led_name)
    {
        printk(KERN_ERR "[%s] input led_name is NULL. \n",  __func__);
        return - 1;
    }
    if((0 != onoff) && (1 != onoff))
    {
        printk(KERN_ERR "[%s] input onoff = [%d] wrong. \n", __func__, onoff);
        return -1;
    }
    
    mutex_lock(&g_led_kernelset_lock);
    g_set_flag = 1;  /*�������ñ�ʶ*/
    
    if(0 == g_ledready_flag)
    {
        /*������α���Ϊȫ�ֱ���*/
#if(FEATURE_ON == MBB_LED_GPIO)
        num = (int)(ARRAY_SIZE(gpio_exp_leds_config));

        for(i = 0; i < num; i++)     /*����name  ���Ҷ�Ӧ���Ƶ�led*/
        {
            if(!strncmp(gpio_exp_leds_config[i].name , led_name , strlen(led_name) ))
            {
                (void)strncpy(g_gpioled_date_in[i].name, led_name, strlen(led_name));
                g_gpioled_date_in[i].statu = onoff;
                g_gpioled_date_in[i].flag = 1;
                mutex_unlock(&g_led_kernelset_lock);
                return 0;    
            }       
        }
#endif
#if(FEATURE_ON == MBB_LED_DR)
        ret = change_name(led_name);
        if(0 != ret)
        {
            printk(KERN_ERR "[%s] match name is fail. \n", __FUNCTION__);
            mutex_unlock(&g_led_kernelset_lock);
            return -1;
        }
        
        (void)strncpy(g_drled_date_in[g_dr_id - 1].name, led_name, strlen(led_name));
        g_drled_date_in[g_dr_id - 1].statu = onoff;
        g_drled_date_in[g_dr_id - 1].flag = 1;
#endif    

        printk(KERN_ERR "[%s] led not ready\n", __FUNCTION__);
        mutex_unlock(&g_led_kernelset_lock);
        return -1;
    }

#if(FEATURE_ON == MBB_LED_GPIO)
    num = (int)(ARRAY_SIZE(gpio_exp_leds_config));

    for(i = 0; i < num; i++)     /*����name  ���Ҷ�Ӧ���Ƶ�led*/
    {
        if(!strncmp(gpio_exp_leds_config[i].name , led_name , strlen(led_name) ))
        {
            gpio_led_switch( i , onoff );      /*���ҳɹ�gpio���Ƶ���*/
            mutex_unlock(&g_led_kernelset_lock);
            return 0;    
        }       
    }
#endif

#if(FEATURE_ON == MBB_LED_DR)
    num = (int)(ARRAY_SIZE(led_dr));

    for(i = 0; i < num; i++)     /*����name  ���Ҷ�Ӧ���Ƶ�led*/
    {
        if(!strncmp(led_dr[i].name , led_name , strlen(led_name) ))
        {
            ret = dr_led_switch( i , onoff);      /*���ҳɹ�dr���Ƶ���*/
            if(0 != ret)
            {
                printk(KERN_ERR "[%s] %s dr_led_switch fail . \n", __func__, led_name);  /*dr�ƿ���ʧ��*/
                mutex_unlock(&g_led_kernelset_lock);
                return -1;
            }
            mutex_unlock(&g_led_kernelset_lock);
            return 0;    
        }       
    }
#endif

    printk(KERN_ERR "[%s] input led_name no found. \n", __func__);  /*����ʧ��*/
    mutex_unlock(&g_led_kernelset_lock);
    return  -1;
}


#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon Tech.Co.,Ltd.<lusuo@hisilicon.com>");
MODULE_DESCRIPTION("BalongV700R200 Hisilicon LED driver");

#ifdef __cplusplus
}
#endif