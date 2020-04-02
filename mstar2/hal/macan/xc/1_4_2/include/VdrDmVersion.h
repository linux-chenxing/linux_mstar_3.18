/****************************************************************************
* This product contains one or more programs protected under international
* and U.S. copyright laws as unpublished works.  They are confidential and
* proprietary to Dolby Laboratories.  Their reproduction or disclosure, in
* whole or in part, or the production of derivative works therefrom without
* the express permission of Dolby Laboratories is prohibited.
*
*             Copyright 2011 - 2015 by Dolby Laboratories.
*                     All rights reserved.
****************************************************************************/
#ifndef VDRDMVERSION_H
#define VDRDMVERSION_H

/*! @name version information

This is the API for discovering the version number of the entire DM library.

There are 3 levels of versioning; reflecting API changes, functionality changes
and the maintenance releases as three numbers. The maintenance version shall
be reset for every increase in versions of the API and functionality. The
functional version shall never be reset.
Since the functional version will always be incremented for a functional
change, also including an API change, the API version number is redundant but
there for convenience.
*/
/* @{ */

#define DM_VERSION_ALG_MAJOR      3
#define DM_VERSION_ALG_MINOR      1
#define DM_VERSION_MTNC           0
#define DM_VERSION_BUILD          110  /* This is contiguous build number regardless version number */

#define DM_VERSION_BRANCH   "dev"                      /**<@brief This indicates whether the code
                                                                   is on main, dev or rel */

#define DM_VERSION_FLAVOR   " (C version)"

#ifdef PERFORCE_CHANGELIST
#define DM_VERSION_MOD      PERFORCE_CHANGELIST
#else
#define DM_VERSION_MOD      "snapshot "__DATE__" "__TIME__
#endif

#define DM_VERSION_EXT      DM_VERSION_BRANCH"-"DM_VERSION_MOD DM_VERSION_FLAVOR


typedef struct STRUCT_DLB_VERSION_INFO {
  int vApi;
  int vFct;
  int vMtnc;
  int vBuild;
  const char* text;
} DLB_VERSION_INFO;


#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

static const DLB_VERSION_INFO * dm_get_version(void)
{
    static const DLB_VERSION_INFO v = {
      DM_VERSION_ALG_MAJOR,
      DM_VERSION_ALG_MINOR,
        DM_VERSION_MTNC,
        DM_VERSION_BUILD,
        DM_VERSION_EXT
    };
    return &v;
}
/* @} */

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif /* VDRDMVERSION_H */
