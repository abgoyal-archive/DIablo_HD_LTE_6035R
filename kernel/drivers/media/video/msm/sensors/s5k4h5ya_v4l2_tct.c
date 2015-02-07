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
 * 13/04/23      Hu Jin       Add driver for DiabloLTE second source
 * 13/05/30		 Hu Jin		  Mod for vt01
 * 13/08/01		 Hu Jin		  OTP support
 * 13/08/04      Hu Jin       Mod OTP
 * 13/08/05      Hu Jin         support the 3rd APK(4h5)
 * 13/08/20      Hu Jin       Mod OTP
 * 13/10/24      Hu Jin       PLD fix
 * 13/10/29      Hu Jin       banding fix
 * --------------------------------------------------------------------------------------
*/
#include "msm_sensor.h"
#include "msm.h"
#ifdef CONFIG_S5K4H5YA_TCT_EEPROM
extern int32_t s5k4h5ya_tct_eeprom_init(struct msm_sensor_ctrl_t *s_ctrl);
#endif
#define SENSOR_NAME "s5k4h5ya"
#define PLATFORM_DRIVER_NAME "msm_camera_s5k4h5ya"


DEFINE_MUTEX(s5k4h5ya_mut);
static struct msm_sensor_ctrl_t s5k4h5ya_s_ctrl;

static struct msm_camera_i2c_reg_conf s5k4h5ya_start_settings[] = {
	{0x0100, 0x01,},
};

static struct msm_camera_i2c_reg_conf s5k4h5ya_stop_settings[] = {
	{0x0100, 0x00,},
};

static struct msm_camera_i2c_reg_conf s5k4h5ya_groupon_settings[] = {
	{0x0104, 0x01},
};

static struct msm_camera_i2c_reg_conf s5k4h5ya_groupoff_settings[] = {
	{0x0104, 0x00},
};

static struct msm_camera_i2c_reg_conf s5k4h5ya_reset_settings[] = {
    {0x0103, 0x01},
};

static struct msm_camera_i2c_reg_conf s5k4h5ya_snap_settings[] = {
	{0x0101, 0x03},  //image_orientation ([0] mirror en, [1] flip en)
	{0x0204, 0x00},  //analogue_gain_code_global H
	{0x0205, 0x20},  //analogue_gain_code_global L
	{0x0200, 0x0C},  //fine_integration_time H
	{0x0201, 0x78},  //fine_integration_time L
	{0x0202, 0x04},  //coarse_integration_time H
	{0x0203, 0xE2},  //coarse_integration_time L
	{0x0340, 0x09},  //  [15:8]  frame_length_lines H
	{0x0341, 0xe2},  //  [7:0]   frame_length_lines L
	{0x0342, 0x0e},  //  [15:8]  line_length_pck H
	{0x0343, 0x68},  //  [7:0]   line_length_pck L
	{0x0344, 0x00},  //  [11:8]  x_addr_start H
	{0x0345, 0x08},  //  [7:0]   x_addr_start L
	{0x0346, 0x00},  //  [11:8]  y_addr_start H
	{0x0347, 0x08},  //  [7:0]   y_addr_start L
	{0x0348, 0x0C},  //  [11:8]  x_addr_end H
	{0x0349, 0xC7},  //  [7:0]   x_addr_end L
	{0x034A, 0x09},  //  [11:8]  y_addr_end H
	{0x034B, 0x97},  //  [7:0]   y_addr_end L
	{0x034C, 0x0C},  //  [11:8]  x_output_size H
	{0x034D, 0xC0},  //  [7:0]   x_output_size L
	{0x034E, 0x09},  //  [11:8]  y_output_size H
	{0x034F, 0x90},  //  [7:0]   y_output_size L
	{0x0390, 0x00},  //binning_mode ([0] binning enable)
	{0x0391, 0x00},  //binning_type (22h : 2x2 binning, 44h : 4x4 binning)
	{0x0381, 0x01},  //x_even_inc
	{0x0383, 0x01},  //x_odd_inc
	{0x0385, 0x01},  //y_even_inc
	{0x0387, 0x01},  //y_odd_inc
	{0x0301, 0x02},  //vt_pix_clk_div
	{0x0303, 0x01},  //vt_sys_clk_div
	{0x0305, 0x06},  //pre_pll_clk_div
	{0x0306, 0x00},  //pll_multiplier H
	{0x0307, 0x8C},  //pll_multiplier L
	{0x0309, 0x02},  //op_pix_clk_div
	{0x030B, 0x01},  //op_sys_clk_div
	{0x3C59, 0x00},  //reg_PLL_S
	{0x030D, 0x06},  //out_pre_pll_clk_div
	{0x030E, 0x00},  //out_pll_multiplier H
	{0x030F, 0xA5},  //out_pll_multiplier L
	{0x3C5A, 0x00},  //reg_out_PLL_S
	{0x0310, 0x01},  //pll_mode (01h : 2-PLL, 00h : 1-PLL)
	{0x3C50, 0x53},  //reg_DIV_G /reg_DIV_DBR
	{0x3C62, 0x02},  //requested_link_bit_rate_mbps HH[31:24]
	{0x3C63, 0x94},  //requested_link_bit_rate_mbps HL[23:16]
	{0x3C64, 0x00},  //requested_link_bit_rate_mbps LH[15:8]
	{0x3C65, 0x00},  //requested_link_bit_rate_mbps LL[7:0]
	{0x3C1E, 0x00},  //reg_isp_fe_TN_SMIA_sync_sel[3]
	{0x0114, 0x03},  //mipi lanes

	// Analog Tuning Start
	// revision history
	// 2012.11.21 EVT1 initial setfile
	// 2012.12.10 dshut_en=1
	// 2012.02.25 Update for analog circuit margin
	// 2013.03.24 EVT1 H4 Power noise reduction tune(Woong Joo)


	{0x302A, 0x0A},    //vda_width
	{0x304B, 0x2A},    //ADC_SAT 490mV
	{0x3205, 0x84},    //adc_offset_odd0
	{0x3207, 0x85},    //adc_offset_even1
	{0x3214, 0x94},    //adc_offset_odd0_MS
	{0x3216, 0x95},    //adc_offset_even1_MS
	{0x303A, 0x9F},    //clp_lvl
	{0x3201, 0x07},    //dither_sel[2]: fob, dither_sel[1]: lob, dither_sel[0]: active
	{0x3051, 0xFF},    //blst
	{0x3052, 0xFF},    //blst
	{0x3054, 0xF0},    //rdv_option[7]=1 (caution of address overwrite)
	{0x302D, 0x7F},    //[4] dshut_en=1

	{0x3002, 0x0D},
	{0x300A, 0x0D},
	{0x3037, 0x12},
	{0x3045, 0x04},    //dbr_tune_rg(VRD=2.9V)
	{0x300C, 0x78},
	{0x300D, 0x80},
	{0x305C, 0x82},    //cds_option (reduce s3/s4 buffer strength, s4_rpt_enable)
	{0x3010, 0x0A},

	{0x305E, 0x11},    //MS control
	{0x305F, 0x11},    //MS control
	{0x3060, 0x10},    //MS control
	{0x3091, 0x04},    //cnt_en_ms_r_offset
	{0x3092, 0x07},    //cnt_en_ms_f_offset
	{0x303D, 0x05},    //off_rst
	{0x3038, 0x99},    //comp1/2_bias
	{0x3B29, 0x01},    //OTP enable
};


static struct v4l2_subdev_info s5k4h5ya_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array s5k4h5ya_init_conf[] = {
	{&s5k4h5ya_reset_settings[0],
	ARRAY_SIZE(s5k4h5ya_reset_settings), 5, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_camera_i2c_conf_array s5k4h5ya_confs[] = {
	{&s5k4h5ya_snap_settings[0],
	ARRAY_SIZE(s5k4h5ya_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t s5k4h5ya_dimensions[] = {
    {
		//3264 2448 30
		.x_output = 3264,
		.y_output = 2448,
		.line_length_pclk = 0x0e68,
		.frame_length_lines = 0x09e2,//30fps->0x09e3
		.vt_pixel_clk = 279919200,
		.op_pixel_clk = 300000000,
		.binning_factor = 0,
	},
};

static struct msm_sensor_output_reg_addr_t s5k4h5ya_reg_addr = {
	.x_output = 0x034C,
	.y_output = 0x034E,
	.line_length_pclk = 0x0342,
	.frame_length_lines = 0x0340,
};

static enum msm_camera_vreg_name_t s5k4h5ya_veg_seq[] = {
//case 4
    CAM_VIO,
	CAM_VDIG,
	CAM_VAF,
};

static struct msm_sensor_id_info_t s5k4h5ya_id_info = {
	.sensor_id_reg_addr = 0x0000,
	.sensor_id = 0x485B,
};

static struct msm_sensor_exp_gain_info_t s5k4h5ya_exp_gain_info = {
	.coarse_int_time_addr = 0x202,
	.global_gain_addr = 0x204,
	.vert_offset = 16,
};

static int32_t s5k4h5ya_write_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
		uint16_t gain, uint32_t line, int32_t luma_avg, uint16_t fgain)
{
	uint32_t fl_lines;
	uint8_t offset;
	fl_lines = s_ctrl->curr_frame_length_lines;
	fl_lines = (fl_lines * s_ctrl->fps_divider) / Q10;
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;
	if (line > (fl_lines - offset))
		fl_lines = line + offset;
	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines, fl_lines,
		MSM_CAMERA_I2C_WORD_DATA);

	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr, line,
		MSM_CAMERA_I2C_WORD_DATA);

	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr, gain,
		MSM_CAMERA_I2C_WORD_DATA);

	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);

	return 0;
}

static void s5k4h5ya_adjust_frame_lines(struct msm_sensor_ctrl_t *s_ctrl)
{
	uint16_t cur_line = 0;
	uint16_t exp_fl_lines = 0;
	if (s_ctrl->sensor_exp_gain_info) {
		msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_exp_gain_info->coarse_int_time_addr,
			&cur_line,
			MSM_CAMERA_I2C_WORD_DATA);
		exp_fl_lines = cur_line +
			s_ctrl->sensor_exp_gain_info->vert_offset;
		if (exp_fl_lines > s_ctrl->msm_sensor_reg->
			output_settings[s_ctrl->curr_res].frame_length_lines)
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				s_ctrl->sensor_output_reg_addr->
				frame_length_lines,
				exp_fl_lines,
				MSM_CAMERA_I2C_WORD_DATA);
		CDBG("%s cur_fl_lines %d, exp_fl_lines %d\n", __func__,
			s_ctrl->msm_sensor_reg->
			output_settings[s_ctrl->curr_res].frame_length_lines,
			exp_fl_lines);
	}
	return;
}

static const struct i2c_device_id s5k4h5ya_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&s5k4h5ya_s_ctrl},
	{ }
};
static struct i2c_driver s5k4h5ya_i2c_driver = {
	.id_table = s5k4h5ya_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client s5k4h5ya_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static const struct of_device_id s5k4h5ya_dt_match[] = {
	{.compatible = "qcom,s5k4h5ya", .data = &s5k4h5ya_s_ctrl},
	{}
};

MODULE_DEVICE_TABLE(of, s5k4h5ya_dt_match);

static struct platform_driver s5k4h5ya_platform_driver = {
	.driver = {
		.name = "qcom,s5k4h5ya",
		.owner = THIS_MODULE,
		.of_match_table = s5k4h5ya_dt_match,
	},
};

static int32_t s5k4h5ya_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	const struct of_device_id *match;

	match = of_match_device(s5k4h5ya_dt_match, &pdev->dev);
	rc = msm_sensor_platform_probe(pdev, match->data);

	return rc;
}

static int __init msm_sensor_init_module(void)
{
	int32_t rc = 0;
	rc = platform_driver_probe(&s5k4h5ya_platform_driver,
		s5k4h5ya_platform_probe);
	if (!rc)
		return rc;
	return i2c_add_driver(&s5k4h5ya_i2c_driver);
}

static void __exit msm_sensor_exit_module(void)
{
	if (s5k4h5ya_s_ctrl.pdev) {
		msm_sensor_free_sensor_data(&s5k4h5ya_s_ctrl);
		platform_driver_unregister(&s5k4h5ya_platform_driver);
	} else
		i2c_del_driver(&s5k4h5ya_i2c_driver);
	return;
}

static struct v4l2_subdev_core_ops s5k4h5ya_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops s5k4h5ya_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops s5k4h5ya_subdev_ops = {
	.core = &s5k4h5ya_subdev_core_ops,
	.video  = &s5k4h5ya_subdev_video_ops,
};
int32_t s5k4h5ya_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res)
{
	int32_t rc = 0;
	if (update_type == MSM_SENSOR_REG_INIT) {
        s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);
		msm_sensor_write_init_settings(s_ctrl);
	} else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		msm_sensor_write_res_settings(s_ctrl, res);
		v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
			NOTIFY_PCLK_CHANGE, &s_ctrl->msm_sensor_reg->
			output_settings[res].op_pixel_clk);
	}
	return rc;
}
int32_t s5k4h5ya_match_id(struct msm_sensor_ctrl_t *s_ctrl)
{
    int32_t rc = 0;
#ifdef CONFIG_S5K4H5YA_TCT_EEPROM
    static uint16_t count=0;
#endif
    rc = msm_sensor_match_id(s_ctrl);
#ifdef CONFIG_S5K4H5YA_TCT_EEPROM
    if((rc>0) && (count++ == 0)){
        s5k4h5ya_tct_eeprom_init(s_ctrl);
	}
#endif
    return rc;
}

static struct msm_sensor_fn_t s5k4h5ya_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain = s5k4h5ya_write_exp_gain,
	.sensor_write_snapshot_exp_gain = s5k4h5ya_write_exp_gain,
	.sensor_setting = s5k4h5ya_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
	.sensor_adjust_frame_lines = s5k4h5ya_adjust_frame_lines,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
	.sensor_match_id = s5k4h5ya_match_id,
};

static struct msm_sensor_reg_t s5k4h5ya_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = s5k4h5ya_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(s5k4h5ya_start_settings),
	.stop_stream_conf = s5k4h5ya_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(s5k4h5ya_stop_settings),
	.group_hold_on_conf = s5k4h5ya_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(s5k4h5ya_groupon_settings),
	.group_hold_off_conf = s5k4h5ya_groupoff_settings,
	.group_hold_off_conf_size =	ARRAY_SIZE(s5k4h5ya_groupoff_settings),
	.init_settings = &s5k4h5ya_init_conf[0],
	.init_size = ARRAY_SIZE(s5k4h5ya_init_conf),
	.mode_settings = &s5k4h5ya_confs[0],
	.output_settings = &s5k4h5ya_dimensions[0],
	.num_conf = ARRAY_SIZE(s5k4h5ya_confs),
};

static struct msm_sensor_ctrl_t s5k4h5ya_s_ctrl = {
	.msm_sensor_reg = &s5k4h5ya_regs,
	.sensor_i2c_client = &s5k4h5ya_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,//0x6c,
	.vreg_seq = s5k4h5ya_veg_seq,
	.num_vreg_seq = ARRAY_SIZE(s5k4h5ya_veg_seq),
	.sensor_output_reg_addr = &s5k4h5ya_reg_addr,
	.sensor_id_info = &s5k4h5ya_id_info,
	.sensor_exp_gain_info = &s5k4h5ya_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex = &s5k4h5ya_mut,
	.sensor_i2c_driver = &s5k4h5ya_i2c_driver,
	.sensor_v4l2_subdev_info = s5k4h5ya_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(s5k4h5ya_subdev_info),
	.sensor_v4l2_subdev_ops = &s5k4h5ya_subdev_ops,
	.func_tbl = &s5k4h5ya_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

module_init(msm_sensor_init_module);
module_exit(msm_sensor_exit_module);
MODULE_DESCRIPTION("Samsung 8MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");



