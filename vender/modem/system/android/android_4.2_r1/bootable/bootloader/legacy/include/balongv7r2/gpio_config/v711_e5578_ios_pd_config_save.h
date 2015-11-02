/**************************************************************
CAUTION : This file is Auto Generated by VBA based on *.xls.
          So, don't modify this file manually!
***************************************************************/
#ifndef  V711_E5578_IOS_PD_SAVE_H_
#define  V711_E5578_IOS_PD_SAVE_H_

/*保存掉电域管脚复用关系、内部上下拉以及驱动能力*/
#define  V711_E5578_IOS_PD_CONFIG_SAVE \
do{\
\
/*配置NANDFLASH(14个PIN)*/\
\
/*配置BOOT_MODE,JTAG_MODE(6个PIN）*/\
    /*antpa_sel[17]管脚复用配置保存*/\
    add_ios_list(IOS_PD_AF_CTRL1);\
    add_ios_list(IOS_PD_IOM_CTRL15);\
    /*antpa_sel[17]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL15);\
\
    /*antpa_sel[18]管脚复用配置保存*/\
    add_ios_list(IOS_PD_AF_CTRL1);\
    add_ios_list(IOS_PD_IOM_CTRL16);\
    /*antpa_sel[18]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL16);\
\
    /*antpa_sel[19]管脚复用配置保存*/\
    add_ios_list(IOS_PD_AF_CTRL1);\
    add_ios_list(IOS_PD_IOM_CTRL17);\
    /*antpa_sel[19]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL17);\
\
    /*antpa_sel[20]管脚复用配置保存*/\
    add_ios_list(IOS_PD_AF_CTRL1);\
    add_ios_list(IOS_PD_IOM_CTRL18);\
    /*antpa_sel[20]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL18);\
\
    /*antpa_sel[21]管脚复用配置保存*/\
    add_ios_list(IOS_PD_AF_CTRL1);\
    add_ios_list(IOS_PD_IOM_CTRL19);\
    /*antpa_sel[21]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL19);\
\
\
/*配置MMC1(6个PIN）*/\
    /*gpio1[5]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL1);\
    add_ios_list(IOS_PD_AF_CTRL2);\
\
    /*gpio1[6]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL1);\
    add_ios_list(IOS_PD_AF_CTRL2);\
    /*gpio1[6]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL22);\
\
    /*gpio1[7]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL1);\
    add_ios_list(IOS_PD_AF_CTRL2);\
    /*gpio1[7]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL23);\
\
    /*gpio1[8]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL1);\
    add_ios_list(IOS_PD_AF_CTRL2);\
    /*gpio1[8]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL24);\
\
    /*gpio1[9]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL1);\
    add_ios_list(IOS_PD_AF_CTRL2);\
    /*gpio1[9]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL25);\
\
    /*gpio1[10]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL1);\
    add_ios_list(IOS_PD_AF_CTRL2);\
    /*gpio1[10]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL26);\
\
\
/*配置JTAG0（6个PIN）*/\
\
/*配置GPIO/GSBI_0(20PIN）*/\
    /*gpio2[0]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL28);\
\
    /*gpio2[1]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL29);\
\
    /*gpio2[3]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL31);\
\
    /*gpio2[8]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL36);\
\
    /*lcd_rst_n管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL37);\
    /*lcd_rst_n管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL37);\
\
    /*i2c0_sda管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
\
    /*i2c0_scl管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
\
    /*antpa_sel[22]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL5);\
    add_ios_list(IOS_PD_IOM_CTRL40);\
    /*antpa_sel[22]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL40);\
\
    /*antpa_sel[23]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL5);\
    add_ios_list(IOS_PD_IOM_CTRL41);\
    /*antpa_sel[23]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL41);\
\
    /*gpio2[15]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL43);\
\
    /*spi0_cs0_n管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL44);\
    /*spi0_cs0_n管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL44);\
\
    /*spi0_clk管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL45);\
    /*spi0_clk管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL45);\
\
    /*spi0_dio管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    /*spi0_dio管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL46);\
\
\
/*配置GPIO/GSBI_1(7PIN）*/\
    /*gpio2[20]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL48);\
\
    /*gpio2[22]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL50);\
\
    /*gpio2[23]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL51);\
\
    /*gpio2[26]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL54);\
\
\
/*配置RF线控：CH0 FEM(7PIN）*/\
    /*ch0_apt_pdm管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL4);\
    add_ios_list(IOS_PD_AF_CTRL8);\
    add_ios_list(IOS_PD_IOM_CTRL58);\
\
    /*ch0_mipi_clk管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL4);\
    add_ios_list(IOS_PD_AF_CTRL8);\
    add_ios_list(IOS_PD_IOM_CTRL59);\
\
    /*ch0_mipi_data管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL4);\
    add_ios_list(IOS_PD_AF_CTRL8);\
\
\
/*配置RF线控：FEM(17个PIN）*/\
    /*antpa_sel[0]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL62);\
    /*antpa_sel[0]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL62);\
\
    /*antpa_sel[2]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL64);\
    /*antpa_sel[2]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL64);\
\
    /*antpa_sel[3]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL65);\
    /*antpa_sel[3]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL65);\
\
    /*antpa_sel[4]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL66);\
    /*antpa_sel[4]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL66);\
\
    /*antpa_sel[5]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL67);\
    /*antpa_sel[5]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL67);\
\
    /*antpa_sel[6]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL68);\
    /*antpa_sel[6]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL68);\
\
    /*antpa_sel[7]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL69);\
    /*antpa_sel[7]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL69);\
\
    /*antpa_sel[8]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL70);\
    /*antpa_sel[8]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL70);\
\
    /*antpa_sel[9]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL71);\
    /*antpa_sel[9]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL71);\
\
    /*antpa_sel[10]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL72);\
    /*antpa_sel[10]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL72);\
\
    /*antpa_sel[11]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL73);\
    /*antpa_sel[11]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL73);\
\
    /*antpa_sel[12]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL74);\
    /*antpa_sel[12]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL74);\
\
    /*antpa_sel[13]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL75);\
    /*antpa_sel[13]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL75);\
\
    /*antpa_sel[14]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL76);\
    /*antpa_sel[14]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL76);\
\
    /*antpa_sel[15]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL77);\
    /*antpa_sel[15]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL77);\
\
    /*antpa_sel[16]管脚复用配置保存*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL78);\
    /*antpa_sel[16]管脚上下拉配置保存*/\
    add_ios_list(IOS_PD_IOM_CTRL78);\
\
\
/*配置ABB信号（18个PIN）*/\
\
/*配置LPDDR2信号（61个PIN） CA信号（16个PIN）*/\
\
/*配置LPDDR2信号（61个PIN） DQ信号（45个PIN）*/\
\
/*配置LPDDR2 KGD信号（1个PIN）*/\
\
/*配置PCIE（7个PIN）*/\
\
/*配置PCIE 差分时钟（2个PIN）*/\
\
/*配置HSIC（2个PIN）*/\
\
}while(0)

/*管脚配置宏for drv*/
#define  V711_E5578_IOS_CONFIG_SAVE \
do{\
     V711_E5578_IOS_AO_CONFIG_SAVE;\
     V711_E5578_IOS_PD_CONFIG_SAVE;\
}while(0)

#endif

