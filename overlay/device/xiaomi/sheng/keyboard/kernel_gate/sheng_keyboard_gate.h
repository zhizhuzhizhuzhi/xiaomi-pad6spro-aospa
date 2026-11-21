/* SPDX-License-Identifier: GPL-2.0-only */
/* Event-driven gate for the four virtual HID functions of the Sheng keyboard. */
#include <linux/device.h>
#include <linux/hid.h>
#include <linux/input.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

static int Nanosic_charge_i2c_write(void *buf, size_t len);

#define SHENG_HID_VENDOR 0x15d9
#define SHENG_HID_FIRST 0x00a1
#define SHENG_HID_LAST 0x00a4
#define SHENG_HID_COUNT 4

static DEFINE_MUTEX(sheng_hid_lock);
static struct device *sheng_hid[SHENG_HID_COUNT];
static bool sheng_attached;
static bool sheng_hall_ready;
static bool sheng_tablet_mode = true;
static bool sheng_gate_stopping;
static bool sheng_gate_registered;
/* Only restore the keyboard after this gate has sent a fold notification. */
static bool sheng_fold_notified;

static void sheng_gate_notify_keyboard(bool attached, bool hall_ready, bool folded)
{
    u8 cmd[66] = { 0x32, 0x00, 0x4e, 0x31, 0x80, 0x38, 0x23, 0x01 };
    int ret;

    if (!attached) {
        sheng_fold_notified = false;
        return;
    }
    if (!hall_ready || folded == sheng_fold_notified)
        return;
    /* HyperOS keyboard backlight command, brightness range 0..100. */
    cmd[8] = folded ? 0 : 50;
    cmd[9] = 0x5b + cmd[8];
    ret = Nanosic_charge_i2c_write(cmd, sizeof(cmd));
    if (ret >= 0) {
        sheng_fold_notified = folded;
        pr_info("sheng keyboard: fold notification %d sent (%d)\n", folded, ret);
    } else {
        pr_warn("sheng keyboard: fold notification %d failed (%d)\n", folded, ret);
    }
}

static void sheng_gate_workfn(struct work_struct *work)
{
    struct device *snapshot[SHENG_HID_COUNT] = { NULL };
    bool enable = READ_ONCE(sheng_attached) && READ_ONCE(sheng_hall_ready) &&
                  !READ_ONCE(sheng_tablet_mode);
    int i;

    sheng_gate_notify_keyboard(READ_ONCE(sheng_attached),
                               READ_ONCE(sheng_hall_ready),
                               READ_ONCE(sheng_tablet_mode));

    mutex_lock(&sheng_hid_lock);
    for (i = 0; i < SHENG_HID_COUNT; ++i) {
        if (sheng_hid[i] && !device_is_registered(sheng_hid[i])) {
            put_device(sheng_hid[i]);
            sheng_hid[i] = NULL;
        }
        if (sheng_hid[i]) snapshot[i] = get_device(sheng_hid[i]);
    }
    mutex_unlock(&sheng_hid_lock);

    for (i = 0; i < SHENG_HID_COUNT; ++i) {
        struct device *dev = snapshot[i];
        if (!dev) continue;
        if (device_is_registered(dev) && !READ_ONCE(sheng_gate_stopping)) {
            if (enable && !READ_ONCE(dev->driver)) {
                int ret = device_attach(dev);
                if (ret < 0) pr_warn("sheng keyboard: attach %s failed: %d\n", dev_name(dev), ret);
            } else if (!enable && READ_ONCE(dev->driver)) {
                device_release_driver(dev);
            }
        }
        put_device(dev);
    }
}
static DECLARE_WORK(sheng_gate_work, sheng_gate_workfn);

static void sheng_gate_update(bool attached)
{
    if (READ_ONCE(sheng_attached) == attached) return;
    WRITE_ONCE(sheng_attached, attached);
    if (!READ_ONCE(sheng_gate_stopping)) schedule_work(&sheng_gate_work);
}

static void sheng_gate_event(struct input_handle *handle, unsigned int type,
                             unsigned int code, int value)
{
    if (type != EV_SW || code != SW_TABLET_MODE) return;
    WRITE_ONCE(sheng_tablet_mode, value != 0);
    WRITE_ONCE(sheng_hall_ready, true);
    if (!READ_ONCE(sheng_gate_stopping)) schedule_work(&sheng_gate_work);
}

static int sheng_gate_connect(struct input_handler *handler, struct input_dev *input,
                              const struct input_device_id *id)
{
    struct input_handle *handle;
    struct device *parent;
    int ret, index;

    if (input->id.vendor == SHENG_HID_VENDOR && input->id.product >= SHENG_HID_FIRST &&
        input->id.product <= SHENG_HID_LAST) {
        parent = input->dev.parent;
        if (!parent || !parent->bus || strcmp(parent->bus->name, "hid")) return -ENODEV;
        index = input->id.product - SHENG_HID_FIRST;
        mutex_lock(&sheng_hid_lock);
        if (sheng_hid[index] != parent) {
            if (sheng_hid[index]) put_device(sheng_hid[index]);
            sheng_hid[index] = get_device(parent);
        }
        mutex_unlock(&sheng_hid_lock);
        if (!READ_ONCE(sheng_gate_stopping)) schedule_work(&sheng_gate_work);
        return -ENODEV; /* observe registration without grabbing input events */
    }
    if (!input->name || strcmp(input->name, "xiaomi-hall") ||
        !test_bit(SW_TABLET_MODE, input->swbit)) return -ENODEV;
    handle = kzalloc(sizeof(*handle), GFP_KERNEL);
    if (!handle) return -ENOMEM;
    handle->dev = input;
    handle->handler = handler;
    handle->name = "sheng-keyboard-hall";
    ret = input_register_handle(handle);
    if (ret) goto fail;
    ret = input_open_device(handle);
    if (ret) goto unregister;
    WRITE_ONCE(sheng_tablet_mode, test_bit(SW_TABLET_MODE, input->sw));
    WRITE_ONCE(sheng_hall_ready, true);
    if (!READ_ONCE(sheng_gate_stopping)) schedule_work(&sheng_gate_work);
    return 0;
unregister:
    input_unregister_handle(handle);
fail:
    kfree(handle);
    return ret;
}

static void sheng_gate_disconnect(struct input_handle *handle)
{
    WRITE_ONCE(sheng_hall_ready, false);
    if (!READ_ONCE(sheng_gate_stopping)) schedule_work(&sheng_gate_work);
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id sheng_gate_ids[] = {
    { .driver_info = 1 },
    { },
};
static struct input_handler sheng_gate_handler = {
    .event = sheng_gate_event,
    .connect = sheng_gate_connect,
    .disconnect = sheng_gate_disconnect,
    .name = "sheng-keyboard-gate",
    .id_table = sheng_gate_ids,
};

static int sheng_gate_init(void)
{
    int ret;
    WRITE_ONCE(sheng_gate_stopping, false);
    ret = input_register_handler(&sheng_gate_handler);
    sheng_gate_registered = ret == 0;
    return ret;
}

static void sheng_gate_exit(void)
{
    int i;
    WRITE_ONCE(sheng_gate_stopping, true);
    if (sheng_gate_registered) input_unregister_handler(&sheng_gate_handler);
    sheng_gate_registered = false;
    cancel_work_sync(&sheng_gate_work);
    mutex_lock(&sheng_hid_lock);
    for (i = 0; i < SHENG_HID_COUNT; ++i) {
        if (sheng_hid[i]) put_device(sheng_hid[i]);
        sheng_hid[i] = NULL;
    }
    mutex_unlock(&sheng_hid_lock);
}
