/**************************************************************
CAUTION : This file is Auto Generated by VBA based on *.xls.
          So, don't modify this file manually!
***************************************************************/
#ifndef  CPE_IOS_PD_SAVE_H_
#define  CPE_IOS_PD_SAVE_H_

/*���������ܽŸ��ù�ϵ���ڲ��������Լ���������*/
#define  CPE_IOS_PD_CONFIG_SAVE \
do{\
\
/*����NANDFLASH(14��PIN)*/\
    /*nf_ce0_n�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL0);\
\
    /*nf_wr_n�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL3);\
\
    /*nf_rd_n�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL4);\
\
\
/*����BOOT_MODE,JTAG_MODE(6��PIN��*/\
    /*antpa_sel[17]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_AF_CTRL1);\
    add_ios_list(IOS_PD_IOM_CTRL15);\
\
    /*antpa_sel[18]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_AF_CTRL1);\
    add_ios_list(IOS_PD_IOM_CTRL16);\
\
    /*antpa_sel[19]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_AF_CTRL1);\
    add_ios_list(IOS_PD_IOM_CTRL17);\
\
    /*antpa_sel[20]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_AF_CTRL1);\
    add_ios_list(IOS_PD_IOM_CTRL18);\
\
    /*gpio1[4]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_AF_CTRL1);\
    /*gpio1[4]�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL19);\
\
\
/*����MMC1(6��PIN��*/\
    /*gpio1[5]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL1);\
    add_ios_list(IOS_PD_AF_CTRL2);\
    /*gpio1[5]�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL21);\
\
    /*gpio1[7]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL1);\
    add_ios_list(IOS_PD_AF_CTRL2);\
\
    /*gpio1[8]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL1);\
    add_ios_list(IOS_PD_AF_CTRL2);\
    /*gpio1[8]�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL24);\
\
    /*gpio1[9]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL1);\
    add_ios_list(IOS_PD_AF_CTRL2);\
    /*gpio1[9]�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL25);\
\
\
/*����JTAG0��6��PIN��*/\
\
/*����GPIO/GSBI_0(20PIN��*/\
    /*rgmii_txd[0]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL28);\
\
    /*rgmii_txd[1]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL29);\
\
    /*rgmii_txd[2]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL30);\
\
    /*rgmii_txd[3]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL31);\
\
    /*gpio2[4]�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL32);\
\
    /*rgmii_tx_clk�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL33);\
\
    /*rgmii_mdio�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    /*rgmii_mdio�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL34);\
\
    /*rgmii_mdc�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL35);\
\
    /*rgmii_txen�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL36);\
\
    /*rgmii_rxdv�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL37);\
\
    /*zclk�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL40);\
\
    /*zsync�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL41);\
\
    /*zmosi�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL42);\
\
    /*zmiso�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL2);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL43);\
\
    /*gpio2[19]�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL47);\
\
\
/*����GPIO/GSBI_1(7PIN��*/\
    /*rgmii_rxd[0]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL3);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL48);\
\
    /*rgmii_rxd[1]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL3);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL49);\
\
    /*rgmii_rxd[2]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL3);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL50);\
\
    /*rgmii_rxd[3]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL3);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL51);\
\
    /*pcie_clkreq_n�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL3);\
    add_ios_list(IOS_PD_AF_CTRL7);\
    /*pcie_clkreq_n�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL53);\
\
    /*rgmii_rx_clk�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL3);\
    add_ios_list(IOS_PD_AF_CTRL4);\
    add_ios_list(IOS_PD_IOM_CTRL54);\
\
\
/*����RF�߿أ�CH0 FEM(7PIN��*/\
    /*ch0_apt_pdm�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL4);\
    add_ios_list(IOS_PD_AF_CTRL8);\
    add_ios_list(IOS_PD_IOM_CTRL58);\
    /*ch0_apt_pdm�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL58);\
\
    /*ch0_mipi_clk�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL4);\
    add_ios_list(IOS_PD_AF_CTRL8);\
    add_ios_list(IOS_PD_IOM_CTRL59);\
    /*ch0_mipi_clk�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL59);\
\
    /*ch0_mipi_data�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL4);\
    add_ios_list(IOS_PD_AF_CTRL8);\
\
\
/*����RF�߿أ�FEM(17��PIN��*/\
    /*gpio1[15]�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL62);\
\
    /*gpio1[16]�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL63);\
\
    /*antpa_sel[2]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL64);\
\
    /*antpa_sel[3]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL65);\
\
    /*antpa_sel[4]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL66);\
\
    /*antpa_sel[5]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL67);\
    /*antpa_sel[5]�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL67);\
\
    /*antpa_sel[6]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL68);\
\
    /*antpa_sel[7]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL69);\
\
    /*antpa_sel[9]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL71);\
\
    /*antpa_sel[10]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL72);\
\
    /*antpa_sel[11]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL73);\
\
    /*antpa_sel[12]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL74);\
    /*antpa_sel[12]�ܽ����������ñ���*/\
    add_ios_list(IOS_PD_IOM_CTRL74);\
\
    /*antpa_sel[13]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL75);\
\
    /*antpa_sel[14]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL76);\
\
    /*antpa_sel[16]�ܽŸ������ñ���*/\
    add_ios_list(IOS_PD_MF_CTRL5);\
    add_ios_list(IOS_PD_AF_CTRL10);\
    add_ios_list(IOS_PD_IOM_CTRL78);\
\
\
/*����ABB�źţ�18��PIN��*/\
\
/*����LPDDR2�źţ�61��PIN�� CA�źţ�16��PIN��*/\
\
/*����LPDDR2�źţ�61��PIN�� DQ�źţ�45��PIN��*/\
\
/*����LPDDR2 KGD�źţ�1��PIN��*/\
\
/*����PCIE��7��PIN��*/\
\
/*����PCIE ���ʱ�ӣ�2��PIN��*/\
\
/*����HSIC��2��PIN��*/\
\
}while(0)

/*�ܽ����ú�for drv*/
#define  CPE_IOS_CONFIG_SAVE \
do{\
     CPE_IOS_AO_CONFIG_SAVE;\
     CPE_IOS_PD_CONFIG_SAVE;\
}while(0)

#endif

