#ifndef INFINITY2_REG_H
#define INFINITY2_REG_H

#include <infinity2_reg_isp0.h>
#include <infinity2_reg_isp1.h>
#include <infinity2_reg_isp2.h>
#include <infinity2_reg_isp3.h>
#include <infinity2_reg_isp4.h>
#include <infinity2_reg_isp5.h>
#include <infinity2_reg_isp6.h>
#include <infinity2_reg_isp7.h>
#include <infinity2_reg_isp8.h>
#include <infinity2_reg_isp9.h>
#include <infinity2_reg_isp10.h>
#include <infinity2_reg_isp11.h>
#include <infinity2_reg_isp12.h>
#include <infinity2_reg_isp_dmag.h>
#include <infinity2_reg_isp_miu2sram.h>
#include <infinity2_reg.h>

#define reg_addr(base,field)  (((short*)base)+offset_of_##field)
#define reg_addrB(base,field)  (((char*)base)+field*2+((field%2==0)?(0):(-1)))
#define reg_addrW(base,field)  (((short*)base)+field)
#if 0
#define isp_reg_addr(base,field)  (((short*)base)+offset_of_##field)
#define isp_reg(base,field)  (*(((short*)base)+offset_of_##field))
#define isp_reg_w(base,field,val,mask) ( isp_reg(base,field)=\
                                        ((isp_reg(base,field)&mask) | (val&mask))\
                                        )
#endif
#endif
