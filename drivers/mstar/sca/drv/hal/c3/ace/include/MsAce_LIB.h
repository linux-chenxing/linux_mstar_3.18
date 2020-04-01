#ifndef __MSACE_LIB_H
#define __MSACE_LIB_H

// ATV Chips
#define CHIP_PAULO      100
#define CHIP_PAULO2     101
#define CHIP_LOLA       102
#define CHIP_LOMEO      103
#define CHIP_LATTE      104
#define CHIP_LASER      105
#define CHIP_LOPEZ      106
#define CHIP_RAPHAEL    107
#define CHIP_MUSE       108
#define CHIP_MONA       109
#define CHIP_METIS      110

// DTV Chips
#define CHIP_NEPTUNE    302
#define CHIP_PLUTO      303
#define CHIP_TITANIA1   304
#define CHIP_TITANIA2   305
#define CHIP_TITANIA7   306

// Monitor Chips
#define CHIP_OMEGA      502

// Others
#define CHIP_CERAMAL    702
#define CHIP_URSA       703

#define CHIP_TYPE       CHIP_TITANIA2

//////////////////////////////////////////////////////////////////////////

#define USE_NEW_ACE_MATRIX_RULE

#define ACE_CHIP_GROUP_PAULO    0
#define ACE_CHIP_GROUP_LOPEZ    1
#define ACE_CHIP_GROUP_DTV1     2
#define ACE_CHIP_GROUP_CERAMAL  3
#define ACE_CHIP_GROUP_MUSE     4
#define ACE_CHIP_GROUP_URSA     5
#define ACE_CHIP_GROUP_T7       6

#if( CHIP_TYPE == CHIP_PAULO ||CHIP_TYPE == CHIP_PAULO2\
  || CHIP_TYPE == CHIP_LOLA  || CHIP_TYPE == CHIP_LOMEO\
  || CHIP_TYPE == CHIP_LATTE || CHIP_TYPE == CHIP_LASER\
   )
    #define ACE_CHIP_GROUP      ACE_CHIP_GROUP_PAULO
#elif( CHIP_TYPE == CHIP_LOPEZ || CHIP_TYPE == CHIP_RAPHAEL || CHIP_TYPE == CHIP_OMEGA || CHIP_TYPE == CHIP_METIS )
    #define ACE_CHIP_GROUP      ACE_CHIP_GROUP_LOPEZ
#elif (CHIP_TYPE == CHIP_PLUTO || CHIP_TYPE == CHIP_TITANIA1 || CHIP_TYPE == CHIP_TITANIA2)
    #define ACE_CHIP_GROUP      ACE_CHIP_GROUP_DTV1
#elif (CHIP_TYPE == CHIP_CERAMAL )
    #define ACE_CHIP_GROUP      ACE_CHIP_GROUP_CERAMAL
#elif (CHIP_TYPE == CHIP_MUSE)
    #define ACE_CHIP_GROUP      ACE_CHIP_GROUP_MUSE
#elif (CHIP_TYPE == CHIP_URSA)
    #define ACE_CHIP_GROUP      ACE_CHIP_GROUP_URSA    
#elif (CHIP_TYPE == CHIP_TITANIA7)  
     #define ACE_CHIP_GROUP      ACE_CHIP_GROUP_T7    
#else
    #error
#endif

//////////////////////////////////////////////////////////////////////////

#if (ACE_CHIP_GROUP == ACE_CHIP_GROUP_PAULO)
    #include "MsAce_LIB_Group_Paulo.h"
#elif (ACE_CHIP_GROUP == ACE_CHIP_GROUP_LOPEZ)
    #include "MsAce_LIB_Group_Lopez.h"
#elif (ACE_CHIP_GROUP == ACE_CHIP_GROUP_DTV1)
    #include "MsAce_LIB_Group_DTV1.h"
#elif (ACE_CHIP_GROUP == ACE_CHIP_GROUP_CERAMAL)
    #include "MsAce_LIB_Group_Ceramal.h"
#elif (ACE_CHIP_GROUP == ACE_CHIP_GROUP_MUSE)
    #include "MsAce_LIB_Group_Muse.h"
#elif (ACE_CHIP_GROUP == ACE_CHIP_GROUP_URSA)
    #include "MsAce_LIB_Group_Ursa.h"
#elif (ACE_CHIP_GROUP == ACE_CHIP_GROUP_T7)
    #include "MsAce_LIB_Group_DTVT7.h"   
#else
    #error
#endif

#endif // __MSACE_LIB_H
