/*************************************************************************
*   版权所有(C) 1987-2013, 深圳华为技术有限公司.
*
*   文 件 名 :  bsp_gic_pm.h
*
*   描    述 :  gic 低功耗保存恢复寄存器接口文件
*************************************************************************/
#ifndef __BSP_GIC_PM_H__
#define __BSP_GIC_PM_H__
/*****************************************************************************
* 函 数 名     : gic_suspend
*
* 功能描述  : 保存gic寄存器
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值     : 无
*
* 修改记录  : 2013年7月3日   lixiaojie     
                          
*****************************************************************************/
void gic_suspend(void);
/*****************************************************************************
* 函 数 名     : gic_resume
*
* 功能描述  : 恢复gic寄存器
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值     : 无
*
* 修改记录  : 2013年7月3日   lixiaojie     
                          
*****************************************************************************/
void gic_resume(void);
#endif
