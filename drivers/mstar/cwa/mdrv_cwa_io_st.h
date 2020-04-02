#ifndef _MDR_CWA_IO_ST_H_H
#define _MDR_CWA_IO_ST_H_H

#ifndef PC_SIM
#include <asm/types.h>
#include <linux/kernel.h>
#else
#include "typedef.h"
#endif //end PC_SIM

#define CWA_DRIVER_VERSION            "1.1.0"

//---------------------------------------------------------------------------
// Structure and enum.
//---------------------------------------------------------------------------

/// @brief CWA driver status enumeration
typedef enum
{
    CWA_STATUS_OK               = 0x0000,
    CWA_STATUS_PARAM_ERROR      = 0x1000,
    CWA_STATUS_ADDRESS_NOT_ALIGNED_ERROR = 0x1001,
    CWA_STATUS_BOUND_BOX_CALC_ERROR     = 0x1002,
    CWA_STATUS_MATRIX_VALUES_ERROR      = 0x1003,
    CWA_STATUS_OP_ERROR                 = 0x1004,
    CWA_STATUS_OUT_OF_MEMORY            = 0x1005,
    CWA_STATUS_HW_UNAVAILABLE           = 0x1006,
    CWA_STATUS_MEMORY_FAILURE           = 0x1007,
    CWA_STATUS_BUSY                     = 0x1008,
    CWA_STATUS_PROC_CONFIG_ERROR        = 0x1009,
    CWA_STATUS_CLK_ERROR                = 0x100a
} CWA_IOC_ERROR;

/// @brief CWA hardware errors enumeration (bit-mask)
typedef enum
{
    CWA_HW_ERROR_OK = 0,
    CWA_HW_ERROR_AXI = 1
} cwa_hw_error_e;

/// @brief CWA operation modes enumeration
typedef enum
{
    CWA_OPERATION_MODE_PERSPECTIVE,
    CWA_OPERATION_MODE_MAP
} cwa_operation_mode_e;

/// @brief CWA image formats enumeration
typedef enum
{
    CWA_IMAGE_FORMAT_RGBA,
    CWA_IMAGE_FORMAT_NV16,
    CWA_IMAGE_FORMAT_NV12
} cwa_image_format_e;

/// @brief CWA perspective coefficients enumeration
typedef enum
{
    CWA_PERSPECTIVE_C00,
    CWA_PERSPECTIVE_C01,
    CWA_PERSPECTIVE_C02,
    CWA_PERSPECTIVE_C10,
    CWA_PERSPECTIVE_C11,
    CWA_PERSPECTIVE_C12,
    CWA_PERSPECTIVE_C20,
    CWA_PERSPECTIVE_C21,
    CWA_PERSPECTIVE_C22,
    CWA_PERSPECTIVE_NUM_COEFFS
} cwa_perspective_coeffs_e;

/// @brief CWA displacement map resolution enumeration
typedef enum
{
    CWA_MAP_8X8,
    CWA_MAP_16X16
} cwa_map_resolution_e;

/// @brief CWA displacement map format enumeration
typedef enum
{
    CWA_MAP_FORMAT_ABSOLUTE,
    CWA_MAP_FORMAT_RELATIVE
} cwa_map_format_e;

/// @brief Number of fraction bits in the displacement table entries
#define CWA_MAP_PRECISION_BITS 3

/// @brief CWA displacement map entry for absolute coordinates format
typedef struct
{
    int  y;
    int  x;
} cwa_table_absolute_entry_t;

/// @brief CWA displacement map entry for coordinates' relative offset format
typedef struct
{
    short  y;
    short  x;
} cwa_table_relative_entry_t;

/// @brief CWA displacement table descriptor
typedef struct
{
    cwa_map_resolution_e resolution;   //< map resolution
    cwa_map_format_e     format;       //< map format (absolute, relative)
    //unsigned int             num_entries;  //< number of (Y,X) entries in the table
    //unsigned int           stride;       //< line stride in bytes
    union
    {
        const cwa_table_absolute_entry_t* p_abs_tab;
        const cwa_table_relative_entry_t* p_rel_tab;
        unsigned int* overlay;
    } tab;  //< pointer to table data (overlay for different pointer types)
} cwa_displacement_table_t;

typedef struct
{
    short height;                           // input tile height
    short width;                            // input tile width
    short y;                                // input tile top left y coordinate
    short x;                                // input tile top left x coordinate
} bound_box_st;

typedef struct
{
    int bb_num;             // number of bound boxes
    const bound_box_st *p_bb_tab;       // list of all bound boxes
}cwa_bound_box_table_t;

/// @brief CWA image plane index enumeration
typedef enum
{
    CWA_IMAGE_PLANE_RGBA = 0,
    CWA_IMAGE_PLANE_Y = 0,
    CWA_IMAGE_PLANE_UV = 1
} cwa_image_plane_id_e;

/// @brief CWA image descriptor
typedef struct
{
    cwa_image_format_e format;  //< image format
    unsigned int width;             //< image width  (for YUV - Y plane width)
    unsigned int height;            //< image height (for YUV - Y plane width)
} cwa_image_desc_t;

/// @brief CWA image data structure
typedef struct
{
    unsigned int num_planes;        //< number of image planes
    unsigned char* p_data[2];         //< pointers to the image planes' data
} cwa_image_data_t;

/// @brief CWA hardware configuration structure
typedef struct
{
    cwa_operation_mode_e      op_mode;         //< Operation mode
    cwa_displacement_table_t  disp_table;      //< Displacement table descriptor
    cwa_bound_box_table_t bb_table;     //< Bounding box table descriptor

    int coeff[CWA_PERSPECTIVE_NUM_COEFFS]; //< Perspective transform coefficients

    cwa_image_desc_t          input_image;     //< Input image
    cwa_image_data_t          input_data;
    cwa_image_desc_t          output_image;    //< Output image
    cwa_image_data_t          output_data;

    unsigned int                  afbc_en;         //< Enable AFBC mode
    unsigned char                   fill_val[2];     //< Fill value for each image plane

    unsigned int                  axi_max_read_burst_size;   //< Maximum read burst size of the AXI master port
    unsigned int                  axi_max_write_burst_size;  //< Maximum write burst size of the AXI master port
    unsigned int                  axi_max_read_outstanding;  //< Maximum outstanding read accesses of the AXI master port
    unsigned int                  axi_max_write_outstanding; //< Maximum outstanding write accesses of the AXI master port

    // Debug features
    unsigned int                  debug_bypass_displacement_en;
    unsigned int                  debug_bypass_interp_en;

    // driver's private structures
    //cwa_alloc_cb_t            _alloc_cb;
    //void*                     _p_bounding_box;           // aligned address for the HW
    //void*                     _p_allocated_bounding_box; // real allocated address to be used by the free function
    //unsigned int                  _bounding_box_calc_done;   // flag to indicate that bounding boxes have been calculated
    unsigned int                  _output_tiles_width;       // calculated internally based on the bounding boxes
    unsigned int                  _output_tiles_height;      // calculated internally based on the bounding boxes
    unsigned int                  _num_output_tiles;         // calculated internally based on the bounding boxes
} cwa_ioc_config;

#endif //_MDR_CWA_IO_ST_H_H
