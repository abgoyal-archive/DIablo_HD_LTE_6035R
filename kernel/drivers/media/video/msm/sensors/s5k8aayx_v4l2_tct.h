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
 * 13/06/04      Hu Jin      Add effect
 */
#ifndef S5K8AAYX_V4L2_TCT_H__
#define S5K8AAYX_V4L2_TCT_H__
#define EFFECT_ON	0
#if EFFECT_ON
static int effect_value = CAMERA_EFFECT_OFF;

static struct msm_camera_i2c_reg_conf s5k8aayx_saturation[][4] = {
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

static struct msm_camera_i2c_conf_array s5k8aayx_saturation_confs[][1] = {
	{{s5k8aayx_saturation[0], ARRAY_SIZE(s5k8aayx_saturation[0]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_saturation[1], ARRAY_SIZE(s5k8aayx_saturation[1]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_saturation[2], ARRAY_SIZE(s5k8aayx_saturation[2]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_saturation[3], ARRAY_SIZE(s5k8aayx_saturation[3]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_saturation[4], ARRAY_SIZE(s5k8aayx_saturation[4]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_saturation[5], ARRAY_SIZE(s5k8aayx_saturation[5]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_saturation[6], ARRAY_SIZE(s5k8aayx_saturation[6]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_saturation[7], ARRAY_SIZE(s5k8aayx_saturation[7]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_saturation[8], ARRAY_SIZE(s5k8aayx_saturation[8]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int s5k8aayx_saturation_enum_map[] = {
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

static struct msm_camera_i2c_enum_conf_array s5k8aayx_saturation_enum_confs = {
	.conf = &s5k8aayx_saturation_confs[0][0],
	.conf_enum = s5k8aayx_saturation_enum_map,
	.num_enum = ARRAY_SIZE(s5k8aayx_saturation_enum_map),
	.num_index = ARRAY_SIZE(s5k8aayx_saturation_confs),
	.num_conf = ARRAY_SIZE(s5k8aayx_saturation_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf s5k8aayx_contrast[][4] = {
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

static struct msm_camera_i2c_conf_array s5k8aayx_contrast_confs[][1] = {
	{{s5k8aayx_contrast[0], ARRAY_SIZE(s5k8aayx_contrast[0]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_contrast[1], ARRAY_SIZE(s5k8aayx_contrast[1]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_contrast[2], ARRAY_SIZE(s5k8aayx_contrast[2]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_contrast[3], ARRAY_SIZE(s5k8aayx_contrast[3]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_contrast[4], ARRAY_SIZE(s5k8aayx_contrast[4]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_contrast[5], ARRAY_SIZE(s5k8aayx_contrast[5]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_contrast[6], ARRAY_SIZE(s5k8aayx_contrast[6]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_contrast[7], ARRAY_SIZE(s5k8aayx_contrast[7]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_contrast[8], ARRAY_SIZE(s5k8aayx_contrast[8]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};


static int s5k8aayx_contrast_enum_map[] = {
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

static struct msm_camera_i2c_enum_conf_array s5k8aayx_contrast_enum_confs = {
	.conf = &s5k8aayx_contrast_confs[0][0],
	.conf_enum = s5k8aayx_contrast_enum_map,
	.num_enum = ARRAY_SIZE(s5k8aayx_contrast_enum_map),
	.num_index = ARRAY_SIZE(s5k8aayx_contrast_confs),
	.num_conf = ARRAY_SIZE(s5k8aayx_contrast_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};
static struct msm_camera_i2c_reg_conf s5k8aayx_sharpness[][5] = {
	{{0x3306, 0x00}, {0x3376, 0x01},{0x3377, 0x00},{0x3378, 0x10},{0x3379, 0x80},},    /* SHARPNESS LEVEL 0*/
	{{0x3306, 0x00}, {0x3376, 0x02},{0x3377, 0x00},{0x3378, 0x08},{0x3379, 0x80},},    /* SHARPNESS LEVEL 1*/
	{{0x3306, 0x00}, {0x3376, 0x04},{0x3377, 0x00},{0x3378, 0x04},{0x3379, 0x80},},    /* SHARPNESS LEVEL 2*/
	{{0x3306, 0x00}, {0x3376, 0x06},{0x3377, 0x00},{0x3378, 0x04},{0x3379, 0x80},},    /* SHARPNESS LEVEL 3*/
	{{0x3306, 0x00}, {0x3376, 0x08},{0x3377, 0x00},{0x3378, 0x04},{0x3379, 0x80},},    /* SHARPNESS LEVEL 4*/
	{{0x3306, 0x00}, {0x3376, 0x0a},{0x3377, 0x00},{0x3378, 0x04},{0x3379, 0x80},},    /* SHARPNESS LEVEL 5*/
};

static struct msm_camera_i2c_conf_array s5k8aayx_sharpness_confs[][1] = {
	{{s5k8aayx_sharpness[0], ARRAY_SIZE(s5k8aayx_sharpness[0]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_sharpness[1], ARRAY_SIZE(s5k8aayx_sharpness[1]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_sharpness[2], ARRAY_SIZE(s5k8aayx_sharpness[2]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_sharpness[3], ARRAY_SIZE(s5k8aayx_sharpness[3]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_sharpness[4], ARRAY_SIZE(s5k8aayx_sharpness[4]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_sharpness[5], ARRAY_SIZE(s5k8aayx_sharpness[5]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int s5k8aayx_sharpness_enum_map[] = {
	MSM_V4L2_SHARPNESS_L0,
	MSM_V4L2_SHARPNESS_L1,
	MSM_V4L2_SHARPNESS_L2,
	MSM_V4L2_SHARPNESS_L3,
	MSM_V4L2_SHARPNESS_L4,
	MSM_V4L2_SHARPNESS_L5,
};

static struct msm_camera_i2c_enum_conf_array s5k8aayx_sharpness_enum_confs = {
	.conf = &s5k8aayx_sharpness_confs[0][0],
	.conf_enum = s5k8aayx_sharpness_enum_map,
	.num_enum = ARRAY_SIZE(s5k8aayx_sharpness_enum_map),
	.num_index = ARRAY_SIZE(s5k8aayx_sharpness_confs),
	.num_conf = ARRAY_SIZE(s5k8aayx_sharpness_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf s5k8aayx_exposure[][3] = {
	{{0x3018, 0x6a}, {0x3019, 0x5a}, {0x301a, 0xd4},}, /*EXPOSURECOMPENSATIONP2*/
	{{0x3018, 0x78}, {0x3019, 0x68}, {0x301a, 0xa5},}, /*EXPOSURECOMPENSATIONp1*/
	{{0x3018, 0x88}, {0x3019, 0x78}, {0x301a, 0xd4},}, /*EXPOSURECOMPENSATIOND*/
	{{0x3018, 0x98}, {0x3019, 0x88}, {0x301a, 0xd4},}, /*EXPOSURECOMPENSATIONN1*/
	{{0x3018, 0xa8}, {0x3019, 0x98}, {0x301a, 0xe5},}, /*EXPOSURECOMPENSATIONN2*/
};

static struct msm_camera_i2c_conf_array s5k8aayx_exposure_confs[][1] = {
	{{s5k8aayx_exposure[0], ARRAY_SIZE(s5k8aayx_exposure[0]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_exposure[1], ARRAY_SIZE(s5k8aayx_exposure[1]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_exposure[2], ARRAY_SIZE(s5k8aayx_exposure[2]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_exposure[3], ARRAY_SIZE(s5k8aayx_exposure[3]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_exposure[4], ARRAY_SIZE(s5k8aayx_exposure[4]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int s5k8aayx_exposure_enum_map[] = {
	MSM_V4L2_EXPOSURE_N2,
	MSM_V4L2_EXPOSURE_N1,
	MSM_V4L2_EXPOSURE_D,
	MSM_V4L2_EXPOSURE_P1,
	MSM_V4L2_EXPOSURE_P2,
};

static struct msm_camera_i2c_enum_conf_array s5k8aayx_exposure_enum_confs = {
	.conf = &s5k8aayx_exposure_confs[0][0],
	.conf_enum = s5k8aayx_exposure_enum_map,
	.num_enum = ARRAY_SIZE(s5k8aayx_exposure_enum_map),
	.num_index = ARRAY_SIZE(s5k8aayx_exposure_confs),
	.num_conf = ARRAY_SIZE(s5k8aayx_exposure_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf s5k8aayx_iso[][1] = {
	{{0x3015, 0x02},},   /*ISO_AUTO*/
	{{0x3015, 0x02},},   /*ISO_DEBLUR*/
	{{0x3015, 0x00},},   /*ISO_100*/
	{{0x3015, 0x01},},   /*ISO_200*/
	{{0x3015, 0x02},},   /*ISO_400*/
	{{0x3015, 0x03},},   /*ISO_800*/
	{{0x3015, 0x03},},   /*ISO_1600*/
};



static struct msm_camera_i2c_conf_array s5k8aayx_iso_confs[][1] = {
	{{s5k8aayx_iso[0], ARRAY_SIZE(s5k8aayx_iso[0]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_iso[1], ARRAY_SIZE(s5k8aayx_iso[1]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_iso[2], ARRAY_SIZE(s5k8aayx_iso[2]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_iso[3], ARRAY_SIZE(s5k8aayx_iso[3]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_iso[4], ARRAY_SIZE(s5k8aayx_iso[4]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_iso[5], ARRAY_SIZE(s5k8aayx_iso[5]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_iso[6], ARRAY_SIZE(s5k8aayx_iso[6]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int s5k8aayx_iso_enum_map[] = {
	MSM_V4L2_ISO_AUTO ,
	MSM_V4L2_ISO_DEBLUR,
	MSM_V4L2_ISO_100,
	MSM_V4L2_ISO_200,
	MSM_V4L2_ISO_400,
	MSM_V4L2_ISO_800,
	MSM_V4L2_ISO_1600,
};


static struct msm_camera_i2c_enum_conf_array s5k8aayx_iso_enum_confs = {
	.conf = &s5k8aayx_iso_confs[0][0],
	.conf_enum = s5k8aayx_iso_enum_map,
	.num_enum = ARRAY_SIZE(s5k8aayx_iso_enum_map),
	.num_index = ARRAY_SIZE(s5k8aayx_iso_confs),
	.num_conf = ARRAY_SIZE(s5k8aayx_iso_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf s5k8aayx_no_effect[] = {
	{0x3391, 0x00},
	{-1, -1},
	{-1, -1},
};

static struct msm_camera_i2c_conf_array s5k8aayx_no_effect_confs[] = {
	{&s5k8aayx_no_effect[0], ARRAY_SIZE(s5k8aayx_no_effect), 0,
		MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_camera_i2c_reg_conf s5k8aayx_special_effect[][3] = {
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

static struct msm_camera_i2c_conf_array s5k8aayx_special_effect_confs[][1] = {
	{{s5k8aayx_special_effect[0],  ARRAY_SIZE(s5k8aayx_special_effect[0]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[1],  ARRAY_SIZE(s5k8aayx_special_effect[1]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[2],  ARRAY_SIZE(s5k8aayx_special_effect[2]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[3],  ARRAY_SIZE(s5k8aayx_special_effect[3]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[4],  ARRAY_SIZE(s5k8aayx_special_effect[4]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[5],  ARRAY_SIZE(s5k8aayx_special_effect[5]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[6],  ARRAY_SIZE(s5k8aayx_special_effect[6]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[7],  ARRAY_SIZE(s5k8aayx_special_effect[7]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[8],  ARRAY_SIZE(s5k8aayx_special_effect[8]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[9],  ARRAY_SIZE(s5k8aayx_special_effect[9]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[10], ARRAY_SIZE(s5k8aayx_special_effect[10]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[11], ARRAY_SIZE(s5k8aayx_special_effect[11]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_special_effect[12], ARRAY_SIZE(s5k8aayx_special_effect[12]), 0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int s5k8aayx_special_effect_enum_map[] = {
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

static struct msm_camera_i2c_enum_conf_array s5k8aayx_special_effect_enum_confs = {
	.conf = &s5k8aayx_special_effect_confs[0][0],
	.conf_enum = s5k8aayx_special_effect_enum_map,
	.num_enum = ARRAY_SIZE(s5k8aayx_special_effect_enum_map),
	.num_index = ARRAY_SIZE(s5k8aayx_special_effect_confs),
	.num_conf = ARRAY_SIZE(s5k8aayx_special_effect_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf s5k8aayx_antibanding[][1] = {
	{{0x3014, 0x00},},	 /*ANTIBANDING 60HZ*/
	{{0x3014, 0x80},},	 /*ANTIBANDING 50HZ*/
	{{0x3014, 0xc0},},	 /* ANTIBANDING AUTO*/
};


static struct msm_camera_i2c_conf_array s5k8aayx_antibanding_confs[][1] = {
	{{s5k8aayx_antibanding[0], ARRAY_SIZE(s5k8aayx_antibanding[0]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_antibanding[1], ARRAY_SIZE(s5k8aayx_antibanding[1]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_antibanding[2], ARRAY_SIZE(s5k8aayx_antibanding[2]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int s5k8aayx_antibanding_enum_map[] = {
	MSM_V4L2_POWER_LINE_60HZ,
	MSM_V4L2_POWER_LINE_50HZ,
	MSM_V4L2_POWER_LINE_AUTO,
};


static struct msm_camera_i2c_enum_conf_array s5k8aayx_antibanding_enum_confs = {
	.conf = &s5k8aayx_antibanding_confs[0][0],
	.conf_enum = s5k8aayx_antibanding_enum_map,
	.num_enum = ARRAY_SIZE(s5k8aayx_antibanding_enum_map),
	.num_index = ARRAY_SIZE(s5k8aayx_antibanding_confs),
	.num_conf = ARRAY_SIZE(s5k8aayx_antibanding_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_camera_i2c_reg_conf s5k8aayx_wb_oem[][4] = {
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

static struct msm_camera_i2c_conf_array s5k8aayx_wb_oem_confs[][1] = {
	{{s5k8aayx_wb_oem[0], ARRAY_SIZE(s5k8aayx_wb_oem[0]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_wb_oem[1], ARRAY_SIZE(s5k8aayx_wb_oem[1]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_wb_oem[2], ARRAY_SIZE(s5k8aayx_wb_oem[2]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_wb_oem[3], ARRAY_SIZE(s5k8aayx_wb_oem[3]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_wb_oem[4], ARRAY_SIZE(s5k8aayx_wb_oem[4]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_wb_oem[5], ARRAY_SIZE(s5k8aayx_wb_oem[5]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
	{{s5k8aayx_wb_oem[6], ARRAY_SIZE(s5k8aayx_wb_oem[6]),  0,
		MSM_CAMERA_I2C_BYTE_DATA},},
};

static int s5k8aayx_wb_oem_enum_map[] = {
	MSM_V4L2_WB_OFF,
	MSM_V4L2_WB_AUTO ,
	MSM_V4L2_WB_CUSTOM,
	MSM_V4L2_WB_INCANDESCENT,
	MSM_V4L2_WB_FLUORESCENT,
	MSM_V4L2_WB_DAYLIGHT,
	MSM_V4L2_WB_CLOUDY_DAYLIGHT,
};

static struct msm_camera_i2c_enum_conf_array s5k8aayx_wb_oem_enum_confs = {
	.conf = &s5k8aayx_wb_oem_confs[0][0],
	.conf_enum = s5k8aayx_wb_oem_enum_map,
	.num_enum = ARRAY_SIZE(s5k8aayx_wb_oem_enum_map),
	.num_index = ARRAY_SIZE(s5k8aayx_wb_oem_confs),
	.num_conf = ARRAY_SIZE(s5k8aayx_wb_oem_confs[0]),
	.data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

int s5k8aayx_saturation_msm_sensor_s_ctrl_by_enum(
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
int s5k8aayx_contrast_msm_sensor_s_ctrl_by_enum(
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
int s5k8aayx_sharpness_msm_sensor_s_ctrl_by_enum(
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

int s5k8aayx_effect_msm_sensor_s_ctrl_by_enum(struct msm_sensor_ctrl_t *s_ctrl,
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

int s5k8aayx_antibanding_msm_sensor_s_ctrl_by_enum(
		struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	CDBG("--CAMERA-- %s effect_value = %d \n", __func__,effect_value);
	return rc;
}

int s5k8aayx_msm_sensor_s_ctrl_by_enum(struct msm_sensor_ctrl_t *s_ctrl,
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
struct msm_sensor_v4l2_ctrl_info_t s5k8aayx_v4l2_ctrl_info[] = {
	{
		.ctrl_id = V4L2_CID_SATURATION,
		.min = MSM_V4L2_SATURATION_L0,
		.max = MSM_V4L2_SATURATION_L8,
		.step = 1,
		.enum_cfg_settings = &s5k8aayx_saturation_enum_confs,
		.s_v4l2_ctrl = s5k8aayx_saturation_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_CONTRAST,
		.min = MSM_V4L2_CONTRAST_L0,
		.max = MSM_V4L2_CONTRAST_L8,
		.step = 1,
		.enum_cfg_settings = &s5k8aayx_contrast_enum_confs,
		.s_v4l2_ctrl = s5k8aayx_contrast_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_SHARPNESS,
		.min = MSM_V4L2_SHARPNESS_L0,
		.max = MSM_V4L2_SHARPNESS_L5,
		.step = 1,
		.enum_cfg_settings = &s5k8aayx_sharpness_enum_confs,
		.s_v4l2_ctrl = s5k8aayx_sharpness_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_EXPOSURE,
		.min = MSM_V4L2_EXPOSURE_N2,
		.max = MSM_V4L2_EXPOSURE_P2,
		.step = 1,
		.enum_cfg_settings = &s5k8aayx_exposure_enum_confs,
		.s_v4l2_ctrl = s5k8aayx_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = MSM_V4L2_PID_ISO,
		.min = MSM_V4L2_ISO_AUTO,
		.max = MSM_V4L2_ISO_1600,
		.step = 1,
		.enum_cfg_settings = &s5k8aayx_iso_enum_confs,
		.s_v4l2_ctrl = s5k8aayx_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_SPECIAL_EFFECT,
		.min = MSM_V4L2_EFFECT_OFF,
		.max = MSM_V4L2_EFFECT_NEGATIVE,
		.step = 1,
		.enum_cfg_settings = &s5k8aayx_special_effect_enum_confs,
		.s_v4l2_ctrl = s5k8aayx_effect_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_POWER_LINE_FREQUENCY,
		.min = MSM_V4L2_POWER_LINE_60HZ,
		.max = MSM_V4L2_POWER_LINE_AUTO,
		.step = 1,
		.enum_cfg_settings = &s5k8aayx_antibanding_enum_confs,
		.s_v4l2_ctrl = s5k8aayx_antibanding_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_WHITE_BALANCE_TEMPERATURE,
		.min = MSM_V4L2_WB_AUTO,
		.max = MSM_V4L2_WB_CLOUDY_DAYLIGHT,
		.step = 1,
		.enum_cfg_settings = &s5k8aayx_wb_oem_enum_confs,
		.s_v4l2_ctrl = s5k8aayx_msm_sensor_s_ctrl_by_enum,
	},
};

#endif
#endif

