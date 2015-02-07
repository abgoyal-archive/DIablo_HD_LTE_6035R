/*
 *
 * FocalTech ft5x06 TouchScreen driver.
 *
 * Copyright (c) 2010  Focal tech Ltd.
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#endif
/*[PLATFORM]-Add-END by TCTNB.WPL*/
#include <linux/regulator/consumer.h>
#include <linux/input/ft5x06_ts.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
/* Early-suspend level */
#define FT5X06_SUSPEND_LEVEL 1
#endif

#define CFG_MAX_TOUCH_POINTS	5

/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2013/01/17, from FAE: should change 150 to 200*/
#define FT_STARTUP_DLY		200
/*[PLATFORM]-Mod-END by TCTNB.WPL*/

#define FT_RESET_DLY		20
#define FT_PRESS		0x7F
#define FT_MAX_ID		0x0F
#define FT_TOUCH_STEP		6
#define FT_TOUCH_X_H_POS	3
#define FT_TOUCH_X_L_POS	4
#define FT_TOUCH_Y_H_POS	5
#define FT_TOUCH_Y_L_POS	6
#define FT_TOUCH_EVENT_POS	3
#define FT_TOUCH_ID_POS		5

#define POINT_READ_BUF	(3 + FT_TOUCH_STEP * CFG_MAX_TOUCH_POINTS)

/*register address*/
#define FT5X06_REG_PMODE	0xA5
#define FT5X06_REG_FW_VER	0xA6
#define FT5X06_REG_POINT_RATE	0x88
#define FT5X06_REG_THGROUP	0x80
/*[PLATFORM]-Add-BEGIN by TCTNB.WQF, 2013/6/19, Add calibration function*/
#define FT5X06_REG_CAL	0xB6
/*[PLATFORM]-Add-END by TCTNB.WQF*/

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE
#define FT5X06_REG_GESTURE_SET    0x96
#define FT5X06_REG_GESTURE_STATE    0x01
#endif
/*[PLATFORM]-Add-END by TCTNB.WPL*/

/* power register bits*/
#define FT5X06_PMODE_ACTIVE		0x00
#define FT5X06_PMODE_MONITOR		0x01
#define FT5X06_PMODE_STANDBY		0x02
#define FT5X06_PMODE_HIBERNATE		0x03

#define FT5X06_VTG_MIN_UV	2600000
#define FT5X06_VTG_MAX_UV	3300000
#define FT5X06_I2C_VTG_MIN_UV	1800000
#define FT5X06_I2C_VTG_MAX_UV	1800000


/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2013/01/08, refer to bug378099*/
#ifdef CONFIG_TOUCHSCREEN_FT5X06_FIRMWARE
static struct class *firmware_class;
static struct device *firmware_cmd_dev;
static struct i2c_client *firmware_client;
static uint8_t newest_firmware_version=0xff;

int TP_VENDOR = 0;
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/

struct ts_event {
	u16 x[CFG_MAX_TOUCH_POINTS];	/*x coordinate */
	u16 y[CFG_MAX_TOUCH_POINTS];	/*y coordinate */
	/* touch event: 0 -- down; 1-- contact; 2 -- contact */
	u8 touch_event[CFG_MAX_TOUCH_POINTS];
	u8 finger_id[CFG_MAX_TOUCH_POINTS];	/*touch ID */
	u16 pressure;
	u8 touch_point;
};

struct ft5x06_ts_data {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct ts_event event;
	const struct ft5x06_ts_platform_data *pdata;
	struct regulator *vdd;
	struct regulator *vcc_i2c;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif
};

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE

#ifdef DEBUG_TP_GESTURE
static int GestureINTCount=0;
static int GestureConfirmedCount=0;
#define TP_GESTURE_DBG(fmt, arg...) printk(KERN_ERR "[TP_GSTURE] %s: " fmt "\n", __func__, ## arg)
#else
#define TP_GESTURE_DBG(mt, arg...) do {} while (0)
#endif

struct tp_gesture_data{
    u8 option;
    u8 flag;
    u8 stateReg_val;
};

static struct  tp_gesture_data gesture={
    .option=0x00,
    .flag=0x00,
    .stateReg_val=0x00,
 };

static long tp_ioctl_control(struct file *filep, unsigned int cmd,unsigned long arg)
{
    int rc = 0;

    TP_GESTURE_DBG("=>tp_ioctl_control\n");

    switch (cmd) {
    case TPWAKEUP_IOCTL_OFF:
    case 0:
        gesture.option = 0x00;
        TP_GESTURE_DBG("=>gesture.option= %d\n",gesture.option);
        break;
	case TPWAKEUP_IOCTL_ON:
	case 1:
        gesture.option = 0x01;
        TP_GESTURE_DBG("=>gesture.option= %d\n",gesture.option);
        break;
    default:
        rc = -EINVAL;
        pr_err("invalid  command! \n");
        break;
    }

    return rc;
}

static const struct file_operations tp_fops_control = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl	= tp_ioctl_control,
	};

struct miscdevice tpwakeup_misc = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "tpwakeup",
	.fops	= &tp_fops_control,
};

#endif
/*[PLATFORM]-Add-END by TCTNB.WPL*/

//[PLATFORM]-Mod by TCTNB.WPL, 2013/01/10, add the TP ft5316 firmware upgrade function, refer to bug386588
#ifdef CONFIG_TOUCHSCREEN_FT5X06_FIRMWARE
int ft5x06_i2c_read(struct i2c_client *client, char *writebuf,
			   int writelen, char *readbuf, int readlen)
{
	int ret;

	if (writelen > 0) {
		struct i2c_msg msgs[] = {
			{
				 .addr = client->addr,
				 .flags = 0,
				 .len = writelen,
				 .buf = writebuf,
			 },
			{
				 .addr = client->addr,
				 .flags = I2C_M_RD,
				 .len = readlen,
				 .buf = readbuf,
			 },
		};
		ret = i2c_transfer(client->adapter, msgs, 2);
		if (ret < 0)
			dev_err(&client->dev, "%s: i2c read error.\n",
				__func__);
	} else {
		struct i2c_msg msgs[] = {
			{
				 .addr = client->addr,
				 .flags = I2C_M_RD,
				 .len = readlen,
				 .buf = readbuf,
			 },
		};
		ret = i2c_transfer(client->adapter, msgs, 1);
		if (ret < 0)
			dev_err(&client->dev, "%s:i2c read error.\n", __func__);
	}
	return ret;
}
#endif
//[PLATFORM]-Mod by TCTNB.WPL, 2013/01/10, add the TP ft5316 firmware upgrade function, refer to bug386588

int ft5x06_i2c_write(struct i2c_client *client, char *writebuf,
			    int writelen)
{
	int ret;

	struct i2c_msg msgs[] = {
		{
			 .addr = client->addr,
			 .flags = 0,
			 .len = writelen,
			 .buf = writebuf,
		 },
	};
	ret = i2c_transfer(client->adapter, msgs, 1);
	if (ret < 0)
		dev_err(&client->dev, "%s: i2c write error.\n", __func__);

	return ret;
}

static void ft5x06_report_value(struct ft5x06_ts_data *data)
{
	struct ts_event *event = &data->event;
	int i;
	int fingerdown = 0;

	for (i = 0; i < event->touch_point; i++) {
		if (event->touch_event[i] == 0 || event->touch_event[i] == 2) {
			event->pressure = FT_PRESS;
			fingerdown++;
		} else {
			event->pressure = 0;
		}

		input_report_abs(data->input_dev, ABS_MT_POSITION_X,
				 event->x[i]);
		input_report_abs(data->input_dev, ABS_MT_POSITION_Y,
				 event->y[i]);
		input_report_abs(data->input_dev, ABS_MT_PRESSURE,
				 event->pressure);
		input_report_abs(data->input_dev, ABS_MT_TRACKING_ID,
				 event->finger_id[i]);
		input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR,
				 event->pressure);
		input_mt_sync(data->input_dev);
	}

	input_report_key(data->input_dev, BTN_TOUCH, !!fingerdown);
	input_sync(data->input_dev);
}

static int ft5x06_handle_touchdata(struct ft5x06_ts_data *data)
{
	struct ts_event *event = &data->event;
	int ret, i;
	u8 buf[POINT_READ_BUF] = { 0 };
	u8 pointid = FT_MAX_ID;

	ret = ft5x06_i2c_read(data->client, buf, 1, buf, POINT_READ_BUF);
	if (ret < 0) {
		dev_err(&data->client->dev, "%s read touchdata failed.\n",
			__func__);
		return ret;
	}
	memset(event, 0, sizeof(struct ts_event));

	event->touch_point = 0;
	for (i = 0; i < CFG_MAX_TOUCH_POINTS; i++) {
		pointid = (buf[FT_TOUCH_ID_POS + FT_TOUCH_STEP * i]) >> 4;
		if (pointid >= FT_MAX_ID)
			break;
		else
			event->touch_point++;
		event->x[i] =
		    (s16) (buf[FT_TOUCH_X_H_POS + FT_TOUCH_STEP * i] & 0x0F) <<
		    8 | (s16) buf[FT_TOUCH_X_L_POS + FT_TOUCH_STEP * i];
		event->y[i] =
		    (s16) (buf[FT_TOUCH_Y_H_POS + FT_TOUCH_STEP * i] & 0x0F) <<
		    8 | (s16) buf[FT_TOUCH_Y_L_POS + FT_TOUCH_STEP * i];
		event->touch_event[i] =
		    buf[FT_TOUCH_EVENT_POS + FT_TOUCH_STEP * i] >> 6;
		event->finger_id[i] =
		    (buf[FT_TOUCH_ID_POS + FT_TOUCH_STEP * i]) >> 4;
	}

	ft5x06_report_value(data);

	return 0;
}

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE
static irqreturn_t ft5x06_ts_interrupt(int irq, void *dev_id)
{
    struct ft5x06_ts_data *data = dev_id;
    int rc;
    u8 reg_value;
    u8 reg_addr;
    int err;

    if((0x01==gesture.option)&&(0x01==gesture.flag))//gesture INT
    {
#ifdef DEBUG_TP_GESTURE
        GestureINTCount++;
        TP_GESTURE_DBG("GestureINTCount = %d\n",GestureINTCount);
#endif
        //msleep(5);
        reg_addr = FT5X06_REG_GESTURE_STATE;
        err = ft5x06_i2c_read(data->client, &reg_addr, 1, &reg_value, 1);
        if((GESTURE_DB == reg_value)||(GESTURE_V == reg_value)||(GESTURE_C == reg_value))
        {
            gesture.stateReg_val=reg_value;
            input_report_key(data->input_dev, KEY_POWER, 1);
            input_report_key(data->input_dev, KEY_POWER, 0);
            input_sync(data->input_dev);
#ifdef DEBUG_TP_GESTURE
            GestureConfirmedCount++;
            TP_GESTURE_DBG("GestureConfirmedCount = %d\n",GestureConfirmedCount);
            TP_GESTURE_DBG("reg_addr 0x01= 0x%04x\n",reg_value);
#endif
        }
        else
        {
            pr_err("=>error  reg_addr 0x01= 0x%04x\n",reg_value);
        }
    }
    else//data INT
    {
        rc = ft5x06_handle_touchdata(data);
        if (rc)
            pr_err("%s: handling touchdata failed\n", __func__);
    }

    return IRQ_HANDLED;
}
#else
static irqreturn_t ft5x06_ts_interrupt(int irq, void *dev_id)
{
	struct ft5x06_ts_data *data = dev_id;
	int rc;

	rc = ft5x06_handle_touchdata(data);
	if (rc)
		pr_err("%s: handling touchdata failed\n", __func__);

	return IRQ_HANDLED;
}
#endif
/*[PLATFORM]-Add-END by TCTNB.WPL*/

static int ft5x06_power_on(struct ft5x06_ts_data *data, bool on)
{
	int rc;

	if (!on)
		goto power_off;

/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2013/01/07, refer to bug380039*/
#ifdef CONFIG_TCT_8X30_COMMON
	rc = regulator_enable(data->vcc_i2c);
	if (rc) {
		dev_err(&data->client->dev,
			"Regulator vcc_i2c enable failed rc=%d\n", rc);
		regulator_disable(data->vdd);
	}

	rc = regulator_enable(data->vdd);
	if (rc) {
		dev_err(&data->client->dev,
			"Regulator vdd enable failed rc=%d\n", rc);
		return rc;
	}

	return rc;
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/

power_off:
	rc = regulator_disable(data->vdd);
	if (rc) {
		dev_err(&data->client->dev,
			"Regulator vdd disable failed rc=%d\n", rc);
		return rc;
	}

	rc = regulator_disable(data->vcc_i2c);
	if (rc) {
		dev_err(&data->client->dev,
			"Regulator vcc_i2c disable failed rc=%d\n", rc);
		regulator_enable(data->vdd);
	}

	return rc;
}

static int ft5x06_power_init(struct ft5x06_ts_data *data, bool on)
{
	int rc;

	if (!on)
		goto pwr_deinit;

/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2012/10/15, for viper TP, refer to bug318601*/
#ifdef CONFIG_TCT_8X30_COMMON
	data->vdd = regulator_get(&data->client->dev, "vdd_ana");
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/

	if (IS_ERR(data->vdd)) {
		rc = PTR_ERR(data->vdd);
		dev_err(&data->client->dev,
			"Regulator get failed vdd rc=%d\n", rc);
		return rc;
	}

	if (regulator_count_voltages(data->vdd) > 0) {
		rc = regulator_set_voltage(data->vdd, FT5X06_VTG_MIN_UV,
					   FT5X06_VTG_MAX_UV);
		if (rc) {
			dev_err(&data->client->dev,
				"Regulator set_vtg failed vdd rc=%d\n", rc);
			goto reg_vdd_put;
		}
	}

	data->vcc_i2c = regulator_get(&data->client->dev, "vcc_i2c");
	if (IS_ERR(data->vcc_i2c)) {
		rc = PTR_ERR(data->vcc_i2c);
		dev_err(&data->client->dev,
			"Regulator get failed vcc_i2c rc=%d\n", rc);
		goto reg_vdd_set_vtg;
	}

	if (regulator_count_voltages(data->vcc_i2c) > 0) {
		rc = regulator_set_voltage(data->vcc_i2c, FT5X06_I2C_VTG_MIN_UV,
					   FT5X06_I2C_VTG_MAX_UV);
		if (rc) {
			dev_err(&data->client->dev,
			"Regulator set_vtg failed vcc_i2c rc=%d\n", rc);
			goto reg_vcc_i2c_put;
		}
	}

	return 0;

reg_vcc_i2c_put:
	regulator_put(data->vcc_i2c);
reg_vdd_set_vtg:
	if (regulator_count_voltages(data->vdd) > 0)
		regulator_set_voltage(data->vdd, 0, FT5X06_VTG_MAX_UV);
reg_vdd_put:
	regulator_put(data->vdd);
	return rc;

pwr_deinit:
	if (regulator_count_voltages(data->vdd) > 0)
		regulator_set_voltage(data->vdd, 0, FT5X06_VTG_MAX_UV);

	regulator_put(data->vdd);

	if (regulator_count_voltages(data->vcc_i2c) > 0)
		regulator_set_voltage(data->vcc_i2c, 0, FT5X06_I2C_VTG_MAX_UV);

	regulator_put(data->vcc_i2c);
	return 0;
}

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/01/11, Add control for USB instert/remove, refer to bug386091*/
#if 0
extern u32 get_chg_status_for_tp(void);
static struct ft5x06_ts_data *internal_drdata;
static int g_ts_is_suspend = false;
void set_tp_status_for_chg(int enable)
{
    char txbuf[2];

    if (!g_ts_is_suspend)
    {
        txbuf[0] = 0x8b;
        txbuf[1] = enable;
        ft5x06_i2c_write(internal_drdata->client, txbuf, sizeof(txbuf));
        pr_debug("---set TP register 0x8b=%d\n", enable);
    }
    return;
}
#endif
/*[PLATFORM]-Add-END by TCTNB.WPL*/

#ifdef CONFIG_PM
/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE
static struct device dev_DoubleClickWakeup;
static int ft5x06_ts_suspend_DoubleClickWakeup(struct device *dev)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);
    char txbuf[2];
    int irq_wakeup;

    TP_GESTURE_DBG("=>-ft5x06_ts_suspend - START, gesture option=<%d>\n", gesture.option);

    if (0x01==gesture.option)
    {
        gesture.flag=0x01;
        txbuf[0] = FT5X06_REG_GESTURE_SET;
        txbuf[1] = 0x55;// enable tp gesture
        ft5x06_i2c_write(data->client, txbuf, sizeof(txbuf));
        gesture.flag=0x01;
        TP_GESTURE_DBG("=>enter gesture mode ok!! \n\n");

        if (device_may_wakeup(dev))
        {
            irq_wakeup=enable_irq_wake(data->client->irq);
            TP_GESTURE_DBG("=> enable_irq_wake  data->client->irq= %d \n",data->client->irq);
            TP_GESTURE_DBG("=> enable_irq_wake return= %d \n",irq_wakeup);
        }
    }
    else
    {
        disable_irq(data->client->irq);
        if (gpio_is_valid(data->pdata->reset_gpio))
        {
            txbuf[0] = FT5X06_REG_PMODE;
            txbuf[1] = FT5X06_PMODE_HIBERNATE;
            ft5x06_i2c_write(data->client, txbuf, sizeof(txbuf));
        }
    }

    TP_GESTURE_DBG("=>ft5x06_ts_suspend - END-\n\n");

    return 0;
}

static int ft5x06_ts_resume_DoubleClickWakeup(struct device *dev)
{
    struct ft5x06_ts_data *data = dev_get_drvdata(dev);
    char txbuf[2];

    TP_GESTURE_DBG("=>ft5x06_ts_resume - START, option=<%d>\n", gesture.option);

    if(0x01==gesture.option)
    {
        gesture.flag=0x00;
        txbuf[0] = FT5X06_REG_GESTURE_SET;
        txbuf[1] = 0x00; //disable gesture
        ft5x06_i2c_write(data->client, txbuf, sizeof(txbuf));
        gesture.flag = 0x00;//clean flag
        TP_GESTURE_DBG("=>exit gesture mode!Set flag =%d \n",gesture.flag);

    }
    else
    {
        if (gpio_is_valid(data->pdata->reset_gpio))
        {
            gpio_set_value_cansleep(data->pdata->reset_gpio, 0);
            msleep(FT_RESET_DLY);
            gpio_set_value_cansleep(data->pdata->reset_gpio, 1);
        }
    }

    TP_GESTURE_DBG("=>ft5x06_ts_resume - END\n");

    return 0;
}

#endif
/*[PLATFORM]-Add-END by TCTNB.WPL*/

static int ft5x06_ts_suspend(struct device *dev)
{
/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE
	return ft5x06_ts_suspend_DoubleClickWakeup(dev);
#else
	struct ft5x06_ts_data *data = dev_get_drvdata(dev);
	char txbuf[2];

/*[PLATFORM]-Add-BEGIN by TCTNB.WQF, 2013/07/11, record the status of TP*/
#if 0
	g_ts_is_suspend = true;
#endif
/*[PLATFORM]-Add-END by TCTNB.WQF*/

	disable_irq(data->client->irq);

	if (gpio_is_valid(data->pdata->reset_gpio)) {
		txbuf[0] = FT5X06_REG_PMODE;
		txbuf[1] = FT5X06_PMODE_HIBERNATE;
		ft5x06_i2c_write(data->client, txbuf, sizeof(txbuf));
	}

	return 0;
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/
}

static int ft5x06_ts_resume(struct device *dev)
{
/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE
    dev_DoubleClickWakeup = *dev;
    ft5x06_ts_resume_DoubleClickWakeup(&dev_DoubleClickWakeup);
#else
/*[PLATFORM]-Mod-BEGIN by TCTNB.YJ, 2013/07/18, refer to PR-490701 */
	struct ft5x06_ts_data *data = dev_get_drvdata(dev);
	if (gpio_is_valid(data->pdata->reset_gpio)) {
		gpio_set_value_cansleep(data->pdata->reset_gpio, 0);
		msleep(FT_RESET_DLY);
		gpio_set_value_cansleep(data->pdata->reset_gpio, 1);
	}

/*[PLATFORM]-Mod-BEGIN by TCTNB.YJ, 2013/07/18, refer to PR-490701 */
#if 0
	enable_irq(data->client->irq);

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/01/10, from FAE, ensure 150ms before wirte TP register, refer to bug386588*/
	msleep(FT_STARTUP_DLY);
/*[PLATFORM]-Add-END by TCTNB.WPL*/

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/01/11, Add control for USB instert/remove, refer to bug386091*/
#ifdef CONFIG_TCT_8X30_COMMON
    g_ts_is_suspend = false;
    if (get_chg_status_for_tp() == 0)
    {
        set_tp_status_for_chg(false);
    } else {
        set_tp_status_for_chg(true);
    }
#endif
/*[PLATFORM]-Add-END by TCTNB.WPL*/
#endif
/*[PLATFORM]-Mod-END by TCTNB.YJ*/
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/
	return 0;
}

/*[PLATFORM]-Mod-BEGIN by TCTNB.YJ, 2013/07/19, refer to PR-490701 */
#ifdef CONFIG_TCT_8X30_COMMON
static int g_FT5X06_IRQ;
void ft5x06_ts_enable_irq_by_lcd(void)
{
/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE
    int irq_wakeup;

    if(0x01==gesture.option){
        if (device_may_wakeup(&dev_DoubleClickWakeup))
        {
            irq_wakeup=disable_irq_wake(g_FT5X06_IRQ);
            TP_GESTURE_DBG("=> disable_irq_wake irq= %d ; return=%d\n",g_FT5X06_IRQ,irq_wakeup);
        }
    }
    else
        enable_irq(g_FT5X06_IRQ);
#else
    enable_irq(g_FT5X06_IRQ);
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/
}
#endif
/*[PLATFORM]-Mod-END by TCTNB.YJ*/

#ifdef CONFIG_HAS_EARLYSUSPEND
static void ft5x06_ts_early_suspend(struct early_suspend *handler)
{
	struct ft5x06_ts_data *data = container_of(handler,
						   struct ft5x06_ts_data,
						   early_suspend);

	ft5x06_ts_suspend(&data->client->dev);
}

static void ft5x06_ts_late_resume(struct early_suspend *handler)
{
	struct ft5x06_ts_data *data = container_of(handler,
						   struct ft5x06_ts_data,
						   early_suspend);

	ft5x06_ts_resume(&data->client->dev);
}
#endif

static const struct dev_pm_ops ft5x06_ts_pm_ops = {
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend = ft5x06_ts_suspend,
	.resume = ft5x06_ts_resume,
#endif
};
#endif

/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2013/01/08, refer to bug378099*/
#ifdef CONFIG_TOUCHSCREEN_FT5X06_FIRMWARE
static ssize_t firmware_version_show(struct device *dev,
                                     struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%0x\n", newest_firmware_version);
}

static DEVICE_ATTR(version, 0444, firmware_version_show, NULL);
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/

static int ft5x06_ts_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	const struct ft5x06_ts_platform_data *pdata = client->dev.platform_data;
	struct ft5x06_ts_data *data;
	struct input_dev *input_dev;
	u8 reg_value;
	u8 TP_FW_VERSION;
	u8 reg_addr;
	int err;
/*[PLATFORM]-Add-BEGIN by TCTNB.WQF, 2013/6/19, Add calibration function*/
#ifdef CONFIG_TCT_8X30_COMMON
	char txbuf[2];
#endif
/*[PLATFORM]-Add-END by TCTNB.WQF*/

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/01/11, add the TP ft5316 firmware upgrade function, refer to bug386588*/
#ifdef CONFIG_TOUCHSCREEN_FT5X06_FIRMWARE
	firmware_client = client;
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/

	if (!pdata) {
		dev_err(&client->dev, "Invalid pdata\n");
		return -EINVAL;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "I2C not supported\n");
		return -ENODEV;
	}

	data = kzalloc(sizeof(struct ft5x06_ts_data), GFP_KERNEL);
	if (!data) {
		dev_err(&client->dev, "Not enough memory\n");
		return -ENOMEM;
	}

	input_dev = input_allocate_device();
	if (!input_dev) {
		err = -ENOMEM;
		dev_err(&client->dev, "failed to allocate input device\n");
		goto free_mem;
	}

	data->input_dev = input_dev;
	data->client = client;
	data->pdata = pdata;

	input_dev->name = "ft5x06_ts";
	input_dev->id.bustype = BUS_I2C;
	input_dev->dev.parent = &client->dev;

	input_set_drvdata(input_dev, data);
	i2c_set_clientdata(client, data);

	__set_bit(EV_KEY, input_dev->evbit);
	__set_bit(EV_ABS, input_dev->evbit);
	__set_bit(BTN_TOUCH, input_dev->keybit);
	__set_bit(INPUT_PROP_DIRECT, input_dev->propbit);

	input_set_abs_params(input_dev, ABS_MT_POSITION_X, 0,
			     pdata->x_max, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y, 0,
			     pdata->y_max, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_TRACKING_ID, 0,
			     CFG_MAX_TOUCH_POINTS, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, FT_PRESS, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_PRESSURE, 0, FT_PRESS, 0, 0);

	err = input_register_device(input_dev);
	if (err) {
		dev_err(&client->dev, "Input device registration failed\n");
		goto free_inputdev;
	}

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/01/10, from FAE, modify the power on timing, refer to bug386588*/
#ifdef CONFIG_TCT_8X30_COMMON
	if (gpio_is_valid(pdata->wakeup_gpio)) {
		err = gpio_request(pdata->wakeup_gpio, "ft5x06_wakeup_gpio");
		if (err) {
			dev_err(&client->dev, "wakeup gpio request failed");
			goto unreg_inputdev;
		}
		err = gpio_direction_output(pdata->wakeup_gpio, 1);
		if (err) {
			dev_err(&client->dev,
				"set_direction for wakeup gpio failed\n");
			goto unreg_inputdev;
		}
	}

	if (gpio_is_valid(pdata->reset_gpio)) {
		err = gpio_request(pdata->reset_gpio, "ft5x06_reset_gpio");
		if (err) {
			dev_err(&client->dev, "reset gpio request failed");
			goto unreg_inputdev;
		}

		err = gpio_direction_output(pdata->reset_gpio, 0);
		if (err) {
			dev_err(&client->dev,
				"set_direction for reset gpio failed\n");
			goto unreg_inputdev;
		}
		msleep(1);
//		msleep(FT_RESET_DLY);
//		gpio_set_value_cansleep(data->pdata->reset_gpio, 1);
	}
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/

	if (pdata->power_init) {
		err = pdata->power_init(true);
		if (err) {
			dev_err(&client->dev, "power init failed");
			goto unreg_inputdev;
		}
	} else {
		err = ft5x06_power_init(data, true);
		if (err) {
			dev_err(&client->dev, "power init failed");
			goto unreg_inputdev;
		}
	}

	if (pdata->power_on) {
		err = pdata->power_on(true);
		if (err) {
			dev_err(&client->dev, "power on failed");
			goto pwr_deinit;
		}
	} else {
		err = ft5x06_power_on(data, true);
		if (err) {
			dev_err(&client->dev, "power on failed");
			goto pwr_deinit;
		}
	}

	if (gpio_is_valid(pdata->irq_gpio)) {
		err = gpio_request(pdata->irq_gpio, "ft5x06_irq_gpio");
		if (err) {
			dev_err(&client->dev, "irq gpio request failed");
			goto pwr_off;
		}
		err = gpio_direction_input(pdata->irq_gpio);
		if (err) {
			dev_err(&client->dev,
				"set_direction for irq gpio failed\n");
			goto free_irq_gpio;
		}
	}


/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2013/01/10, from FAE, modify the power on timing, refer to bug386588*/
#if 0
	if (gpio_is_valid(pdata->reset_gpio)) {
		err = gpio_request(pdata->reset_gpio, "ft5x06_reset_gpio");
		if (err) {
			dev_err(&client->dev, "reset gpio request failed");
			goto pwr_off;
		}

		err = gpio_direction_output(pdata->reset_gpio, 0);
		if (err) {
			dev_err(&client->dev,
				"set_direction for reset gpio failed\n");
			goto free_reset_gpio;
		}
		msleep(FT_RESET_DLY);
		gpio_set_value_cansleep(data->pdata->reset_gpio, 1);
	}
#endif
	msleep(1);
	gpio_set_value_cansleep(data->pdata->reset_gpio, 1);
/*[PLATFORM]-Mod-END by TCTNB.WPL*/

	/* make sure CTP already finish startup process */
	msleep(FT_STARTUP_DLY);

	/*get some register information */
	reg_addr = FT5X06_REG_FW_VER;
	err = ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
	if (err < 0) {  //[PLATFORM]-Mod by TCTNB.WQF, 2012/8/21, Check right return walue for scribe5
		dev_err(&client->dev, "version read failed");
		goto free_reset_gpio;
	} else {
		newest_firmware_version = reg_value;
		dev_info(&client->dev, "[FTS] Firmware version = 0x%x\n", reg_value);
		TP_FW_VERSION = reg_value;
	}

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/01/10, add the TP ft5316 firmware upgrade function, refer to bug386588*/
#ifdef CONFIG_TOUCHSCREEN_FT5X06_FIRMWARE
	TP_VENDOR = fts_ctpm_update_project_setting(client);
	dev_info(&client->dev, "[FTS] bootloader vendor ID=0x%x\n", TP_VENDOR);
	if(TP_VENDOR == IS_MUTTO_TP){
		printk("vendor: MUTTO TP\n");
		if(TP_FW_VERSION < 0x12){
			printk("ft5316 MUTTO FW upgrade begin...\n");
			err = fts_ctpm_fw_upgrade_with_i_file(client);         //upgrade firmware
			printk("ft5316 MUTTO FW upgrade end...\n");
			if (err != 0){
				printk(KERN_ERR "MUTTO ft5x06 upgrade failed\n");
				goto free_reset_gpio;
			}

			reg_addr = FT5X06_REG_FW_VER;
			err = ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
			if (err < 0) {
				dev_err(&client->dev, "version read failed");
				goto free_reset_gpio;
			} else {
				newest_firmware_version = reg_value;
				dev_info(&client->dev, "new [FTS] MUTTO Firmware version = 0x%x\n\n", reg_value);
			}
		}
	      /***************upgrade tp end**************/
	}else if(TP_VENDOR == IS_JUNDA_TP){
		printk("vendor: JUNDA TP\n");
		if(TP_FW_VERSION < 0xc){
			printk("ft5316 JUNDA FW upgrade begin...\n");
			err = fts_ctpm_fw_upgrade_with_i_file(client);         //upgrade firmware
			printk("ft5316 JUNDA FW upgrade end...\n\n");
			if (err != 0){
				printk(KERN_ERR "JUNDA ft5x06 upgrade failed\n");
				goto free_reset_gpio;
			}

			reg_addr = FT5X06_REG_FW_VER;
			err = ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
			if (err < 0) {
				dev_err(&client->dev, "version read failed");
				goto free_reset_gpio;
			} else {
				newest_firmware_version = reg_value;
				dev_info(&client->dev, "new [FTS] JUNDA Firmware version = 0x%x\n\n", reg_value);
			}
		}
	      /***************upgrade tp end**************/
	}else if (TP_VENDOR == IS_OGS_TP){
		printk("vendor: OGS TP & diabo 1st TP\n");
/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE
		if (TP_FW_VERSION < 0xf4){
#else
		if (TP_FW_VERSION < 0x14){
#endif
			printk("ft5316 OGS FW upgrade begin...\n");
			err = fts_ctpm_fw_upgrade_with_i_file(client);         //upgrade firmware
			printk("ft5316 OGS FW upgrade end...\n\n");
			if (err != 0){
				printk(KERN_ERR "OGS ft5x06 upgrade failed\n");
				goto free_reset_gpio;
			}

			reg_addr = FT5X06_REG_FW_VER;
			err = ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
			if (err < 0) {
				dev_err(&client->dev, "version read failed");
				goto free_reset_gpio;
			} else {
				newest_firmware_version = reg_value;
				dev_info(&client->dev, "new [FTS] OGS Firmware version = 0x%x\n\n", reg_value);
			}
		}
	}else if(TP_VENDOR == IS_TDT_TP){
		printk("vendor: TDT TP & diabo 2rd TP\n");
		if (TP_FW_VERSION != 0x10){
			printk("ft5316 TDT FW upgrade begin...\n");
			err = fts_ctpm_fw_upgrade_with_i_file(client);         //upgrade firmware
			printk("ft5316 TDT FW upgrade end...\n\n");
			if (err != 0){
				printk(KERN_ERR "TDT ft5x06 upgrade failed\n");
				goto free_reset_gpio;
			}

			reg_addr = FT5X06_REG_FW_VER;
			err = ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
			if (err < 0) {
				dev_err(&client->dev, "version read failed");
				goto free_reset_gpio;
			} else {
				newest_firmware_version = reg_value;
				dev_info(&client->dev, "new [FTS] TDT Firmware version = 0x%x\n\n", reg_value);
			}
		}
	}else{
		printk("\n\nUNKNOWN TP, can't upgrade FW\n\n");
	}
#endif
/*[PLATFORM]-Add-END by TCTNB.WPL*/

/*[PLATFORM]-Add-BEGIN by TCTNB.WQF, 2013/6/19, Add calibration function*/
#ifdef CONFIG_TCT_8X30_COMMON
	reg_addr = FT5X06_REG_CAL;
	ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
	if (reg_value != 0xaa) {
		txbuf[0] = FT5X06_REG_CAL;
		txbuf[1] = 0xaa;
		ft5x06_i2c_write(client, txbuf, sizeof(txbuf));
		fts_ctpm_auto_clb(client);  //start auto CLB
		printk("ft5x06 calibration executed\n");
	}
#endif
/*[PLATFORM]-Add-END by TCTNB.WQF*/

	reg_addr = FT5X06_REG_POINT_RATE;
	ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
	if (err < 0)  //[PLATFORM]-Mod by TCTNB.WQF, 2012/8/21, Check right return walue for scribe5
		dev_err(&client->dev, "report rate read failed");
	dev_info(&client->dev, "[FTS] report rate is %dHz.\n", reg_value * 10);

	reg_addr = FT5X06_REG_THGROUP;
	err = ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
	if (err < 0)  //[PLATFORM]-Mod by TCTNB.WQF, 2012/8/21, Check right return walue for scribe5
		dev_err(&client->dev, "threshold read failed");
	dev_dbg(&client->dev, "[FTS] touch threshold is %d.\n", reg_value * 4);

	err = request_threaded_irq(client->irq, NULL,
				   ft5x06_ts_interrupt, pdata->irqflags,
				   client->dev.driver->name, data);
	if (err) {
		dev_err(&client->dev, "request irq failed\n");
		goto free_reset_gpio;
	}

/*[PLATFORM]-Add-BEGIN by TCTNB.WQF, 2013/7/30, Save irq number*/
#ifdef CONFIG_TCT_8X30_COMMON
	g_FT5X06_IRQ = client->irq;
#endif
/*[PLATFORM]-Add-END by TCTNB.WQF*/

/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE
    device_init_wakeup(&client->dev, 1);
    input_set_capability(data->input_dev, EV_KEY, KEY_POWER);
    err = misc_register(&tpwakeup_misc);
    if (err) {
        dev_err(&client->dev, "tpwakeup_misc  register failed\n");
        goto free_irq_data;
    }
#endif

/*[PLATFORM]-Add-END by TCTNB.WPL*/
#ifdef CONFIG_HAS_EARLYSUSPEND
	data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN +
	    FT5X06_SUSPEND_LEVEL;
	data->early_suspend.suspend = ft5x06_ts_early_suspend;
	data->early_suspend.resume = ft5x06_ts_late_resume;
	register_early_suspend(&data->early_suspend);
#endif

/*[PLATFORM]-Mod-BEGIN by TCTNB.WPL, 2013/01/08, refer to bug378099*/
#ifdef CONFIG_TOUCHSCREEN_FT5X06_FIRMWARE
	firmware_class = class_create(THIS_MODULE, "touchscreen-ft5316");
    if (IS_ERR(firmware_class))
        pr_err("Failed to create class(firmware)!\n");
    firmware_cmd_dev = device_create(firmware_class,
                                     NULL, 0, NULL, "device");
    if (IS_ERR(firmware_cmd_dev))
        pr_err("Failed to create device(firmware_cmd_dev)!\n");

    // version
    if (device_create_file(firmware_cmd_dev, &dev_attr_version) < 0)
        pr_err("Failed to create device file(%s)!\n", dev_attr_version.attr.name);

	dev_set_drvdata(firmware_cmd_dev, NULL);
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/01/10, add the TP ft5316 firmware upgrade function, refer to bug386588*/
#ifdef CONFIG_TOUCHSCREEN_FT5X06_FIRMWARE
#ifdef SYSFS_DEBUG
	ft5x0x_create_sysfs(client);
#endif
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/01/11, Add control for USB instert/remove, refer to bug386091*/
#if 0
	internal_drdata = data;
#endif
/*[PLATFORM]-Add-END by TCTNB.WPL*/

	return 0;

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE
free_irq_data:
      free_irq(client->irq, data);
#endif
/*[PLATFORM]-Add-END by TCTNB.WPL*/
free_reset_gpio:
	if (gpio_is_valid(pdata->reset_gpio))
		gpio_free(pdata->reset_gpio);
free_irq_gpio:
	if (gpio_is_valid(pdata->irq_gpio))
		gpio_free(pdata->reset_gpio);
pwr_off:
	if (pdata->power_on)
		pdata->power_on(false);
	else
		ft5x06_power_on(data, false);
pwr_deinit:
	if (pdata->power_init)
		pdata->power_init(false);
	else
		ft5x06_power_init(data, false);
unreg_inputdev:
	input_unregister_device(input_dev);
	input_dev = NULL;
free_inputdev:
	input_free_device(input_dev);
free_mem:
	kfree(data);
	return err;
}

static int __devexit ft5x06_ts_remove(struct i2c_client *client)
{
	struct ft5x06_ts_data *data = i2c_get_clientdata(client);

#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&data->early_suspend);
#endif
/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/01/10, add the TP ft5316 firmware upgrade function, refer to bug386588*/
#ifdef CONFIG_TOUCHSCREEN_FT5X06_FIRMWARE
#ifdef SYSFS_DEBUG
	ft5x0x_release_mutex();
#endif
#endif
/*[PLATFORM]-Mod-END by TCTNB.WPL*/

/*[PLATFORM]-Add-BEGIN by TCTNB.WPL, 2013/08/08, add for TP gesture detection function.*/
#ifdef CONFIG_8X30_TP_GESTURE
       misc_deregister(&tpwakeup_misc);
#endif
/*[PLATFORM]-Add-END by TCTNB.WPL*/

	free_irq(client->irq, data);

	if (gpio_is_valid(data->pdata->reset_gpio))
		gpio_free(data->pdata->reset_gpio);

	if (gpio_is_valid(data->pdata->irq_gpio))
		gpio_free(data->pdata->reset_gpio);

	if (data->pdata->power_on)
		data->pdata->power_on(false);
	else
		ft5x06_power_on(data, false);

	if (data->pdata->power_init)
		data->pdata->power_init(false);
	else
		ft5x06_power_init(data, false);

	input_unregister_device(data->input_dev);
	kfree(data);

	return 0;
}

static const struct i2c_device_id ft5x06_ts_id[] = {
	{"ft5x06_ts", 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, ft5x06_ts_id);

static struct i2c_driver ft5x06_ts_driver = {
	.probe = ft5x06_ts_probe,
	.remove = __devexit_p(ft5x06_ts_remove),
	.driver = {
		   .name = "ft5x06_ts",
		   .owner = THIS_MODULE,
#ifdef CONFIG_PM
		   .pm = &ft5x06_ts_pm_ops,
#endif
		   },
	.id_table = ft5x06_ts_id,
};

static int __init ft5x06_ts_init(void)
{
	return i2c_add_driver(&ft5x06_ts_driver);
}
module_init(ft5x06_ts_init);

static void __exit ft5x06_ts_exit(void)
{
	i2c_del_driver(&ft5x06_ts_driver);
}
module_exit(ft5x06_ts_exit);

MODULE_DESCRIPTION("FocalTech ft5x06 TouchScreen driver");
MODULE_LICENSE("GPL v2");
