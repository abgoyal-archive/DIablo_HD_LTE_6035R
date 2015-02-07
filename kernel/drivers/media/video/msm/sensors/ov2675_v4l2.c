/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *         Modify History For This Module
 * When           Who             What,Where,Why
 * --------------------------------------------------------------------------------------
 * 12/11/22      Zhang Lu       Add driver for viper
 * 12/12/07      Zhang Lu       Mod driver to adjust exposure for snapshot,
 *                              enable ZSL, modify preview&snapshot framerate(30fps/7.5fps),
 *                              enable manual effect
 */

#include "msm_sensor.h"
#include <media/msm_camera.h>
#include "msm.h"
#define SENSOR_NAME "ov2675"
#define PLATFORM_DRIVER_NAME "msm_camera_ov2675"
#define ov2675_obj ov2675_##obj

DEFINE_MUTEX(ov2675_mut);
#ifdef CDBG
#undef CDBG
#endif

//#define OV2675_VERBOSE_DGB

#ifdef OV2675_VERBOSE_DGB
#define CDBG(fmt, args...) printk(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#endif
static struct msm_sensor_ctrl_t ov2675_s_ctrl;

/*************************************************/
static int effect_value = CAMERA_EFFECT_OFF;
//static unsigned int SAT_U = 0x80; /* DEFAULT SATURATION VALUES*/
//static unsigned int SAT_V = 0x80; /* DEFAULT SATURATION VALUES*/

/*************************************************/
static struct msm_camera_i2c_reg_conf ov2675_start_settings[] = {
	{0x3086, 0x00},//sleep off
};

static struct msm_camera_i2c_reg_conf ov2675_stop_settings[] = {
	//{0x3086, 0x0f},//sleep on
};
/***********************************************************************/
static struct msm_camera_i2c_reg_conf ov2675_saturation[][4] = {
	{{0x3301, 0xff}, {0x3391, 0x02}, {0x3394, 0x10},
		{0x3395, 0x10},},	/* SATURATION LEVEL0*/
	{{0x3301, 0xff}, {0x3391, 0x02}, {0x3394, 0x18},
		{0x3395, 0x18},},	/* SATURATION LEVEL1*/
	{{0x3301, 0xff}, {0x3391, 0x02}, {0x3394, 0x20},
		{0x3395, 0x20},},	/* SATURATION LEVEL2*/
	{{0x3301, 0xff}, {0x3391, 0x02}, {0x3394, 0x22},
		{0x3395, 0x22},},	/* SATURATION LEVEL3*/
	{{0x3301, 0xff}, {0x3391, 0x02}, {0x3394, 0x48},
		{0x3395, 0x48},},	/* SATURATION LEVEL4*/
	{{0x3301, 0xff}, {0x3391, 0x02}, {0x3394, 0x40},
		{0x3395, 0x40},},	/* SATURATION LEVEL5*/
	{{0x3301, 0xff}, {0x3391, 0x02}, {0x3394, 0x50},
		{0x3395, 0x50},},	/* SATURATION LEVEL6*/
	{{0x3301, 0xff}, {0x3391, 0x02}, {0x3394, 0x60},
		{0x3395, 0x60},},	/* SATURATION LEVEL7*/
	{{0x3301, 0xff}, {0x3391, 0x02}, {0x3394, 0x68},
		{0x3395, 0x68},},	/* SATURATION LEVEL8*/
};

static struct msm_camera_i2c_conf_array ov2675_saturation_confs[][1] = {
	{{ov2675_saturation[0], ARRAY_SIZE(ov2675_saturation[0]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_saturation[1], ARRAY_SIZE(ov2675_saturation[1]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_saturation[2], ARRAY_SIZE(ov2675_saturation[2]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_saturation[3], ARRAY_SIZE(ov2675_saturation[3]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_saturation[4], ARRAY_SIZE(ov2675_saturation[4]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_saturation[5], ARRAY_SIZE(ov2675_saturation[5]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_saturation[6], ARRAY_SIZE(ov2675_saturation[6]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_saturation[7], ARRAY_SIZE(ov2675_saturation[7]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_saturation[8], ARRAY_SIZE(ov2675_saturation[8]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int ov2675_saturation_enum_map[] = {
	MSM_V4L2_SATURATION_L0,
	MSM_V4L2_SATURATION_L1,
	MSM_V4L2_SATURATION_L2,
	MSM_V4L2_SATURATION_L3,
	MSM_V4L2_SATURATION_L4,
	MSM_V4L2_SATURATION_L5,
	MSM_V4L2_SATURATION_L6,
	MSM_V4L2_SATURATION_L7,
	MSM_V4L2_SATURATION_L8,
};

static struct msm_camera_i2c_enum_conf_array ov2675_saturation_enum_confs = {
	.conf = &ov2675_saturation_confs[0][0],
	.conf_enum = ov2675_saturation_enum_map,
	.num_enum = ARRAY_SIZE(ov2675_saturation_enum_map),
	.num_index = ARRAY_SIZE(ov2675_saturation_confs),
	.num_conf = ARRAY_SIZE(ov2675_saturation_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf ov2675_contrast[][4] = {
	{{0x3391, 0x04}, {0x3390, 0x45}, {0x3398, 0x14}, {0x3399, 0x14}, }, /* CONTRAST L0*/
	{{0x3391, 0x04}, {0x3390, 0x45}, {0x3398, 0x16}, {0x3399, 0x16}, }, /* CONTRAST L1*/
	{{0x3391, 0x04}, {0x3390, 0x45}, {0x3398, 0x18}, {0x3399, 0x18}, }, /* CONTRAST L2*/
	{{0x3391, 0x04}, {0x3390, 0x45}, {0x3398, 0x1c}, {0x3399, 0x1c}, }, /* CONTRAST L3*/
	{{0x3391, 0x04}, {0x3390, 0x45}, {0x3398, 0x20}, {0x3399, 0x20}, }, /* CONTRAST L4*/
	{{0x3391, 0x04}, {0x3390, 0x45}, {0x3398, 0x24}, {0x3399, 0x24}, }, /* CONTRAST L5*/
	{{0x3391, 0x04}, {0x3390, 0x45}, {0x3398, 0x28}, {0x3399, 0x28}, }, /* CONTRAST L6*/
	{{0x3391, 0x04}, {0x3390, 0x45}, {0x3398, 0x2c}, {0x3399, 0x2c}, }, /* CONTRAST L7*/
	{{0x3391, 0x04}, {0x3390, 0x45}, {0x3398, 0x30}, {0x3399, 0x30}, }, /* CONTRAST L8*/
};

static struct msm_camera_i2c_conf_array ov2675_contrast_confs[][1] = {
	{{ov2675_contrast[0], ARRAY_SIZE(ov2675_contrast[0]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_contrast[1], ARRAY_SIZE(ov2675_contrast[1]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_contrast[2], ARRAY_SIZE(ov2675_contrast[2]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_contrast[3], ARRAY_SIZE(ov2675_contrast[3]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_contrast[4], ARRAY_SIZE(ov2675_contrast[4]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_contrast[5], ARRAY_SIZE(ov2675_contrast[5]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_contrast[6], ARRAY_SIZE(ov2675_contrast[6]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_contrast[7], ARRAY_SIZE(ov2675_contrast[7]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_contrast[8], ARRAY_SIZE(ov2675_contrast[8]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};


static int ov2675_contrast_enum_map[] = {
	MSM_V4L2_CONTRAST_L0,
	MSM_V4L2_CONTRAST_L1,
	MSM_V4L2_CONTRAST_L2,
	MSM_V4L2_CONTRAST_L3,
	MSM_V4L2_CONTRAST_L4,
	MSM_V4L2_CONTRAST_L5,
	MSM_V4L2_CONTRAST_L6,
	MSM_V4L2_CONTRAST_L7,
	MSM_V4L2_CONTRAST_L8,
};

static struct msm_camera_i2c_enum_conf_array ov2675_contrast_enum_confs = {
	.conf = &ov2675_contrast_confs[0][0],
	.conf_enum = ov2675_contrast_enum_map,
	.num_enum = ARRAY_SIZE(ov2675_contrast_enum_map),
	.num_index = ARRAY_SIZE(ov2675_contrast_confs),
	.num_conf = ARRAY_SIZE(ov2675_contrast_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};
static struct msm_camera_i2c_reg_conf ov2675_sharpness[][5] = {
	{{0x3306, 0x00}, {0x3376, 0x01},{0x3377, 0x00},{0x3378, 0x10},{0x3379, 0x80},},    /* SHARPNESS LEVEL 0*/
	{{0x3306, 0x00}, {0x3376, 0x02},{0x3377, 0x00},{0x3378, 0x08},{0x3379, 0x80},},    /* SHARPNESS LEVEL 1*/
	{{0x3306, 0x00}, {0x3376, 0x04},{0x3377, 0x00},{0x3378, 0x04},{0x3379, 0x80},},    /* SHARPNESS LEVEL 2*/
	{{0x3306, 0x00}, {0x3376, 0x06},{0x3377, 0x00},{0x3378, 0x04},{0x3379, 0x80},},    /* SHARPNESS LEVEL 3*/
	{{0x3306, 0x00}, {0x3376, 0x08},{0x3377, 0x00},{0x3378, 0x04},{0x3379, 0x80},},    /* SHARPNESS LEVEL 4*/
	{{0x3306, 0x00}, {0x3376, 0x0a},{0x3377, 0x00},{0x3378, 0x04},{0x3379, 0x80},},    /* SHARPNESS LEVEL 5*/
};

static struct msm_camera_i2c_conf_array ov2675_sharpness_confs[][1] = {
	{{ov2675_sharpness[0], ARRAY_SIZE(ov2675_sharpness[0]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_sharpness[1], ARRAY_SIZE(ov2675_sharpness[1]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_sharpness[2], ARRAY_SIZE(ov2675_sharpness[2]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_sharpness[3], ARRAY_SIZE(ov2675_sharpness[3]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_sharpness[4], ARRAY_SIZE(ov2675_sharpness[4]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_sharpness[5], ARRAY_SIZE(ov2675_sharpness[5]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int ov2675_sharpness_enum_map[] = {
	MSM_V4L2_SHARPNESS_L0,
	MSM_V4L2_SHARPNESS_L1,
	MSM_V4L2_SHARPNESS_L2,
	MSM_V4L2_SHARPNESS_L3,
	MSM_V4L2_SHARPNESS_L4,
	MSM_V4L2_SHARPNESS_L5,
};

static struct msm_camera_i2c_enum_conf_array ov2675_sharpness_enum_confs = {
	.conf = &ov2675_sharpness_confs[0][0],
	.conf_enum = ov2675_sharpness_enum_map,
	.num_enum = ARRAY_SIZE(ov2675_sharpness_enum_map),
	.num_index = ARRAY_SIZE(ov2675_sharpness_confs),
	.num_conf = ARRAY_SIZE(ov2675_sharpness_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf ov2675_exposure[][3] = {
	{{0x3018, 0x6a}, {0x3019, 0x5a}, {0x301a, 0xd4},}, /*EXPOSURECOMPENSATIONP2*/
	{{0x3018, 0x78}, {0x3019, 0x68}, {0x301a, 0xa5},}, /*EXPOSURECOMPENSATIONp1*/
	{{0x3018, 0x88}, {0x3019, 0x78}, {0x301a, 0xd4},}, /*EXPOSURECOMPENSATIOND*/
	{{0x3018, 0x98}, {0x3019, 0x88}, {0x301a, 0xd4},}, /*EXPOSURECOMPENSATIONN1*/
	{{0x3018, 0xa8}, {0x3019, 0x98}, {0x301a, 0xe5},}, /*EXPOSURECOMPENSATIONN2*/
};

static struct msm_camera_i2c_conf_array ov2675_exposure_confs[][1] = {
	{{ov2675_exposure[0], ARRAY_SIZE(ov2675_exposure[0]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_exposure[1], ARRAY_SIZE(ov2675_exposure[1]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_exposure[2], ARRAY_SIZE(ov2675_exposure[2]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_exposure[3], ARRAY_SIZE(ov2675_exposure[3]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_exposure[4], ARRAY_SIZE(ov2675_exposure[4]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int ov2675_exposure_enum_map[] = {
	MSM_V4L2_EXPOSURE_N2,
	MSM_V4L2_EXPOSURE_N1,
	MSM_V4L2_EXPOSURE_D,
	MSM_V4L2_EXPOSURE_P1,
	MSM_V4L2_EXPOSURE_P2,
};

static struct msm_camera_i2c_enum_conf_array ov2675_exposure_enum_confs = {
	.conf = &ov2675_exposure_confs[0][0],
	.conf_enum = ov2675_exposure_enum_map,
	.num_enum = ARRAY_SIZE(ov2675_exposure_enum_map),
	.num_index = ARRAY_SIZE(ov2675_exposure_confs),
	.num_conf = ARRAY_SIZE(ov2675_exposure_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf ov2675_iso[][1] = {
	{{0x3015, 0x02},},   /*ISO_AUTO*/
	{{0x3015, 0x02},},   /*ISO_DEBLUR*/
	{{0x3015, 0x00},},   /*ISO_100*/
	{{0x3015, 0x01},},   /*ISO_200*/
	{{0x3015, 0x02},},   /*ISO_400*/
	{{0x3015, 0x03},},   /*ISO_800*/
	{{0x3015, 0x03},},   /*ISO_1600*/
};



static struct msm_camera_i2c_conf_array ov2675_iso_confs[][1] = {
	{{ov2675_iso[0], ARRAY_SIZE(ov2675_iso[0]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_iso[1], ARRAY_SIZE(ov2675_iso[1]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_iso[2], ARRAY_SIZE(ov2675_iso[2]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_iso[3], ARRAY_SIZE(ov2675_iso[3]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_iso[4], ARRAY_SIZE(ov2675_iso[4]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_iso[5], ARRAY_SIZE(ov2675_iso[5]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_iso[6], ARRAY_SIZE(ov2675_iso[6]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int ov2675_iso_enum_map[] = {
	MSM_V4L2_ISO_AUTO ,
	MSM_V4L2_ISO_DEBLUR,
	MSM_V4L2_ISO_100,
	MSM_V4L2_ISO_200,
	MSM_V4L2_ISO_400,
	MSM_V4L2_ISO_800,
	MSM_V4L2_ISO_1600,
};


static struct msm_camera_i2c_enum_conf_array ov2675_iso_enum_confs = {
	.conf = &ov2675_iso_confs[0][0],
	.conf_enum = ov2675_iso_enum_map,
	.num_enum = ARRAY_SIZE(ov2675_iso_enum_map),
	.num_index = ARRAY_SIZE(ov2675_iso_confs),
	.num_conf = ARRAY_SIZE(ov2675_iso_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf ov2675_no_effect[] = {
	{0x3391, 0x00},
	{-1, -1},
	{-1, -1},
};

static struct msm_camera_i2c_conf_array ov2675_no_effect_confs[] = {
	{&ov2675_no_effect[0], ARRAY_SIZE(ov2675_no_effect), 0,
		MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_camera_i2c_reg_conf ov2675_special_effect[][3] = {
	{{0x3391, 0x00},{-1, -1},{-1, -1},},		/*for special effect OFF*/
	{{0x3391, 0x20},{-1, -1},{-1, -1},},		/*for special effect MONO*/
	{{0x3391, 0x40},{-1, -1},{-1, -1},},		/*for special efefct Negative*/
	{{0x3391, 0x00},{-1, -1},{-1, -1},},		/*Solarize is not supported by sensor*/
	{{0x3391, 0x18},{0x3396, 0x40},{0x3397, 0xa6},},	/*for sepia*/
	{{0x3391, 0x00},{-1, -1},{-1, -1},},		/* Posteraize not supported */
	{{0x3391, 0x00},{-1, -1},{-1, -1},},		/* White board not supported*/
	{{0x3391, 0x00},{-1, -1},{-1, -1},},		/*Blackboard not supported*/
	{{0x3391, 0x00},{-1, -1},{-1, -1},},		/*Aqua not supported*/
	{{0x3391, 0x00},{-1, -1},{-1, -1},},		/*Emboss not supported */
	{{0x3391, 0x00},{-1, -1},{-1, -1},},		/*sketch not supported*/
	{{0x3391, 0x00},{-1, -1},{-1, -1},},		/*Neon not supported*/
	{{0x3391, 0x00},{-1, -1},{-1, -1},},		/*MAX value*/
};

static struct msm_camera_i2c_conf_array ov2675_special_effect_confs[][1] = {
	{{ov2675_special_effect[0],  ARRAY_SIZE(ov2675_special_effect[0]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[1],  ARRAY_SIZE(ov2675_special_effect[1]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[2],  ARRAY_SIZE(ov2675_special_effect[2]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[3],  ARRAY_SIZE(ov2675_special_effect[3]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[4],  ARRAY_SIZE(ov2675_special_effect[4]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[5],  ARRAY_SIZE(ov2675_special_effect[5]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[6],  ARRAY_SIZE(ov2675_special_effect[6]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[7],  ARRAY_SIZE(ov2675_special_effect[7]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[8],  ARRAY_SIZE(ov2675_special_effect[8]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[9],  ARRAY_SIZE(ov2675_special_effect[9]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[10], ARRAY_SIZE(ov2675_special_effect[10]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[11], ARRAY_SIZE(ov2675_special_effect[11]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_special_effect[12], ARRAY_SIZE(ov2675_special_effect[12]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int ov2675_special_effect_enum_map[] = {
	MSM_V4L2_EFFECT_OFF,
	MSM_V4L2_EFFECT_MONO,
	MSM_V4L2_EFFECT_NEGATIVE,
	MSM_V4L2_EFFECT_SOLARIZE,
	MSM_V4L2_EFFECT_SEPIA,
	MSM_V4L2_EFFECT_POSTERAIZE,
	MSM_V4L2_EFFECT_WHITEBOARD,
	MSM_V4L2_EFFECT_BLACKBOARD,
	MSM_V4L2_EFFECT_AQUA,
	MSM_V4L2_EFFECT_EMBOSS,
	MSM_V4L2_EFFECT_SKETCH,
	MSM_V4L2_EFFECT_NEON,
	MSM_V4L2_EFFECT_MAX,
};

static struct msm_camera_i2c_enum_conf_array ov2675_special_effect_enum_confs = {
	.conf = &ov2675_special_effect_confs[0][0],
	.conf_enum = ov2675_special_effect_enum_map,
	.num_enum = ARRAY_SIZE(ov2675_special_effect_enum_map),
	.num_index = ARRAY_SIZE(ov2675_special_effect_confs),
	.num_conf = ARRAY_SIZE(ov2675_special_effect_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf ov2675_antibanding[][1] = {
	{{0x3014, 0x00},},	 /*ANTIBANDING 60HZ*/
	{{0x3014, 0x80},},	 /*ANTIBANDING 50HZ*/
	{{0x3014, 0xc0},},	 /* ANTIBANDING AUTO*/
};


static struct msm_camera_i2c_conf_array ov2675_antibanding_confs[][1] = {
	{{ov2675_antibanding[0], ARRAY_SIZE(ov2675_antibanding[0]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_antibanding[1], ARRAY_SIZE(ov2675_antibanding[1]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_antibanding[2], ARRAY_SIZE(ov2675_antibanding[2]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int ov2675_antibanding_enum_map[] = {
	MSM_V4L2_POWER_LINE_60HZ,
	MSM_V4L2_POWER_LINE_50HZ,
	MSM_V4L2_POWER_LINE_AUTO,
};


static struct msm_camera_i2c_enum_conf_array ov2675_antibanding_enum_confs = {
	.conf = &ov2675_antibanding_confs[0][0],
	.conf_enum = ov2675_antibanding_enum_map,
	.num_enum = ARRAY_SIZE(ov2675_antibanding_enum_map),
	.num_index = ARRAY_SIZE(ov2675_antibanding_confs),
	.num_conf = ARRAY_SIZE(ov2675_antibanding_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf ov2675_wb_oem[][4] = {
	{{-1, -1}, {-1, -1}, {-1, -1},
		{-1, -1},},/*WHITEBALNACE OFF*/
	{{0x3306, 0x00}, {-1, -1}, {-1, -1},
		{-1, -1},}, /*WHITEBALNACE AUTO*/
	{{0x3306, 0x02}, {0x3337, 0x44}, {0x3338, 0x40},
		{0x3339, 0x70},},	/*WHITEBALNACE CUSTOM*/
	{{0x3306, 0x02}, {0x3337, 0x52}, {0x3338, 0x40},
		{0x3339, 0x58},},	/*INCANDISCENT*/
	{{0x3306, 0x02}, {0x3337, 0x5e}, {0x3338, 0x50},
		{0x3339, 0x56},}, /*FLOURESECT NOT SUPPORTED */
	{{0x3306, 0x02}, {0x3337, 0x5e}, {0x3338, 0x40},
		{0x3339, 0x46},},	/*DAYLIGHT*/
	{{0x3306, 0x02}, {0x3337, 0x65}, {0x3338, 0x4a},
		{0x3339, 0x46},},	/*CLOUDY*/
};

static struct msm_camera_i2c_conf_array ov2675_wb_oem_confs[][1] = {
	{{ov2675_wb_oem[0], ARRAY_SIZE(ov2675_wb_oem[0]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_wb_oem[1], ARRAY_SIZE(ov2675_wb_oem[1]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_wb_oem[2], ARRAY_SIZE(ov2675_wb_oem[2]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_wb_oem[3], ARRAY_SIZE(ov2675_wb_oem[3]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_wb_oem[4], ARRAY_SIZE(ov2675_wb_oem[4]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_wb_oem[5], ARRAY_SIZE(ov2675_wb_oem[5]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{ov2675_wb_oem[6], ARRAY_SIZE(ov2675_wb_oem[6]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int ov2675_wb_oem_enum_map[] = {
	MSM_V4L2_WB_OFF,
	MSM_V4L2_WB_AUTO ,
	MSM_V4L2_WB_CUSTOM,
	MSM_V4L2_WB_INCANDESCENT,
	MSM_V4L2_WB_FLUORESCENT,
	MSM_V4L2_WB_DAYLIGHT,
	MSM_V4L2_WB_CLOUDY_DAYLIGHT,
};

static struct msm_camera_i2c_enum_conf_array ov2675_wb_oem_enum_confs = {
	.conf = &ov2675_wb_oem_confs[0][0],
	.conf_enum = ov2675_wb_oem_enum_map,
	.num_enum = ARRAY_SIZE(ov2675_wb_oem_enum_map),
	.num_index = ARRAY_SIZE(ov2675_wb_oem_confs),
	.num_conf = ARRAY_SIZE(ov2675_wb_oem_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

int ov2675_saturation_msm_sensor_s_ctrl_by_enum(
								struct msm_sensor_ctrl_t *s_ctrl,
								struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	CDBG("--CAMERA-- %s effect_value = %d \n", __func__,effect_value);
	if (effect_value == CAMERA_EFFECT_OFF) {
		rc = msm_sensor_write_enum_conf_array(
						s_ctrl->sensor_i2c_client,
						ctrl_info->enum_cfg_settings, value);
	}
	//	if (value <= MSM_V4L2_SATURATION_L8)
	//		SAT_U = SAT_V = value * 0x10;
	CDBG("--CAMERA-- %s ...(End)\n", __func__);
	return rc;
}
int ov2675_contrast_msm_sensor_s_ctrl_by_enum(
								struct msm_sensor_ctrl_t *s_ctrl,
								struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	CDBG("--CAMERA-- %s effect_value = %d \n", __func__,effect_value);
	if (effect_value == CAMERA_EFFECT_OFF) {
		rc = msm_sensor_write_enum_conf_array(
						s_ctrl->sensor_i2c_client,
						ctrl_info->enum_cfg_settings, value);
	}
	return rc;
}
int ov2675_sharpness_msm_sensor_s_ctrl_by_enum(
								struct msm_sensor_ctrl_t *s_ctrl,
								struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	CDBG("--CAMERA-- %s effect_value = %d \n", __func__,effect_value);
	if (effect_value == CAMERA_EFFECT_OFF) {
		rc = msm_sensor_write_enum_conf_array(
						s_ctrl->sensor_i2c_client,
						ctrl_info->enum_cfg_settings, value);
	}
	return rc;
}

int ov2675_effect_msm_sensor_s_ctrl_by_enum(struct msm_sensor_ctrl_t *s_ctrl,
								struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	effect_value = value;
	CDBG("--CAMERA-- %s effect_value = %d \n", __func__,effect_value);
	if (effect_value == CAMERA_EFFECT_OFF) {
		rc = msm_sensor_write_conf_array(
						s_ctrl->sensor_i2c_client,
						s_ctrl->msm_sensor_reg->no_effect_settings, 0);
		if (rc < 0) {
			CDBG("write faield\n");
			return rc;
		}
		//msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0xda, SAT_U,
		//	MSM_CAMERA_I2C_BYTE_DATA);
		//msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0xdb, SAT_V,
		//	MSM_CAMERA_I2C_BYTE_DATA);
	} else {

		rc = msm_sensor_write_enum_conf_array(
						s_ctrl->sensor_i2c_client,
						ctrl_info->enum_cfg_settings, value);
	}

	return rc;
}

int ov2675_antibanding_msm_sensor_s_ctrl_by_enum(
		struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	CDBG("--CAMERA-- %s effect_value = %d \n", __func__,effect_value);
	return rc;
}

int ov2675_msm_sensor_s_ctrl_by_enum(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	CDBG("--CAMERA-- %s effect_value = %d \n", __func__,effect_value);
	rc = msm_sensor_write_enum_conf_array(
					s_ctrl->sensor_i2c_client,
					ctrl_info->enum_cfg_settings, value);
	if (rc < 0) {
		CDBG("write faield\n");
		return rc;
	}
	return rc;
}
struct msm_sensor_v4l2_ctrl_info_t ov2675_v4l2_ctrl_info[] = {
	{
		.ctrl_id = V4L2_CID_SATURATION,
		.min = MSM_V4L2_SATURATION_L0,
		.max = MSM_V4L2_SATURATION_L8,
		.step = 1,
		.enum_cfg_settings = &ov2675_saturation_enum_confs,
		.s_v4l2_ctrl = ov2675_saturation_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_CONTRAST,
		.min = MSM_V4L2_CONTRAST_L0,
		.max = MSM_V4L2_CONTRAST_L8,
		.step = 1,
		.enum_cfg_settings = &ov2675_contrast_enum_confs,
		.s_v4l2_ctrl = ov2675_contrast_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_SHARPNESS,
		.min = MSM_V4L2_SHARPNESS_L0,
		.max = MSM_V4L2_SHARPNESS_L5,
		.step = 1,
		.enum_cfg_settings = &ov2675_sharpness_enum_confs,
		.s_v4l2_ctrl = ov2675_sharpness_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_EXPOSURE,
		.min = MSM_V4L2_EXPOSURE_N2,
		.max = MSM_V4L2_EXPOSURE_P2,
		.step = 1,
		.enum_cfg_settings = &ov2675_exposure_enum_confs,
		.s_v4l2_ctrl = ov2675_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = MSM_V4L2_PID_ISO,
		.min = MSM_V4L2_ISO_AUTO,
		.max = MSM_V4L2_ISO_1600,
		.step = 1,
		.enum_cfg_settings = &ov2675_iso_enum_confs,
		.s_v4l2_ctrl = ov2675_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_SPECIAL_EFFECT,
		.min = MSM_V4L2_EFFECT_OFF,
		.max = MSM_V4L2_EFFECT_NEGATIVE,
		.step = 1,
		.enum_cfg_settings = &ov2675_special_effect_enum_confs,
		.s_v4l2_ctrl = ov2675_effect_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_POWER_LINE_FREQUENCY,
		.min = MSM_V4L2_POWER_LINE_60HZ,
		.max = MSM_V4L2_POWER_LINE_AUTO,
		.step = 1,
		.enum_cfg_settings = &ov2675_antibanding_enum_confs,
		.s_v4l2_ctrl = ov2675_antibanding_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_WHITE_BALANCE_TEMPERATURE,
		.min = MSM_V4L2_WB_AUTO,
		.max = MSM_V4L2_WB_CLOUDY_DAYLIGHT,
		.step = 1,
		.enum_cfg_settings = &ov2675_wb_oem_enum_confs,
		.s_v4l2_ctrl = ov2675_msm_sensor_s_ctrl_by_enum,
	},
};
static struct msm_camera_i2c_reg_conf ov2675_prev_settings[] = {
	{0x3086, 0x0f},
	{0x3013, 0xf7},
	{0x3373, 0x50},
	{0x3376, 0x08},
	{0x3015, 0x32},
	{0x3014, 0x84},
	{0x3012, 0x10},
	{0x3016, 0x82},
	{0x3023, 0x06},
	{0x3026, 0x02},
	{0x3027, 0x5e},
	{0x330c, 0x00},
	{0x3301, 0xff},
	{0x3069, 0x80},
	{0x306f, 0x14},
	{0x3088, 0x03},
	{0x3089, 0x20},
	{0x308a, 0x02},
	{0x308b, 0x58},
	{0x308e, 0x00},
	{0x30a1, 0x41},
	{0x30a3, 0x80},
	{0x30d9, 0x95},
	{0x3302, 0x11},
	{0x3317, 0x25},
	{0x3318, 0x80},
	{0x3319, 0x08},
	{0x331d, 0x38},
	{0x3362, 0x90},
	{0x3302, 0x11},
	{0x3088, 0x02},
	{0x3089, 0x80},
	{0x308a, 0x01},
	{0x308b, 0xe0},
	{0x331a, 0x28},
	{0x331b, 0x1e},
	{0x331c, 0x00},
	{0x3302, 0x11},
	{0x363b, 0x01},
	{0x309e, 0x08},
	{0x3606, 0x00},
	{0x3630, 0x31},
	{0x304e, 0x04},
	{0x363b, 0x01},
	{0x309e, 0x08},
	{0x3606, 0x00},
	{0x3084, 0x01},
	{0x3634, 0x26},
	//banding
	{0x3011, 0x00},
	{0x300e, 0x34},//34 30fps//3a 15fps
	{0x3010, 0x81},
	{0x302a, 0x02},
	{0x302b, 0x6a},
	{0x3071, 0x00},
	{0x3070, 0x5d},
	{0x3073, 0x00},
	{0x3072, 0x4d},
	{0x301c, 0x05},
	{0x301d, 0x07},
	{0x3640, 0x30},
	{0x3645, 0xf0},
	{0x3306, 0x00},//auto awb


};
static struct msm_camera_i2c_reg_conf ov2675_snap_settings[] = {
	{0x3086, 0x0f},
	{0x3011, 0x01},
	{0x3373, 0x40},
	{0x3376, 0x05},
	{0x3010, 0x81},
	{0x3012, 0x00},
	{0x302a, 0x04},
	{0x302b, 0xD4},
	{0x306f, 0x54},
	{0x3362, 0x80},
	{0x3070, 0x5d},
	{0x3072, 0x5d},
	{0x301c, 0x0f},
	{0x301d, 0x0f},
	{0x3020, 0x01},
	{0x3021, 0x18},
	{0x3022, 0x00},
	{0x3023, 0x0A},
	{0x3024, 0x06},
	{0x3025, 0x58},
	{0x3026, 0x04},
	{0x3027, 0xbc},
	{0x3088, 0x06},
	{0x3089, 0x40},
	{0x308A, 0x04},
	{0x308B, 0xB0},
	{0x3316, 0x64},
	{0x3317, 0x4B},
	{0x3318, 0x00},
	{0x3319, 0x2C},
	{0x331A, 0x64},
	{0x331B, 0x4B},
	{0x331C, 0x00},
	{0x331D, 0x4C},
	{0x3302, 0x01},
	{0x300e, 0x28},
	{0x3640, 0x10},
	{0x3645, 0xc0},
	{0x309e, 0x08},
	{0x3606, 0x00},
	{0x3084, 0x01},
	{0x3634, 0x26},


};
static struct msm_camera_i2c_reg_conf ov2675_zsl_settings[] = {
	//snapshot
	{0x3086, 0x0f},
	{0x3011, 0x01},
	{0x3373, 0x40},
	{0x3376, 0x05},
	{0x3010, 0x81},
	{0x3012, 0x00},
	{0x302a, 0x04},
	{0x302b, 0xD4},
	{0x306f, 0x54},
	{0x3362, 0x80},
	{0x3070, 0x5d},
	{0x3072, 0x5d},
	{0x301c, 0x0f},
	{0x301d, 0x0f},
	{0x3020, 0x01},
	{0x3021, 0x18},
	{0x3022, 0x00},
	{0x3023, 0x0A},
	{0x3024, 0x06},
	{0x3025, 0x58},
	{0x3026, 0x04},
	{0x3027, 0xbc},
	{0x3088, 0x06},
	{0x3089, 0x40},
	{0x308A, 0x04},
	{0x308B, 0xB0},
	{0x3316, 0x64},
	{0x3317, 0x4B},
	{0x3318, 0x00},
	{0x3319, 0x2C},
	{0x331A, 0x64},
	{0x331B, 0x4B},
	{0x331C, 0x00},
	{0x331D, 0x4C},
	{0x3302, 0x01},
	{0x300e, 0x28},
	{0x3640, 0x10},
	{0x3645, 0xc0},
	{0x309e, 0x08},
	{0x3606, 0x00},
	{0x3084, 0x01},
	{0x3634, 0x26},

};

static struct msm_camera_i2c_reg_conf ov2675_recommend_settings[] = {
	{0x3086, 0x0f},
	{0x308c, 0x80},
	{0x308d, 0x0e},
	{0x360b, 0x00},
	{0x30b0, 0xff},
	{0x30b1, 0xff},
	{0x30b2, 0x04},
	{0x300e, 0x34},
	{0x300f, 0xa6},
	{0x3010, 0x81},
	{0x3082, 0x01},
	{0x30f4, 0x01},
	{0x3090, 0x43},
	{0x3091, 0xc0},
	{0x30ac, 0x42},
	{0x30d1, 0x08},
	{0x30a8, 0x54},
	{0x3015, 0x04},
	{0x3093, 0x00},
	{0x307e, 0xe5},
	{0x3079, 0x00},
	{0x30aa, 0x52},
	{0x3017, 0x40},
	{0x30f3, 0x83},
	{0x306a, 0x0c},
	{0x306d, 0x00},
	{0x336a, 0x3c},
	{0x3076, 0x6a},
	{0x30d9, 0x95},
	{0x3016, 0x52},
	{0x3601, 0x30},
	{0x304e, 0x88},
	{0x30f1, 0x82},
	{0x306f, 0x14},
	{0x302a, 0x02},
	{0x302b, 0x6a},
	{0x3012, 0x10},
	{0x3011, 0x01},
	{0x30af, 0x00},
	{0x3048, 0x1f},
	{0x3049, 0x4e},
	{0x304a, 0x20},
	{0x304f, 0x20},
	{0x304b, 0x02},
	{0x304c, 0x00},
	{0x304d, 0x02},
	{0x304f, 0x20},
	{0x30a3, 0x10},
	{0x3013, 0xe7},
	{0x3014, 0x84},
	{0x302c, 0x00},	
	{0x302d, 0x00},
	{0x302e, 0x00},
	{0x3071, 0x00},
	{0x3070, 0x5d},
	{0x3073, 0x00},
	{0x3072, 0x4d},
	{0x301c, 0x07},
	{0x301d, 0x08},
	{0x304d, 0x42},
	{0x304a, 0x40},
	{0x304f, 0x40},
	{0x3095, 0x07},
	{0x3096, 0x16},
	{0x3097, 0x1d},
	{0x3020, 0x01},
	{0x3021, 0x18},
	{0x3022, 0x00},
	{0x3023, 0x06},
	{0x3024, 0x06},
	{0x3025, 0x58},
	{0x3026, 0x02},
	{0x3027, 0x61},
	{0x3088, 0x02},
	{0x3089, 0x80},
	{0x308a, 0x01},
	{0x308b, 0xe0},
	{0x3316, 0x64},
	{0x3317, 0x25},
	{0x3318, 0x80},
	{0x3319, 0x08},
	{0x331a, 0x28},
	{0x331b, 0x1e},
	{0x331c, 0x00},
	{0x331d, 0x38},
	{0x3100, 0x00},
	{0x3320, 0xfa},
	{0x3321, 0x11},
	{0x3322, 0x92},
	{0x3323, 0x01},
	{0x3324, 0x97},
	{0x3325, 0x02},
	{0x3326, 0xff},
	{0x3327, 0x10},
	{0x3328, 0x10},
	{0x3329, 0x1f},
	{0x332a, 0x56},
	{0x332b, 0x54},
	{0x332c, 0xbe},
	{0x332d, 0xce},
	{0x332e, 0x2e},
	{0x332f, 0x30},
	{0x3330, 0x4d},
	{0x3331, 0x44},
	{0x3332, 0xf0},
	{0x3333, 0x0a},
	{0x3334, 0xf0},
	{0x3335, 0xf0},
	{0x3336, 0xf0},
	{0x3337, 0x40},
	{0x3338, 0x40},
	{0x3339, 0x40},
	{0x333a, 0x00},
	{0x333b, 0x00},
	//color matrix
	{0x3380, 0x28},
	{0x3381, 0x48},
	{0x3382, 0x14},
	{0x3383, 0x17},
	{0x3384, 0x90},
	{0x3385, 0xa7},
	{0x3386, 0xbf},
	{0x3387, 0xb7},
	{0x3388, 0x08},
	{0x3389, 0x98},
	{0x338a, 0x01},
	//gamma
	{0x3340, 0x06},
	{0x3341, 0x0c},
	{0x3342, 0x19},
	{0x3343, 0x34},
	{0x3344, 0x4e},
	{0x3345, 0x5f},
	{0x3346, 0x6d},
	{0x3347, 0x78},
	{0x3348, 0x84},
	{0x3349, 0x95},
	{0x334a, 0xa5},
	{0x334b, 0xb4},
	{0x334c, 0xcc},
	{0x334d, 0xe2},
	{0x334e, 0xf6},
	{0x334f, 0x11},
	{0x3090, 0x03},
	{0x307c, 0x10},
	{0x3350, 0x32},
	{0x3351, 0x28},
	{0x3352, 0x00},
	{0x3353, 0x28},
	{0x3354, 0x00},
	{0x3355, 0x85},
	{0x3356, 0x32},
	{0x3357, 0x28},
	{0x3358, 0x00},
	{0x3359, 0x22},
	{0x335a, 0x00},
	{0x335b, 0x85},
	{0x335c, 0x32},
	{0x335d, 0x28},
	{0x335e, 0x00},
	{0x335f, 0x20},
	{0x3360, 0x00},
	{0x3361, 0x85},
	{0x3363, 0x70},
	{0x3364, 0x7f},
	{0x3365, 0x00},
	{0x3366, 0x00},
	{0x3301, 0xff},
	{0x338B, 0x13},
	{0x338c, 0x10},
	{0x338d, 0x40},
	//sharpness&noise
	{0x3370, 0xd0},
	{0x3371, 0x00},
	{0x3372, 0x00},
	{0x3373, 0x50},
	{0x3374, 0x10},
	{0x3375, 0x10},
	{0x3376, 0x06},
	{0x3377, 0x00},
	{0x3378, 0x04},
	{0x3379, 0x80},
	{0x3069, 0x86},
	{0x3087, 0x02},
	{0x30a8, 0x54},
	{0x30aa, 0x82},
	{0x30a3, 0x91},
	{0x30a1, 0x41},
	{0x3300, 0xfc},
	{0x3302, 0x11},
	{0x3400, 0x00},
	{0x3606, 0x20},
	{0x3601, 0x30},
	{0x300e, 0x34},
	{0x30f3, 0x83},
	{0x304e, 0x88},
	{0x3391, 0x06},
	{0x3394, 0x48},
	{0x3395, 0x48},
	{0x3018, 0x88},
	{0x3019, 0x78},
	{0x301a, 0xd4},
	{0x3640, 0x30},
	{0x3645, 0xf0},

};

static struct v4l2_subdev_info ov2675_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,

	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array ov2675_init_conf[] = {
	{&ov2675_recommend_settings[0],
	ARRAY_SIZE(ov2675_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}
};

static struct msm_camera_i2c_conf_array ov2675_confs[] = {
	{&ov2675_snap_settings[0],
	ARRAY_SIZE(ov2675_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov2675_prev_settings[0],
	ARRAY_SIZE(ov2675_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov2675_zsl_settings[0],
	ARRAY_SIZE(ov2675_zsl_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t ov2675_dimensions[] = {
	{/* For SNAPSHOT */
		.x_output = 0x640,/*1600*/  /*for 2Mp*/
		.y_output = 0x4b0,/*1200*/
		.line_length_pclk = 0x793,
		.frame_length_lines = 0x4D4,
		.vt_pixel_clk = 19172832,
		.op_pixel_clk = 72000000,
		.binning_factor = 0,

	},
	{ /* For PREVIEW */
		.x_output = 0x280, /*640*/
		.y_output = 0x1E0, /*480*/
		.line_length_pclk = 0x793,
		.frame_length_lines = 0x26A,
		.vt_pixel_clk = 35949060,
		.op_pixel_clk = 72000000,
		.binning_factor = 0,
	},
	{/* For ZSL */
		.x_output = 0x640,/*1600*/  /*for 2Mp*/
		.y_output = 0x4b0,/*1200*/
		.line_length_pclk = 0x793,
		.frame_length_lines = 0x4D4,
		.vt_pixel_clk = 19172832,
		.op_pixel_clk = 72000000,
		.binning_factor = 0,
	},
};

static struct msm_sensor_output_reg_addr_t ov2675_reg_addr = {
	.x_output = 0x3088,
	.y_output = 0x308A,
	.line_length_pclk = 0x3028,
	.frame_length_lines = 0x302A,
};

static struct msm_sensor_id_info_t ov2675_id_info = {
	.sensor_id_reg_addr = 0x300A,
	.sensor_id = 0x2656,
};
#if 0
static struct msm_sensor_exp_gain_info_t ov2675_exp_gain_info = {
	.coarse_int_time_addr = 0x3501,
	.global_gain_addr = 0x3508,
	.vert_offset = 6,
};
#endif
static enum msm_camera_vreg_name_t ov2675_veg_seq[] = {
	CAM_VIO,
	CAM_VANA,
};
#if 0
static int32_t ov2675_write_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
		uint16_t gain, uint32_t line)
{
	uint32_t fl_lines, offset;
	uint8_t int_time[3];
	fl_lines =
		(s_ctrl->curr_frame_length_lines * s_ctrl->fps_divider) / Q10;
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;
	if (line > (fl_lines - offset))
		fl_lines = line + offset;

	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines, fl_lines,
		MSM_CAMERA_I2C_WORD_DATA);
	int_time[0] = line >> 12;
	int_time[1] = line >> 4;
	int_time[2] = line << 4;
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr-1,
		int_time[0], MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr,
		int_time[1], MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr+1,
		int_time[2], MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr, gain,
		MSM_CAMERA_I2C_WORD_DATA);
	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	return 0;
}
#endif
static const struct i2c_device_id ov2675_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&ov2675_s_ctrl},
	{ }
};

static struct i2c_driver ov2675_i2c_driver = {
	.id_table = ov2675_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov2675_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};


static const struct of_device_id ov2675_dt_match[] = {
	{.compatible = "qcom,ov2675", .data = &ov2675_s_ctrl},
	{}
};

MODULE_DEVICE_TABLE(of, ov2675_dt_match);

static struct platform_driver ov2675_platform_driver = {
	.driver = {
		.name = "qcom,ov2675",
		.owner = THIS_MODULE,
		.of_match_table = ov2675_dt_match,
	},
};

static int32_t ov2675_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	const struct of_device_id *match;
	match = of_match_device(ov2675_dt_match, &pdev->dev);
	rc = msm_sensor_platform_probe(pdev, match->data);
	return rc;
}

static int __init msm_sensor_init_module(void)
{
	int32_t rc = 0;
	rc = platform_driver_probe(&ov2675_platform_driver,
		ov2675_platform_probe);
	if (!rc)
		return rc;
	return i2c_add_driver(&ov2675_i2c_driver);
}

static void __exit msm_sensor_exit_module(void)
{
	if (ov2675_s_ctrl.pdev) {
		msm_sensor_free_sensor_data(&ov2675_s_ctrl);
		platform_driver_unregister(&ov2675_platform_driver);
	} else
		i2c_del_driver(&ov2675_i2c_driver);
	return;
}

static struct v4l2_subdev_core_ops ov2675_subdev_core_ops = {
/*************************************************/
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
/*************************************************/
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops ov2675_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops ov2675_subdev_ops = {
	.core = &ov2675_subdev_core_ops,
	.video  = &ov2675_subdev_video_ops,
};
int OV2675_get_sysclk(struct msm_sensor_ctrl_t *s_ctrl){
	// calculate sysclk
	unsigned short temp1, temp2;
	unsigned short Indiv2x, Bit8Div, FreqDiv2x, PllDiv, SensorDiv, ScaleDiv, DvpDiv, ClkDiv, VCO, sysclk;

	unsigned short Indiv2x_map[] = {
		 2, 3, 4, 6, 4, 6, 8, 12};

	unsigned short Bit8Div_map[] = {
		 1, 1, 4, 5};

	unsigned short FreqDiv2x_map[] = {
		 2, 3, 4, 6};

	unsigned short DvpDiv_map[] = {
		 1, 2, 8, 16};

	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			 0x300e, &temp1, MSM_CAMERA_I2C_BYTE_DATA);

	// bit[5:0] PllDiv
	PllDiv = 64 - (temp1 & 0x3f);

	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			 0x300f, &temp1, MSM_CAMERA_I2C_BYTE_DATA);

	// bit[2:0] Indiv
	temp2 = temp1 & 0x07;
	Indiv2x = Indiv2x_map[temp2];

	// bit[5:4] Bit8Div
	temp2 = (temp1 >> 4) & 0x03;
	Bit8Div = Bit8Div_map[temp2];

	// bit[7:6] FreqDiv
	temp2 = temp1 >> 6;
	FreqDiv2x = FreqDiv2x_map[temp2];

	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			 0x3010, &temp1, MSM_CAMERA_I2C_BYTE_DATA);

	//bit[3:0] ScaleDiv
	temp2 = temp1 & 0x0f;
	if(temp2==0) {
		ScaleDiv = 1;
	}
	else {
		ScaleDiv = temp2 * 2;
	}

	// bit[4] SensorDiv
	if(temp1 & 0x10) {
		SensorDiv = 2;
	}
	else {
		SensorDiv = 1;
	}

	// bit[5] LaneDiv

	// bit[7:6] DvpDiv
	temp2 = temp1 >> 6;
	DvpDiv = DvpDiv_map[temp2];

	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			 0x3011, &temp1, MSM_CAMERA_I2C_BYTE_DATA);

	// bit[5:0] ClkDiv
	temp2 = temp1 & 0x3f;
	ClkDiv = temp2 + 1;

	VCO = 2400 * Bit8Div * FreqDiv2x * PllDiv / Indiv2x ;//24M clock

	sysclk = VCO / Bit8Div / SensorDiv / ClkDiv / 4;

	return sysclk;
}
int OV2675_get_HTS(struct msm_sensor_ctrl_t *s_ctrl)
{
	unsigned short ret_l = 0, ret_h = 0, temp = 0;
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
		0x3028, &ret_h, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
		0x3029, &ret_l, MSM_CAMERA_I2C_BYTE_DATA);
	temp = (ret_h << 8) | (ret_l & 0xff); //HTS

	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
		0x302c, &ret_l, MSM_CAMERA_I2C_BYTE_DATA);//extra_HTS
	return temp + ret_l;
}
int OV2675_get_VTS(struct msm_sensor_ctrl_t *s_ctrl)
{
	unsigned short ret_l = 0, ret_h = 0, temp = 0, capture_VTS = 0;

	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x302a, &ret_h, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x302b, &ret_l, MSM_CAMERA_I2C_BYTE_DATA);
	temp = (ret_h << 8) + ret_l ;//VTS
	capture_VTS = temp;
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x302d, &ret_h, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x302e, &ret_l, MSM_CAMERA_I2C_BYTE_DATA);
	temp = (ret_h << 8) + ret_l;//extra_VTS

	capture_VTS += temp;
	return capture_VTS;
}
unsigned short preview_sysclk = 0, preview_HTS = 0;
void OV2675_set_bandingfilter(struct msm_sensor_ctrl_t *s_ctrl){
	unsigned short preview_VTS,temp;
	unsigned int band_step60, max_band60, band_step50, max_band50;

	// read preview PCLK
	preview_sysclk = OV2675_get_sysclk(s_ctrl);

	// read preview HTS
	preview_HTS = OV2675_get_HTS(s_ctrl);

	// read preview VTS
	preview_VTS = OV2675_get_VTS(s_ctrl);

	// calculate banding filter
	// 60Hz
	band_step60 = preview_sysclk * 100/preview_HTS * 100/120;
	temp = band_step60 >> 8;
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x3073, temp, MSM_CAMERA_I2C_BYTE_DATA);

	temp = band_step60 & 0xff;
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x3072, temp, MSM_CAMERA_I2C_BYTE_DATA);

	max_band60 = (unsigned int)((preview_VTS-4)/band_step60);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x301d, max_band60-1, MSM_CAMERA_I2C_BYTE_DATA);

	// 50Hz
	band_step50 = preview_sysclk * 100/preview_HTS; 
	temp = band_step50 >> 8;
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x3071, temp, MSM_CAMERA_I2C_BYTE_DATA);
	 temp = band_step50 & 0xff;
	 msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x3070, temp, MSM_CAMERA_I2C_BYTE_DATA);

	 max_band50 = (unsigned int)((preview_VTS-4)/band_step50);
	 msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			 0x301c, max_band50-1, MSM_CAMERA_I2C_BYTE_DATA); 

}


void ov2675_set_shutter(struct msm_sensor_ctrl_t *s_ctrl)
{
	// write shutter, in number of line period
	unsigned short gain16 = 0, binning = 0, capture_HTS = 0, capture_VTS = 0;
	unsigned int shutter = 0, extra_line = 0, capture_sysclk = 0, capture_bandingfilter = 0, capture_gain16_shutter = 0;
	unsigned int capture_max_band = 0;
	unsigned short temp = 0, ret_l = 0, ret_h = 0;

	//awb off & read awb gain
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3306, 0x02, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x330c, 0x02, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x330f, &temp, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3337, temp, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x330c, 0x03, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x330f, &temp, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3338, temp, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x330c, 0x04, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x330f, &temp, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3339, temp, MSM_CAMERA_I2C_BYTE_DATA);

	// read shutter, in number of line period
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x3002, &ret_h, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x3003, &ret_l, MSM_CAMERA_I2C_BYTE_DATA);
	shutter = (ret_h << 8) | (ret_l & 0xff) ;

	ret_l = ret_h = 0;
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x302d, &ret_h, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x302e, &ret_l, MSM_CAMERA_I2C_BYTE_DATA);
	extra_line = (ret_h << 8) | (ret_l & 0xff) ;

	shutter = shutter + extra_line;

	// read gain
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x3000, &temp, MSM_CAMERA_I2C_BYTE_DATA);	
	gain16 = ((temp>>4) + 1) * (16 + (temp & 0x0f));
	//get binning
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x300b, &temp, MSM_CAMERA_I2C_BYTE_DATA);
	if(temp==0x52)
		binning = 2;
	else
		binning = 1;
	// turn off night mode for capture
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x3014, &temp, MSM_CAMERA_I2C_BYTE_DATA);
	temp = temp & 0xf7;
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3014, temp, MSM_CAMERA_I2C_BYTE_DATA);

	// turn off AEC & AGC
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x3013, &temp, MSM_CAMERA_I2C_BYTE_DATA);
	temp = temp & 0xfa;

	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3013, temp, MSM_CAMERA_I2C_BYTE_DATA);

	// read capture sysclk
	capture_sysclk = OV2675_get_sysclk(s_ctrl);
	// read HTS from register settings

	capture_HTS = OV2675_get_HTS(s_ctrl);

	// read VTS from register settings

	capture_VTS = OV2675_get_VTS(s_ctrl);

	// calculate capture banding filter
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			0x3014, &temp, MSM_CAMERA_I2C_BYTE_DATA);
	if (temp & 0x40) {
		msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				0x508e, &temp, MSM_CAMERA_I2C_BYTE_DATA);
		if (temp & 0x01) {
			capture_bandingfilter = capture_sysclk * 100 / capture_HTS;//50Hz
			}
		else
			{
			capture_bandingfilter = capture_sysclk * 100 / capture_HTS * 100 / 120;//60Hz
			}
		}
	else
		{
		if(temp & 0x80)
			{
			capture_bandingfilter = capture_sysclk * 100 / capture_HTS;//50Hz
			}
		else
			{
			capture_bandingfilter = capture_sysclk * 100 / capture_HTS * 100 / 120;//60Hz
			}
		}
	capture_max_band = (unsigned int)((capture_VTS-4)/capture_bandingfilter);
	// gain to shutter
	gain16 = gain16 * capture_sysclk/preview_sysclk * preview_HTS/capture_HTS * binning;
	if (gain16 < 16)
		gain16 = 16;
	capture_gain16_shutter = gain16 * shutter;
	shutter = gain16 * shutter;
	if(shutter < (capture_bandingfilter * 16)) {
		// shutter < 1/100
		shutter = capture_gain16_shutter/16;
		gain16 = capture_gain16_shutter/shutter;
	}
	else {
		if(capture_gain16_shutter > (capture_bandingfilter*capture_max_band*16)) {
			// exposure reach max
			shutter = capture_bandingfilter*capture_max_band;
			gain16 = capture_gain16_shutter / shutter;
		}
		else {
			// 1/100 < capture_shutter < max, capture_shutter = n/100
			shutter = ((unsigned int) (capture_gain16_shutter/(16*capture_bandingfilter))) * capture_bandingfilter;
			gain16 = capture_gain16_shutter / shutter;
		}
	}
	// write capture gain
	gain16 = gain16 & 0x1ff;   // max gain is 32x

	temp = 0;
	if (gain16 > 32) {
		gain16 = gain16 /2;
		temp = 0x10;
	}
	if (gain16 > 32) {
		gain16 = gain16 /2;
		temp = temp | 0x20;
	}
	if (gain16 > 32) {
		gain16 = gain16 /2;
		temp = temp | 0x40;
	}
	if (gain16 > 32) {
		gain16 = gain16 /2;
		temp = temp | 0x80;
	}
	
	temp = temp | (gain16 -16);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3000, temp, MSM_CAMERA_I2C_BYTE_DATA);
	// write capture shutter
	if (shutter > (capture_VTS - 4)) {
		capture_VTS = shutter + 4;
		temp = capture_VTS & 0xff;
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x302b, temp, MSM_CAMERA_I2C_BYTE_DATA);
		temp = capture_VTS >> 8;
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				0x302a, temp, MSM_CAMERA_I2C_BYTE_DATA);
	}

	shutter = shutter & 0xffff;
	temp = shutter & 0xff;
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3003, temp, MSM_CAMERA_I2C_BYTE_DATA);
	temp = shutter >> 8;
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			0x3002, temp, MSM_CAMERA_I2C_BYTE_DATA);
}

int32_t ov2675_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res)
{
	int32_t rc = 0;

	if (update_type == MSM_SENSOR_REG_INIT) {
		s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);
		msm_sensor_write_init_settings(s_ctrl);
	} else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		if (res == MSM_SENSOR_RES_FULL ){
			ov2675_set_shutter(s_ctrl);
		}
		msm_sensor_write_res_settings(s_ctrl, res);

		if(res == MSM_SENSOR_RES_QTR)
			OV2675_set_bandingfilter(s_ctrl);
		v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
			NOTIFY_PCLK_CHANGE, &s_ctrl->msm_sensor_reg->
			output_settings[res].op_pixel_clk);
	}
	return rc;
}

static struct msm_sensor_fn_t ov2675_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_set_fps = msm_sensor_set_fps,
#if 0
	.sensor_write_exp_gain = ov2675_write_exp_gain,
	.sensor_write_snapshot_exp_gain = ov2675_write_exp_gain,
#endif
	.sensor_setting = ov2675_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
	//.sensor_adjust_frame_lines = msm_sensor_adjust_frame_lines2,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
};

static struct msm_sensor_reg_t ov2675_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = ov2675_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(ov2675_start_settings),
	.stop_stream_conf = ov2675_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(ov2675_stop_settings),
	.init_settings = &ov2675_init_conf[0],
	.init_size = ARRAY_SIZE(ov2675_init_conf),
	.mode_settings = &ov2675_confs[0],
/*************************************************/
	.no_effect_settings = &ov2675_no_effect_confs[0],
/*************************************************/
	.output_settings = &ov2675_dimensions[0],
	.num_conf = ARRAY_SIZE(ov2675_confs),
};

static struct msm_sensor_ctrl_t ov2675_s_ctrl = {
	.msm_sensor_reg = &ov2675_regs,
/*************************************************/
	.msm_sensor_v4l2_ctrl_info = ov2675_v4l2_ctrl_info,
	.num_v4l2_ctrl = ARRAY_SIZE(ov2675_v4l2_ctrl_info),
/*************************************************/
	.sensor_i2c_client = &ov2675_sensor_i2c_client,
	.sensor_i2c_addr = 0x60,
	.vreg_seq = ov2675_veg_seq,
	.num_vreg_seq = ARRAY_SIZE(ov2675_veg_seq),
	.sensor_output_reg_addr = &ov2675_reg_addr,
	.sensor_id_info = &ov2675_id_info,
	//.sensor_exp_gain_info = &ov2675_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex = &ov2675_mut,
	.sensor_i2c_driver = &ov2675_i2c_driver,
	.sensor_v4l2_subdev_info = ov2675_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov2675_subdev_info),
	.sensor_v4l2_subdev_ops = &ov2675_subdev_ops,
	.func_tbl = &ov2675_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

module_init(msm_sensor_init_module);
module_exit(msm_sensor_exit_module);
MODULE_DESCRIPTION("Omnivision 2MP YUV sensor driver");
MODULE_LICENSE("GPL v2");


