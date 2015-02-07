/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
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
 * 13/03/26      Hu Jin       Add driver for DiabloLTE for 2lane
 * 13/04/08      pengdong.su  Modify the main camera config
 * 13/04/09      Hu Jin       Support 4lane
 * 13/04/10      Hu Jin       Enable zsl & 1080p
 * 13/04/15      Hu Jin       Sometimes force close when launch camera
 * 13/04/16      Hu Jin       The white balance function is nok
 * 13/04/23      Hu Jin       OTP calibration ,shading , awb
 * 13/07/09      Hu Jin       change to 24fps
 * 13/07/09      Hu Jin       sclk -> 200M
 * 13/08/16      Hu Jin       Burst shot blinking in lowlight[501005], disable BLC update/frame
 * --------------------------------------------------------------------------------------
 */
#include "ov8850_v4l2_tct_otp.h"
#include "msm_sensor.h"
#include "msm.h"
#define SENSOR_NAME "ov8850"
#define PLATFORM_DRIVER_NAME "msm_camera_ov8850"
#define OTP_ENABLE 1

DEFINE_MUTEX(ov8850_mut);
static struct msm_sensor_ctrl_t ov8850_s_ctrl;

static struct msm_camera_i2c_reg_conf ov8850_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf ov8850_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_camera_i2c_reg_conf ov8850_groupon_settings[] = {
	{0x3208, 0x00},
};

static struct msm_camera_i2c_reg_conf ov8850_groupoff_settings[] = {
	{0x3208, 0x10},
	{0x3208, 0xA0},
};

static struct msm_camera_i2c_reg_conf ov8850_reset_settings[] = {
	{0x0103, 0x01},
};
static struct msm_camera_i2c_reg_conf ov8850_recommend_settings[] = {
    //;@@0300globalsetting
    //{0x0103, 0x01}, //;softwarereset
    {0x0102, 0x01}, //;
    {0x3002, 0x08}, //;FREXoutputenable
    {0x3004, 0x00}, //
    {0x3005, 0x00}, //;strobe,PWMinput
    {0x3011, 0x41}, //;MIPI4lane
    {0x3012, 0x08}, //
    {0x3014, 0x4a}, //
    {0x3015, 0x0a}, //`;MIPI4laneenable
    {0x3021, 0x00}, //
    {0x3022, 0x02}, //
    {0x3081, 0x02}, //
    {0x3083, 0x01}, //
    {0x3092, 0x00}, //;PLL2divs
    {0x3093, 0x00}, //;PLL2seld5
    {0x309a, 0x00}, //;PLL3divs
    {0x309b, 0x00}, //;PLL3div
    {0x309c, 0x00}, //;PLL3multiplier
    {0x30b3, 0x62}, //;PLL1multiplier
    {0x30b4, 0x03}, //;PLL1prediv
    {0x30b5, 0x04}, //;PLL1op_pix_div
    {0x30b6, 0x01}, //;PLL1op_sys_div
    {0x3104, 0xa1}, //
    {0x3106, 0x01}, //
    {0x3503, 0x07}, //;AGCmanual,AECmanual
    {0x350a, 0x00}, //;GainH
    {0x350b, 0x38}, //;GainL
    {0x3602, 0x70}, //;analogcontrol
    {0x3620, 0x64}, //;
    {0x3622, 0x0f}, //;
    {0x3623, 0x68}, //;
    {0x3625, 0x40}, //;
    {0x3631, 0x83}, //;
    {0x3633, 0x34}, //;
    {0x3634, 0x03}, //;
    {0x364c, 0x00}, //;
    {0x364d, 0x00}, //;
    {0x364e, 0x00}, //;
    {0x364f, 0x00}, //;
    {0x3660, 0x80}, //;
    {0x3662, 0x10}, //;
    {0x3665, 0x00}, //;
    {0x3666, 0x00}, //;
    {0x366f, 0x20}, //;analogcontrol
    {0x3703, 0x2e}, //;sensorcontrol
    {0x3732, 0x05}, //;
    {0x373a, 0x51}, //;
    {0x373d, 0x22}, //;
    {0x3754, 0xc0}, //;
    {0x3756, 0x2a}, //;
    {0x3759, 0x0f}, //;
    {0x376b, 0x44}, //;sensorcontrol
    {0x3795, 0x00}, //;PSRAMcontrol
    {0x379c, 0x0c}, //;PSRAMcontrol
    {0x3810, 0x00}, //;ISPxoffsetH
    {0x3811, 0x04}, //;ISPxoffsetL
    {0x3812, 0x00}, //;ISPyoffsetH
    {0x3813, 0x04}, //;ISPyoffsetL
    {0x3820, 0x10}, //;binoff,flipoff
    {0x3821, 0x0e}, //;binoff,mirroron
    {0x3826, 0x00}, //
    {0x4000, 0x10}, //;enablemoduleDCBLCautoload
    {0x4002, 0xc5}, //;BLCleveltriggeron
    {0x4005, 0x18}, //;BLCleveltrigger
    {0x4006, 0x20}, //
    {0x4008, 0x20}, //
    {0x4009, 0x10}, //
    {0x404f, 0xA0}, //;BLCstablerange
    {0x4100, 0x1d}, //
    {0x4101, 0x23}, //
    {0x4102, 0x44}, //
    {0x4104, 0x5c}, //
    {0x4109, 0x03}, //
    {0x4300, 0xff}, //;datamaxH
    {0x4301, 0x00}, //;dataminH
    {0x4315, 0x00}, //;Vsyncdelay
    {0x4512, 0x01}, //;verticalbinningaverage
    {0x4837, 0x08}, //;MIPIglobaltiming
    {0x4a00, 0xaa}, //;LVDScontrol
    {0x4a03, 0x01}, //;LVDScontrol
    {0x4a05, 0x08}, //;LVDScontrol
    {0x4d00, 0x04}, //;temperaturemonitor
    {0x4d01, 0x52}, //;
    {0x4d02, 0xfe}, //;
    {0x4d03, 0x05}, //;
    {0x4d04, 0xff}, //;
    {0x4d05, 0xff}, //;temperaturemonitor
    {0x5000, 0x06}, //;DPCon
    {0x5001, 0x01}, //;MWBon
    {0x5002, 0x80}, //;scaleon
    {0x5041, 0x04}, //;averageenable
    {0x5043, 0x48}, //
    {0x5e00, 0x00}, //;testpatternoff
    {0x5e10, 0x1c}, //
    {0x3500, 0x00}, //;AEC[18:16]
    {0x3501, 0x9C}, //;AEC[15:8]
    {0x3502, 0x20}, //;AEC[7:0]
    {0x3090, 0x03}, //;PLL2prediv
    {0x3091, 0x11}, //;PLL2multiplier
    {0x3094, 0x01}, //;PLL2mult2
    {0x3098, 0x02}, //;PLL3prediv
    {0x3099, 0x16}, //;PLL3multiplier
    {0x3624, 0x04}, //;analogcontrol
    {0x3680, 0xb0}, //;analogcontrol
    {0x3702, 0x6e}, //;sensorcontrol
    {0x3704, 0x55}, //;sensorcontrol
    {0x3708, 0xe4}, //;sensorcontrol
    {0x3709, 0xc3}, //;sensorcontrol
    {0x371f, 0x0d}, //;sensorcontrol
    {0x3739, 0x80}, //;sensorcontrol
    {0x373c, 0x24}, //;sensorcontrol
    {0x3781, 0xc8}, //;PSRAMcontrol
    {0x3786, 0x08}, //;PSRAMcontrol
    {0x3796, 0x43}, //;PSRAMcontrol
    {0x3800, 0x00}, //;xstartH
    {0x3801, 0x04}, //;xstartL
    {0x3802, 0x00}, //;ystartH
    {0x3803, 0x0c}, //;ystartL
    {0x3804, 0x0c}, //;xendH
    {0x3805, 0xcb}, //;xendL
    {0x3806, 0x09}, //;yendH
    {0x3807, 0xa3}, //;yendL
    {0x3808, 0x0c}, //;xoutputsizeH
    {0x3809, 0xc0}, //;xoutputsizeL
    {0x380a, 0x09}, //;youtputsizeH
    {0x380b, 0x90}, //;youtputsizeL
    {0x380c, 0x0e}, //;HTSH
    {0x380d, 0x17}, //;HTSL
    {0x380e, 0x09}, //;VTSH
    {0x380f, 0xD0}, //;VTSL
    {0x3814, 0x11}, //;xinc
    {0x3815, 0x11}, //;yinc
    {0x3820, 0x10}, //;binoff,flipoff
    {0x3821, 0x0e}, //;binoff,mirroron
    {0x3a04, 0x09}, //
    {0x3a05, 0xcc}, //
    {0x4001, 0x06}, //;BLCstartline
    {0x4004, 0x04}, //;numberofblacklines
    
    {0x3011, 0x41}, //
    {0x3015, 0x0a}, //
    {0x3090, 0x03}, //
    {0x3091, 0x19}, //
    {0x3092, 0x00}, //
    {0x3093, 0x00}, //
    {0x3094, 0x00}, //
    {0x30b3, 0x50}, //
    {0x30b4, 0x03}, //
    {0x30b5, 0x04}, //
    {0x30b6, 0x01}, //
    {0x4837, 0x0d}, //
    {0x3098, 0x03}, //
    {0x3099, 0x1e}, //
    {0x309a, 0x00}, //
    {0x309b, 0x00}, //
    {0x309c, 0x00}, //
};

static struct msm_camera_i2c_reg_conf ov8850_prev_settings[] = {
    {0x3500, 0x00}, // ; AEC[18:16]
    {0x3501, 0x7c}, // ; AEC[15:8]
    {0x3502, 0x00}, // ; AEC[7:0]
    {0x3624, 0x00}, // ; analog control
    {0x3680, 0xe0}, // ; analog control
    {0x3702, 0xf3}, // ; sensor control
    {0x3704, 0x71}, // ; sensor control
    {0x3708, 0xe6}, // ; sensor control
    {0x3709, 0xc3}, // ; sensor control
    {0x371f, 0x0c}, // ; sensor control
    {0x3739, 0x30}, // ; sensor control
    {0x373c, 0x20}, // ; sensor control
    {0x3781, 0x0c}, // ; PSRAM control
    {0x3786, 0x16}, // ; PSRAM control
    {0x3796, 0x64}, // ; PSRAM control
    {0x3800, 0x00}, // ; x start H
    {0x3801, 0x00}, // ; x start L
    {0x3802, 0x00}, // ; y start H
    {0x3803, 0x00}, // ; y start L
    {0x3804, 0x0c}, // ; x end H
    {0x3805, 0xcf}, // ; x end L
    {0x3806, 0x09}, // ; y end H
    {0x3807, 0x9f}, // ; y ens L
    {0x3808, 0x06}, // ; x output size H
    {0x3809, 0x60}, // ; x output size L
    {0x380a, 0x04}, // ; y output size H
    {0x380b, 0xc8}, // ; y output size L
    {0x380c, 0x0d}, // ; HTS H
    {0x380d, 0x0c}, // ; HTS L
    {0x380e, 0x09}, // ; VTS H
    {0x380f, 0xbe}, // ; VTS L
    {0x3814, 0x31}, // ; x inc
    {0x3815, 0x31}, // ; y inc
    {0x3820, 0x53}, // ; bin on, flip off
    {0x3821, 0x09}, // ; bin on, mirror on
    {0x3a04, 0x07}, //
    {0x3a05, 0xc8}, //
    {0x4001, 0x02}, // ; BLC start line
    {0x4004, 0x04}, // ; number of black line
    {0x4005, 0x18}, // ; BLC trigger every frame, for single capture
    {0x4102, 0x44}, //
};

static struct msm_camera_i2c_reg_conf ov8850_snap_settings[] = {
    {0x3500, 0x00},//;AEC[18:16]
    {0x3501, 0x9c},//;AEC[15:8]
    {0x3502, 0x20},//;AEC[7:0]
    {0x3624, 0x04},//;analogcontrol
    {0x3680, 0xb0},//;analogcontrol
    {0x3702, 0x6e},//;sensorcontrol
    {0x3704, 0x55},//;sensorcontrol
    {0x3708, 0xe4},//;sensorcontrol
    {0x3709, 0xc3},//;sensorcontrol
    {0x371f, 0x0d},//;sensorcontrol
    {0x3739, 0x80},//;sensorcontrol
    {0x373c, 0x24},//;sensorcontrol
    {0x3781, 0xc8},//;PSRAMcontrol
    {0x3786, 0x08},//;PSRAMcontrol
    {0x3796, 0x43},//;PSRAMcontrol
    {0x3800, 0x00},//;xstartH
    {0x3801, 0x04},//;xstartL
    {0x3802, 0x00},//;ystartH
    {0x3803, 0x0c},//;ystartL
    {0x3804, 0x0c},//;xendH
    {0x3805, 0xcb},//;xendL
    {0x3806, 0x09},//;yendH
    {0x3807, 0xa3},//;yensL
    {0x3808, 0x0c},//;xoutputsizeH
    {0x3809, 0xc0},//;xoutputsizeL
    {0x380a, 0x09},//;youtputsizeH
    {0x380b, 0x90},//;youtputsizeL
    {0x380c, 0x0e},//;HTSH
    {0x380d, 0x17},//;HTSL
    {0x380e, 0x09},//;VTSH
    {0x380f, 0xd0},//;VTSL
    {0x3814, 0x11},//;xinc
    {0x3815, 0x11},//;yinc
//[BUGFIX]-MOD by pengdong.su,419106,modify the config
    {0x3820, 0x52},//;binoff,flipoff
    {0x3821, 0x08},//;binoff,mirroron
//[BUGFIX]-MOD by pengdong.su
    {0x3a04, 0x09},//
    {0x3a05, 0xcc},//
    {0x4001, 0x06},//;BLCstartline
    {0x4004, 0x04},//;numberofblackline
    {0x4005, 0x1a},//;BLCtriggereveryframe,forsinglecapture
    {0x4102, 0x44}, //
};
static struct msm_camera_i2c_reg_conf ov8850_1080video_settings[] = {
    {0x3500, 0x00}, //; AEC[18:16]
    {0x3501, 0x7c}, //; AEC[15:8]
    {0x3502, 0x00}, //; AEC[7:0]
    {0x3624, 0x00}, //; analog control
    {0x3680, 0xe0}, //; analog control
    {0x3702, 0xf3}, //; sensor control
    {0x3704, 0x71}, //; sensor control
    {0x3708, 0xe3}, //; sensor control
    {0x3709, 0xc3}, //; sensor control
    {0x371f, 0x0c}, //; sensor control
    {0x3739, 0x30}, //; sensor control
    {0x373c, 0x20}, //; sensor control
    {0x3781, 0x0c}, //; PSRAM control
    {0x3786, 0x16}, //; PSRAM control
    {0x3796, 0x64}, //; PSRAM control
    {0x3800, 0x00}, //; x start H
    {0x3801, 0x0c}, //; x start L
    {0x3802, 0x01}, //; y start H
    {0x3803, 0x40}, //; y start L
    {0x3804, 0x0c}, //; x end H
    {0x3805, 0xd3}, //; x end L
    {0x3806, 0x08}, //; y end H
    {0x3807, 0x73}, //; y ens L
    {0x3808, 0x07}, //; x output size H
    {0x3809, 0x80}, //; x output size L
    {0x380a, 0x04}, //; y output size H
    {0x380b, 0x38}, //; y output size L
    {0x380c, 0x0d}, //; HTS H
    {0x380d, 0x0c}, //; HTS L
    {0x380e, 0x07}, //; VTS H
    {0x380f, 0xcc}, //; VTS L
    {0x3814, 0x11}, //; x inc
    {0x3815, 0x11}, //; y inc
	{0x3820, 0x52},//;binoff,flipoff
	{0x3821, 0x08},//;binoff,mirroron
    {0x3a04, 0x07}, //
    {0x3a05, 0xc8}, //
    {0x4001, 0x02}, //; BLC start line
    {0x4004, 0x08}, //; number of black line
    {0x4005, 0x18}, //; BLC trigger every frame, for single capture
    {0x4102, 0x44}, //
};

static struct msm_camera_i2c_reg_conf ov8850_zsl_settings[] = {
    {0x3500, 0x00},//;AEC[18:16]
    {0x3501, 0x9c},//;AEC[15:8]
    {0x3502, 0x20},//;AEC[7:0]
    {0x3624, 0x04},//;analogcontrol
    {0x3680, 0xb0},//;analogcontrol
    {0x3702, 0x6e},//;sensorcontrol
    {0x3704, 0x55},//;sensorcontrol
    {0x3708, 0xe4},//;sensorcontrol
    {0x3709, 0xc3},//;sensorcontrol
    {0x371f, 0x0d},//;sensorcontrol
    {0x3739, 0x80},//;sensorcontrol
    {0x373c, 0x24},//;sensorcontrol
    {0x3781, 0xc8},//;PSRAMcontrol
    {0x3786, 0x08},//;PSRAMcontrol
    {0x3796, 0x43},//;PSRAMcontrol
    {0x3800, 0x00},//;xstartH
    {0x3801, 0x04},//;xstartL
    {0x3802, 0x00},//;ystartH
    {0x3803, 0x0c},//;ystartL
    {0x3804, 0x0c},//;xendH
    {0x3805, 0xcb},//;xendL
    {0x3806, 0x09},//;yendH
    {0x3807, 0xa3},//;yensL
    {0x3808, 0x0c},//;xoutputsizeH
    {0x3809, 0xc0},//;xoutputsizeL
    {0x380a, 0x09},//;youtputsizeH
    {0x380b, 0x90},//;youtputsizeL
    {0x380c, 0x0d},//;HTSH
    {0x380d, 0x0c},//;HTSL
    {0x380e, 0x09},//;VTSH
    {0x380f, 0xbf},//;VTSL//[BUGFIX]-MOD by ZL PR502025
    {0x3814, 0x11},//;xinc
    {0x3815, 0x11},//;yinc
//[BUGFIX]-MOD by pengdong.su,419106,modify the config
    {0x3820, 0x52},//;binoff,flipoff
    {0x3821, 0x08},//;binoff,mirroron
//[BUGFIX]-MOD by pengdong.su
    {0x3a04, 0x09},//
    {0x3a05, 0xcc},//
    {0x4001, 0x06},//;BLCstartline
    {0x4004, 0x04},//;numberofblackline
    {0x4005, 0x18},//;BLCtriggereveryframe,forsinglecapture
    {0x4102, 0x04},//
};

static struct v4l2_subdev_info ov8850_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array ov8850_init_conf[] = {
	{&ov8850_reset_settings[0],
	ARRAY_SIZE(ov8850_reset_settings), 5, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8850_recommend_settings[0],
	ARRAY_SIZE(ov8850_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}
};

static struct msm_camera_i2c_conf_array ov8850_confs[] = {
	{&ov8850_snap_settings[0],
	ARRAY_SIZE(ov8850_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8850_prev_settings[0],
	ARRAY_SIZE(ov8850_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8850_1080video_settings[0],
	ARRAY_SIZE(ov8850_1080video_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&ov8850_zsl_settings[0],
	ARRAY_SIZE(ov8850_zsl_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t ov8850_dimensions[] = {
    {//3264 2448 23.8
        .x_output = 3264,
        .y_output = 2448,
        .line_length_pclk = 0xE17,
        .frame_length_lines = 0x9D0,
        .vt_pixel_clk = 215646660,
        .op_pixel_clk = 260000000,
        .binning_factor = 0,
    },
    {// 1632 1224 30->24
        .x_output = 1632,
        .y_output = 1224,
        .line_length_pclk = 0xd0c,
        .frame_length_lines = 0x9be,
        .vt_pixel_clk = 199919040,//216047040,
        .op_pixel_clk = 260000000,//266000000,//172800000,
        .binning_factor = 0,
    },
    {// video 1080p 30
		.x_output = 1920,
		.y_output = 1080,
		.line_length_pclk = 0xd0c,
		.frame_length_lines = 0x7cc,
		.vt_pixel_clk = 199999200,
		.op_pixel_clk = 260000000,//266000000,//172800000,
		.binning_factor = 0,
    },
    {//3264 2448 24
        .x_output = 3264,
        .y_output = 2448,
        .line_length_pclk = 0xd0c,
        .frame_length_lines = 0x9bf,//[BUGFIX]-MOD by ZL PR502025
        .vt_pixel_clk = 199999200,
        .op_pixel_clk = 260000000,
        .binning_factor = 0,
    },
};

static struct msm_sensor_output_reg_addr_t ov8850_reg_addr = {
	.x_output = 0x3808,
	.y_output = 0x380a,
	.line_length_pclk = 0x380c,
	.frame_length_lines = 0x380e,
};

static enum msm_camera_vreg_name_t ov8850_veg_seq[] = {
//case 4
    CAM_VIO,
	CAM_VDIG,
	CAM_VAF,
};

static struct msm_sensor_id_info_t ov8850_id_info = {
	.sensor_id_reg_addr = 0x300A,
	.sensor_id = 0x8850,
};

static struct msm_sensor_exp_gain_info_t ov8850_exp_gain_info = {
	.coarse_int_time_addr = 0x3500,
	.global_gain_addr = 0x350A,
	.vert_offset = 14,
};

static int32_t ov8850_write_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
		uint16_t gain, uint32_t line, int32_t luma_avg, uint16_t fgain)
{
	uint32_t fl_lines, offset;
	uint8_t int_time[3];

	fl_lines =
		(s_ctrl->curr_frame_length_lines * s_ctrl->fps_divider) / Q10;
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;
	if (line > (fl_lines - offset))
		fl_lines = line + offset;
	CDBG("ov8850_write_exp_gain: %d %d %d\n", fl_lines, gain, line);
	if(s_ctrl->curr_res!=0)//[PLATFORM]-Mod by TCTNB.ZL, 2013/02/06, for bug405223*/
		s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines, fl_lines,
		MSM_CAMERA_I2C_WORD_DATA);
	int_time[0] = line >> 12;
	int_time[1] = line >> 4;
	int_time[2] = line << 4;
	msm_camera_i2c_write_seq(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr,
		&int_time[0], 3);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr, gain,
		MSM_CAMERA_I2C_WORD_DATA);
	if(s_ctrl->curr_res!=0)//[PLATFORM]-Mod by TCTNB.ZL, 2013/02/06, for bug405223*/
		s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	return 0;
}

static void ov8850_adjust_frame_lines(struct msm_sensor_ctrl_t *s_ctrl)
{
	uint16_t cur_line = 0;
	uint16_t exp_fl_lines = 0;
	uint8_t int_time[3];
	if (s_ctrl->sensor_exp_gain_info) {
		msm_camera_i2c_read_seq(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_exp_gain_info->coarse_int_time_addr,
			&int_time[0], 3);
		cur_line = int_time[0] << 12;
		cur_line |= int_time[1] << 4;
		cur_line |= int_time[2] >> 4;

		CDBG("%s: sensor cur_line: %d\n", __func__, cur_line);
		exp_fl_lines = cur_line +
			s_ctrl->sensor_exp_gain_info->vert_offset;
		if (exp_fl_lines > s_ctrl->msm_sensor_reg->
			output_settings[s_ctrl->curr_res].frame_length_lines) {
			exp_fl_lines += (exp_fl_lines & 0x01);
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				s_ctrl->sensor_output_reg_addr->
				frame_length_lines,
				exp_fl_lines,
				MSM_CAMERA_I2C_WORD_DATA);
		}

		CDBG("%s cur_fl_lines %d, exp_fl_lines %d\n", __func__,
			s_ctrl->msm_sensor_reg->
			output_settings[s_ctrl->curr_res].frame_length_lines,
			exp_fl_lines);
	}
	return;
}

static const struct i2c_device_id ov8850_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&ov8850_s_ctrl},
	{ }
};
#if 0
int32_t ov8850_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
#if 0
	struct msm_camera_sensor_info *info = NULL;

	info = s_ctrl->sensordata;
	gpio_direction_output(info->sensor_pwd, 0);
	gpio_direction_output(info->sensor_reset, 0);
#endif
	usleep_range(10000, 11000);
	rc = msm_sensor_power_up(s_ctrl);
	if (rc < 0) {
		CDBG("%s: msm_sensor_power_up failed\n", __func__);
		return rc;
	}
#if 0
	/* turn on ldo and vreg */
	gpio_direction_output(info->sensor_pwd, 1);
	msleep(20);
	gpio_direction_output(info->sensor_reset, 1);
#endif
	msleep(40);
	return rc;
}
#endif
static struct i2c_driver ov8850_i2c_driver = {
	.id_table = ov8850_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov8850_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static const struct of_device_id ov8850_dt_match[] = {
	{.compatible = "qcom,ov8850", .data = &ov8850_s_ctrl},
	{}
};

MODULE_DEVICE_TABLE(of, ov8850_dt_match);

static struct platform_driver ov8850_platform_driver = {
	.driver = {
		.name = "qcom,ov8850",
		.owner = THIS_MODULE,
		.of_match_table = ov8850_dt_match,
	},
};

static int32_t ov8850_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	const struct of_device_id *match;

	match = of_match_device(ov8850_dt_match, &pdev->dev);
	rc = msm_sensor_platform_probe(pdev, match->data);

	return rc;
}

static int __init msm_sensor_init_module(void)
{
	int32_t rc = 0;
	rc = platform_driver_probe(&ov8850_platform_driver,
		ov8850_platform_probe);
	if (!rc)
		return rc;
	return i2c_add_driver(&ov8850_i2c_driver);
}

static void __exit msm_sensor_exit_module(void)
{
	if (ov8850_s_ctrl.pdev) {
		msm_sensor_free_sensor_data(&ov8850_s_ctrl);
		platform_driver_unregister(&ov8850_platform_driver);
	} else
		i2c_del_driver(&ov8850_i2c_driver);
	return;
}

static struct v4l2_subdev_core_ops ov8850_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops ov8850_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops ov8850_subdev_ops = {
	.core = &ov8850_subdev_core_ops,
	.video  = &ov8850_subdev_video_ops,
};
#if 0
int32_t ov8850_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res)
{
	int32_t rc = 0;

	if (update_type == MSM_SENSOR_REG_INIT) {
		CDBG("Register INIT\n");
		s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);
		msm_sensor_enable_debugfs(s_ctrl);
		msm_sensor_write_init_settings(s_ctrl);
		CDBG("Update OTP\n");
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x100, 0x1,
				MSM_CAMERA_I2C_BYTE_DATA);
		msleep(66);
		ov8850_update_otp(s_ctrl);
		usleep_range(10000, 11000);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x100, 0x0,
		  MSM_CAMERA_I2C_BYTE_DATA);
	} else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		CDBG("PERIODIC : %d\n", res);
		msm_sensor_write_conf_array(
			s_ctrl->sensor_i2c_client,
			s_ctrl->msm_sensor_reg->mode_settings, res);
		msleep(30);
		v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
			NOTIFY_PCLK_CHANGE,
			&s_ctrl->sensordata->pdata->ioclk.vfe_clk_rate);
	}
	return rc;
}
#endif
int32_t ov8850_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res)
{
	int32_t rc = 0;
	if (update_type == MSM_SENSOR_REG_INIT) {
        s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);
		msm_sensor_write_init_settings(s_ctrl);
#if OTP_ENABLE
		OV8850_TCT_OTP_calibration(s_ctrl->sensor_i2c_client);
#endif
	} else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		msm_sensor_write_res_settings(s_ctrl, res);
		v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
			NOTIFY_PCLK_CHANGE, &s_ctrl->msm_sensor_reg->
			output_settings[res].op_pixel_clk);
	}
	return rc;
}
static struct msm_sensor_fn_t ov8850_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain = ov8850_write_exp_gain,
	.sensor_write_snapshot_exp_gain = ov8850_write_exp_gain,
	.sensor_setting = ov8850_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
	.sensor_adjust_frame_lines = ov8850_adjust_frame_lines,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
};

static struct msm_sensor_reg_t ov8850_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = ov8850_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(ov8850_start_settings),
	.stop_stream_conf = ov8850_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(ov8850_stop_settings),
	.group_hold_on_conf = ov8850_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(ov8850_groupon_settings),
	.group_hold_off_conf = ov8850_groupoff_settings,
	.group_hold_off_conf_size =	ARRAY_SIZE(ov8850_groupoff_settings),
	.init_settings = &ov8850_init_conf[0],
	.init_size = ARRAY_SIZE(ov8850_init_conf),
	.mode_settings = &ov8850_confs[0],
	.output_settings = &ov8850_dimensions[0],
	.num_conf = ARRAY_SIZE(ov8850_confs),
};

static struct msm_sensor_ctrl_t ov8850_s_ctrl = {
	.msm_sensor_reg = &ov8850_regs,
	.sensor_i2c_client = &ov8850_sensor_i2c_client,
	.sensor_i2c_addr = 0x6c,
	.vreg_seq = ov8850_veg_seq,
	.num_vreg_seq = ARRAY_SIZE(ov8850_veg_seq),
	.sensor_output_reg_addr = &ov8850_reg_addr,
	.sensor_id_info = &ov8850_id_info,
	.sensor_exp_gain_info = &ov8850_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex = &ov8850_mut,
	.sensor_i2c_driver = &ov8850_i2c_driver,
	.sensor_v4l2_subdev_info = ov8850_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov8850_subdev_info),
	.sensor_v4l2_subdev_ops = &ov8850_subdev_ops,
	.func_tbl = &ov8850_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

module_init(msm_sensor_init_module);
module_exit(msm_sensor_exit_module);
MODULE_DESCRIPTION("Omnivison 8MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");
