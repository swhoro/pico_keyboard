extern "C" {
#include "pico/stdlib.h"

#include "bsp/board.h"
}

static void send_hid_report() {
    // skip if hid is not ready yet
    if (!tud_hid_ready()) return;

    uint8_t keycode[6] = {0};
    keycode[0]         = gpio_get(12) ? 0 : HID_KEY_Q;
    keycode[1]         = gpio_get(13) ? 0 : HID_KEY_E;

    tud_hid_keyboard_report(1, 0, keycode);
}

static void send_key() {
    // Remote wakeup
    if (tud_suspended()) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    } else {
        // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
        send_hid_report();
    }
}

int main(void) {
    board_init();
    tusb_init();

    uint32_t gpio_mask = 1 << 12 | 1 << 13;
    gpio_set_dir_in_masked(gpio_mask);
    gpio_pull_up(12);
    gpio_pull_up(13);

    while (1) {
        tud_task();
        send_key();
        sleep_ms(1000 / ROUNDHZ);
    }
    return 0;
}
