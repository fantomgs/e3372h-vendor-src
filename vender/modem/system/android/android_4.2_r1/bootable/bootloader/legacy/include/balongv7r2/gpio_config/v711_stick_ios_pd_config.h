/**************************************************************
CAUTION : This file is Auto Generated by VBA based on *.xls.
          So, don't modify this file manually!
***************************************************************/
#ifndef V711_stick_IOS_PD_CONFIG_H_
#define V711_stick_IOS_PD_CONFIG_H_

/*配置管脚复用关系、内部上下拉以及驱动能力*/
#define V711_stick_IOS_PD_CONFIG \
do{\
\
/*配置NANDFLASH(14个PIN)*/\
\
/*配置BOOT_MODE,JTAG_MODE(6个PIN）*/\
\
/*配置MMC1(6个PIN）*/\
    /*mmc1_clk管脚复用配置*/\
    OUTSET_IOS_PD_IOM_CTRL21;\
    /*mmc1_clk管脚Drive电流配置*/\
    I16MASET_IOS_PD_IOM_CTRL21;\
\
\
/*配置JTAG0（6个PIN）*/\
\
/*配置GPIO/GSBI_0(20PIN）*/\
    /*gpio2[0]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_0);\
\
    /*gpio2[1]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_1);\
\
    /*gpio2[2]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_2);\
\
    /*gpio2[3]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_3);\
\
    /*gpio2[4]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_4);\
\
    /*gpio2[5]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_5);\
\
    /*gpio2[6]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_6);\
\
    /*gpio2[7]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_7);\
\
    /*gpio2[8]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_8);\
\
    /*gpio2[9]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_9);\
\
    /*gpio2[10]管脚上下拉配置*/\
    PUSET_IOS_PD_IOM_CTRL38;\
\
    /*gpio2[11]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_11);\
\
    /*gpio2[12]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_12);\
\
    /*gpio2[13]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_13);\
\
    /*gpio2[14]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_14);\
\
    /*gpio2[15]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_15);\
\
    /*gpio2[16]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_16);\
\
    /*gpio2[17]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_17);\
\
    /*gpio2[18]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_18);\
\
    /*gpio2[19]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_19);\
\
\
/*配置GPIO/GSBI_1(7PIN）*/\
    /*gpio2[20]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_20);\
\
    /*gpio2[21]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_21);\
\
    /*gpio2[22]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_22);\
\
    /*gpio2[23]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_23);\
\
    /*gpio2[24]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_24);\
\
    /*gpio2[25]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_25);\
\
    /*gpio2[26]管脚输入方向配置*/\
    gpio_direction_input(GPIO_2_26);\
\
\
/*配置RF线控：CH0 FEM(7PIN）*/\
    /*ch0_apt_pdm管脚复用配置*/\
    SET_IOS_CH0_APT_PDM_CTRL1_1;\
    OUTSET_IOS_PD_IOM_CTRL58;\
    CLR_IOS_GPIO1_11_CTRL1_1;\
    /*ch0_apt_pdm管脚上下拉配置*/\
    NASET_IOS_PD_IOM_CTRL58;\
\
    /*ch0_mipi_clk管脚复用配置*/\
    SET_IOS_CH0_MIPI_CTRL1_1;\
    OUTSET_IOS_PD_IOM_CTRL59;\
    CLR_IOS_GPIO1_12_CTRL1_1;\
    /*ch0_mipi_clk管脚上下拉配置*/\
    NASET_IOS_PD_IOM_CTRL59;\
\
    /*ch0_mipi_data管脚复用配置*/\
    SET_IOS_CH0_MIPI_CTRL1_1;\
    CLR_IOS_GPIO1_13_CTRL1_1;\
    /*ch0_mipi_data管脚上下拉配置*/\
    NASET_IOS_PD_IOM_CTRL60;\
\
    /*ch0_afc_pdm管脚上下拉配置*/\
    NASET_IOS_PD_IOM_CTRL61;\
\
\
/*配置RF线控：FEM(17个PIN）*/\
    /*gpio1[15]管脚上下拉配置*/\
    PUSET_IOS_PD_IOM_CTRL62;\
\
    /*gpio1[16]管脚输入方向配置*/\
    gpio_direction_input(GPIO_1_16);\
\
    /*antpa_sel[2]管脚复用配置*/\
    SET_IOS_ANTPA_SEL2_CTRL1_1;\
    OUTSET_IOS_PD_IOM_CTRL64;\
    CLR_IOS_GPIO1_17_CTRL1_1;\
    /*antpa_sel[2]管脚上下拉配置*/\
    NASET_IOS_PD_IOM_CTRL64;\
\
    /*antpa_sel[3]管脚复用配置*/\
    SET_IOS_ANTPA_SEL3_CTRL1_1;\
    OUTSET_IOS_PD_IOM_CTRL65;\
    CLR_IOS_GPIO1_18_CTRL1_1;\
    /*antpa_sel[3]管脚上下拉配置*/\
    NASET_IOS_PD_IOM_CTRL65;\
\
    /*antpa_sel[4]管脚复用配置*/\
    SET_IOS_ANTPA_SEL4_CTRL1_1;\
    OUTSET_IOS_PD_IOM_CTRL66;\
    CLR_IOS_GPIO1_19_CTRL1_1;\
    /*antpa_sel[4]管脚上下拉配置*/\
    NASET_IOS_PD_IOM_CTRL66;\
\
    /*antpa_sel[5]管脚复用配置*/\
    SET_IOS_ANTPA_SEL5_CTRL1_1;\
    OUTSET_IOS_PD_IOM_CTRL67;\
    CLR_IOS_GPIO1_20_CTRL1_1;\
    /*antpa_sel[5]管脚上下拉配置*/\
    NASET_IOS_PD_IOM_CTRL67;\
\
    /*antpa_sel[6]管脚复用配置*/\
    SET_IOS_ANTPA_SEL6_CTRL1_1;\
    OUTSET_IOS_PD_IOM_CTRL68;\
    CLR_IOS_GPIO1_21_CTRL1_1;\
    /*antpa_sel[6]管脚上下拉配置*/\
    NASET_IOS_PD_IOM_CTRL68;\
\
    /*antpa_sel[7]管脚复用配置*/\
    SET_IOS_ANTPA_SEL7_CTRL1_1;\
    OUTSET_IOS_PD_IOM_CTRL69;\
    CLR_IOS_GPIO1_22_CTRL1_1;\
    /*antpa_sel[7]管脚上下拉配置*/\
    NASET_IOS_PD_IOM_CTRL69;\
\
    /*antpa_sel[8]管脚复用配置*/\
    SET_IOS_ANTPA_SEL8_CTRL1_1;\
    OUTSET_IOS_PD_IOM_CTRL70;\
    CLR_IOS_GPIO1_23_CTRL1_1;\
    /*antpa_sel[8]管脚上下拉配置*/\
    NASET_IOS_PD_IOM_CTRL70;\
\
    /*gpio1[24]管脚输入方向配置*/\
    gpio_direction_input(GPIO_1_24);\
\
    /*gpio1[25]管脚输入方向配置*/\
    gpio_direction_input(GPIO_1_25);\
\
    /*gpio1[26]管脚输入方向配置*/\
    gpio_direction_input(GPIO_1_26);\
\
    /*gpio1[27]管脚输入方向配置*/\
    gpio_direction_input(GPIO_1_27);\
\
    /*gpio1[28]管脚输入方向配置*/\
    gpio_direction_input(GPIO_1_28);\
\
    /*gpio1[29]管脚输入方向配置*/\
    gpio_direction_input(GPIO_1_29);\
\
    /*gpio1[30]管脚输入方向配置*/\
    gpio_direction_input(GPIO_1_30);\
\
    /*gpio1[31]管脚输入方向配置*/\
    gpio_direction_input(GPIO_1_31);\
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

#endif

