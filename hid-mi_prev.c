/*
 * Force feedback support for ACRUX game controllers
 *
 * From what I have gathered, these devices are mass produced in China
 * by several vendors. They often share the same design as the original
 * Xbox 360 controller.
 *
 * 1a34:0802 "ACRUX USB GAMEPAD 8116"
 *  - tested with an EXEQ EQ-PCU-02090 game controller.
 *
 * Copyright (c) 2015 Maxim Lapunin <majagyage@gmail.com>
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/input.h>
#include <linux/slab.h>
#include <linux/hid.h>
#include <linux/module.h>
#include <linux/delay.h>

static __u8 mi_gamepad_rdesc[] = {
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x05,        // Usage (Game Pad)
0xA1, 0x01,        // Collection (Application)
0x85, 0x04,        //   Report ID (4)
0x15, 0x00,        //   Logical Minimum (0)
0x25, 0x01,        //   Logical Maximum (1)
0x35, 0x00,        //   Physical Minimum (0)
0x45, 0x01,        //   Physical Maximum (1)
0x75, 0x01,        //   Report Size (1)
0x95, 0x0F,        //   Report Count (15)
0x05, 0x09,        //   Usage Page (Button)
0x19, 0x01,        //   Usage Minimum (0x01)
0x29, 0x0F,        //   Usage Maximum (0x0F)
0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x95, 0x01,        //   Report Count (1)
0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
0x09, 0x07,        //   Usage (Keypad)
0xA1, 0x00,        //   Collection (Physical)
0x05, 0x07,        //     Usage Page (Kbrd/Keypad)
0x75, 0x01,        //     Report Size (1)
0x15, 0x00,        //     Logical Minimum (0)
0x25, 0x01,        //     Logical Maximum (1)
0x35, 0x00,        //     Physical Minimum (0)
0x45, 0x01,        //     Physical Maximum (1)
0x95, 0x05,        //     Report Count (5)
0x09, 0x52,        //     Usage (0x52)
0x09, 0x51,        //     Usage (0x51)
0x09, 0x50,        //     Usage (0x50)
0x09, 0x4F,        //     Usage (0x4F)
0x09, 0xF1,        //     Usage (0xF1)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //   End Collection
0x95, 0x03,        //   Report Count (3)
0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
0x25, 0x07,        //   Logical Maximum (7)
0x46, 0x3B, 0x01,  //   Physical Maximum (315)
0x75, 0x04,        //   Report Size (4)
0x95, 0x01,        //   Report Count (1)
0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
0x09, 0x39,        //   Usage (Hat switch)
0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
0x65, 0x00,        //   Unit (None)
0x95, 0x01,        //   Report Count (1)
0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x26, 0xFF, 0x00,  //   Logical Maximum (255)
0x46, 0xFF, 0x00,  //   Physical Maximum (255)
0x09, 0x30,        //   Usage (X)
0x09, 0x31,        //   Usage (Y)
0x09, 0x32,        //   Usage (Z)
0x09, 0x35,        //   Usage (Rz)
0x09, 0x33,        //   Usage (Rx)
0x09, 0x34,        //   Usage (Ry)
0x75, 0x08,        //   Report Size (8)
0x95, 0x06,        //   Report Count (6)
0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x05, 0x02,        //   Usage Page (Sim Ctrls)
0x15, 0x00,        //   Logical Minimum (0)
0x26, 0xFF, 0x00,  //   Logical Maximum (255)
0x09, 0xC5,        //   Usage (Brake)
0x09, 0xBB,        //   Usage (Throttle)
0x95, 0x02,        //   Report Count (2)
0x75, 0x08,        //   Report Size (8)
0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x05, 0x20,        //   Usage Page (0x20)
0x09, 0x73,        //   Usage (0x73)
0xA1, 0x80,        //   Collection (Vendor Defined 0x80)
0x05, 0x20,        //     Usage Page (0x20)
0x0A, 0x53, 0x04,  //     Usage (0x0453)
0x16, 0x01, 0x80,  //     Logical Minimum (32769)
0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
0x75, 0x10,        //     Report Size (16)
0x95, 0x01,        //     Report Count (1)
0x55, 0x0E,        //     Unit Exponent (-2)
0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x0A, 0x54, 0x04,  //     Usage (0x0454)
0x16, 0x01, 0x80,  //     Logical Minimum (32769)
0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
0x75, 0x10,        //     Report Size (16)
0x95, 0x01,        //     Report Count (1)
0x55, 0x0E,        //     Unit Exponent (-2)
0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x0A, 0x55, 0x04,  //     Usage (0x0455)
0x16, 0x01, 0x80,  //     Logical Minimum (32769)
0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
0x75, 0x10,        //     Report Size (16)
0x95, 0x01,        //     Report Count (1)
0x55, 0x0E,        //     Unit Exponent (-2)
0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //   End Collection
0x05, 0x0D,        //   Usage Page (Digitizer)
0x09, 0x3B,        //   Usage (Battery Strength)
0x15, 0x00,        //   Logical Minimum (0)
0x26, 0xFF, 0x00,  //   Logical Maximum (255)
0x75, 0x08,        //   Report Size (8)
0x95, 0x01,        //   Report Count (1)
0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x05, 0x0C,        //   Usage Page (Consumer)
0x0A, 0x23, 0x02,  //   Usage (AC Home)
0x75, 0x01,        //   Report Size (1)
0x15, 0x00,        //   Logical Minimum (0)
0x25, 0x01,        //   Logical Maximum (1)
0x35, 0x00,        //   Physical Minimum (0)
0x45, 0x01,        //   Physical Maximum (1)
0x95, 0x01,        //   Report Count (1)
0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x95, 0x07,        //   Report Count (7)
0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x85, 0x20,        //   Report ID (32)
0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
0x0A, 0x21, 0x26,  //   Usage (0x2621)
0x75, 0x08,        //   Report Size (8)
0x95, 0x06,        //   Report Count (6)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              // End Collection
//0x00,              // Unknown (bTag: 0x00, bType: 0x00)

// 247 bytes
};

static __u8 *mi_report_fixup(struct hid_device *hdev, __u8 *rdesc, unsigned int *rsize)
{
  struct hid_device *hid = hid_get_drvdata(hdev);
  rdesc = mi_gamepad_rdesc;
  *rsize = sizeof(mi_gamepad_rdesc);
  return rdesc;
};

struct miff_device {
  struct hid_report *report;
  struct work_struct state_worker;
  struct hid_device *hdev;
  int left, right;
  __u8 worker_initialized;
};

static void miff_state_worker(struct work_struct *work)
{
  struct miff_device *miff = container_of(work, struct miff_device, state_worker);


  miff->report->field[0]->value[0] = miff->left;
  miff->report->field[0]->value[1] = miff->right;
 // dbg_hid("(left,right)=(%04x, %04x)\n", left, right);
  hid_hw_request(miff->hdev, miff->report, HID_REQ_SET_REPORT);
}

static int miff_play(struct input_dev *dev, void *data, struct ff_effect *effect)
{
  struct hid_device *hid = input_get_drvdata(dev);
 // struct miff_device *miff = data;
  struct miff_device *miff = hid_get_drvdata(hid);

  if (effect->type != FF_RUMBLE)
    return 0;

  miff->left = effect->u.rumble.weak_magnitude * 0xff / 0xffff;
  miff->right = effect->u.rumble.strong_magnitude * 0xff / 0xffff;

  dbg_hid("(left,right)=(%04x, %04x)\n", miff->left, miff->right);

  schedule_work(&miff->state_worker);

  return 0;
};

static int xiaomi_init_ff(struct miff_device *miff)
{
//  struct hid_input *hidinput = list_first_entry(&(miff->hdev)->inputs, struct hid_input, list);

//  struct hid_input *hidinput = list_first_entry(&hid->inputs, struct hid_input, list);
  struct hid_input *hidinput = list_entry(miff->hdev->inputs.prev, struct hid_input, list);

  struct input_dev *dev = hidinput->input;

  miff->report = hid_validate_values(miff->hdev, HID_FEATURE_REPORT, 0, 0, 6);
  if (!miff->report)
    return -ENODEV;

  input_set_capability(dev, EV_FF, FF_RUMBLE);
  return input_ff_create_memless(dev, NULL, miff_play);
}

/*static int miff_init(struct hid_device *hid)
{
 // struct miff_device *miff;
  struct hid_report *report;
  struct hid_input *hidinput = list_first_entry(&hid->inputs, struct hid_input, list);
  struct input_dev *dev = hidinput->input;
  struct miff_device *miff = hid_get_drvdata(hid);
  int error;
*/

  /* Check that the report looks ok */
/*  report = hid_validate_values(hid, HID_FEATURE_REPORT, 0, 0, 6);
  if (!report)
    return -ENODEV;

  miff = kmalloc(sizeof(struct miff_device), GFP_KERNEL);
  if (!miff)
    return -ENOMEM;

  miff->hdev = hid;


  input_set_capability(dev, EV_FF, FF_RUMBLE);

  miff->report = report;
  error = input_ff_create_memless(dev, miff, miff_play);
  if (error) {
    kfree(miff);
    return error;
  }

  report->field[0]->value[0] = 0xFF;
  report->field[0]->value[1] = 0xFF;
  hid_hw_request(hid, report, HID_REQ_SET_REPORT);*/
/*
  msleep(2000);
  printk("1\n");

  report->field[0]->value[0] = 0x00;
  report->field[0]->value[1] = 0xff;
  hid_hw_request(hid, report, HID_REQ_SET_REPORT);

  printk("2\n");
*/
/*  msleep(1000);
  report->field[0]->value[0] = 0x00;
  report->field[0]->value[1] = 0x00;
  hid_hw_request(hid, miff->report, HID_REQ_SET_REPORT);
  hid_info(hid, "Force Feedback for Xiaomi Gamepad by Maxim Lapunin <majagyage@gmail.com>\n");

  return 0;
}
*/
static inline void miff_init_work(struct miff_device *miff, void (*worker)(struct work_struct *))
{
  if (!miff->worker_initialized)
    INIT_WORK(&miff->state_worker, worker);

  miff->worker_initialized = 1;
}

static inline void miff_cancel_work_sync(struct miff_device *miff)
{
  if (miff->worker_initialized)
    cancel_work_sync(&miff->state_worker);
}

static int mi_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
  int error;
  struct miff_device *miff;

  dev_dbg(&hdev->dev, "Xiaomi HID hardware probe...\n");

  miff = devm_kzalloc(&hdev->dev, sizeof(*miff), GFP_KERNEL);
  if (miff == NULL) {
    hid_err(hdev, "can't alloc xiaomi descriptor\n");
    return -ENOMEM;
  }

  hid_set_drvdata(hdev, miff);
  miff->hdev = hdev;

  error = hid_parse(hdev);
  if (error) {
    hid_err(hdev, "parse failed\n");
    return error;
  }

  error = hid_hw_start(hdev, HID_CONNECT_DEFAULT & ~HID_CONNECT_FF);
  if (error) {
    hid_err(hdev, "hw start failed\n");
    return error;
  }

  miff_init_work(miff, miff_state_worker);
  xiaomi_init_ff(miff);
 // error = miff_init(hdev);
//  if (error) {
//    hid_warn(hdev, "Failed to enable force feedback support, error: %d\n", error);
//    return error;
  //}

/*  error = hid_hw_open(hdev);
  if (error) {
    dev_err(&hdev->dev, "hw open failed\n");
    hid_hw_stop(hdev);
    return error;
  }*/


  return 0;
}

static void mi_remove(struct hid_device *hdev)
{
  struct miff_device *miff = hid_get_drvdata(hdev);
 // hid_hw_close(hdev);
  miff_cancel_work_sync(miff);
  hid_hw_stop(hdev);
}

static const struct hid_device_id mi_devices[] = {
  { HID_BLUETOOTH_DEVICE(0x2717, 0x3144) },
  { }
};
MODULE_DEVICE_TABLE(hid, mi_devices);

static struct hid_driver mi_driver = {
  .name         = "migamepad",
  .id_table     = mi_devices,
  .probe        = mi_probe,
  .report_fixup = mi_report_fixup,
  .remove       = mi_remove
};
module_hid_driver(mi_driver);

MODULE_AUTHOR("Maxim Lapunin");
MODULE_DESCRIPTION("Force feedback support for Xiaomi Gamepad");
MODULE_LICENSE("GPL");
