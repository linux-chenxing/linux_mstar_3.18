#ifndef _MDRV_VERCHK_H
#define _MDRV_VERCHK_H


//----------------------------------------------------------------------------------
//        define & Macro
//----------------------------------------------------------------------------------
#define VERCHK_HEADER       0x4D530000
#define VERCHK_HADER_MASK   0xFFFF0000
#define VERCHK_VERSION_MASK 0x0000FFFF
#define VERCHK_MAJORVERSION_MASK 0x0000FF00

#define FILL_VERCHK_TYPE(var, var_ver, var_size, version)        \
({                                                               \
    var_ver = (VERCHK_HEADER | (version & VERCHK_VERSION_MASK)); \
    var_size = sizeof(var);                                      \
    var;                                                         \
})


#define CHK_VERCHK_HEADER(pvar)                                \
({															   \
    u32 *p = (u32 *)pvar;                                      \
    bool  bH;                                                  \
    if((*p & VERCHK_HADER_MASK) == VERCHK_HEADER)              \
        bH = 1;                                                \
    else                                                       \
        bH = 0;                                                \
    bH;                                                        \
})


#define CHK_VERCHK_VERSION_EQU(pvar, v)                        \
({                                                             \
    u32 *p = (u32 *)pvar;                                      \
    bool bV;                                                   \
    if(((*p & VERCHK_VERSION_MASK) == v)                       \
        bV = 1;                                                \
    else                                                       \
        bV = 0;                                                \
    bV;                                                        \
})

#define CHK_VERCHK_VERSION_LESS(pvar, v)                       \
({                                                             \
    u32 *p = (u32 *)pvar;                                      \
    bool bV;                                                   \
    if((*p & VERCHK_VERSION_MASK) < v)                         \
        bV = 1;                                                \
    else                                                       \
        bV = 0;                                                \
    bV;                                                        \
})

#define CHK_VERCHK_MAJORVERSION_LESS(pvar, v)                  \
({                                                             \
    u32 *p = (u32 *)pvar;                                      \
    bool bV;                                                   \
    if((*p & VERCHK_MAJORVERSION_MASK) < (v& VERCHK_MAJORVERSION_MASK)) \
        bV = 1;                                                         \
    else                                                                \
        bV = 0;                                                         \
    bV;                                                                 \
})

#define CHK_VERCHK_VERSION_GREATER(pvar, v)                    \
({                                                             \
    u32 *p = (u32 *)pvar;                                      \
    bool bV;                                                   \
    if((*p & VERCHK_VERSION_MASK) > v)                         \
        bV = 1;                                                \
    else                                                       \
        bV = 0;                                                \
    bV;                                                        \
})


#define CHK_VERCHK_VERSION_LESS_EQU(pvar, v)                   \
({                                                             \
    u32 *p = (u32 *)pvar;                                      \
    bool bV;                                                   \
    if((*p & VERCHK_VERSION_MASK) <= v)                        \
        bV = 1;                                                \
    else                                                       \
        bV = 0;                                                \
    bV;                                                        \
})

#define CHK_VERCHK_VERSION_GREATER_EQU(pvar, v)                \
({                                                             \
    u32 *p = (u32 *)pvar;                                      \
    bool bV;                                                   \
    if((*p & VERCHK_VERSION_MASK) >= v)                        \
        bV = 1;                                                \
    else                                                       \
        bV = 0;                                                \
    bV;                                                        \
})


#define CHK_VERCHK_SIZE(pvar,s)                                \
({                                                             \
    u32 *p = (u32 *)pvar;                                      \
    bool bS;                                                   \
    if(*p == s )                                               \
        bS = 1;                                                \
    else                                                       \
        bS = 0;                                                \
    bS;                                                        \
})


#define VERCHK_ERR(_fmt, _args...)                 \
    do                                             \
    {                                              \
        sclprintf(_fmt, ## _args);                \
    }while(0);

#endif
