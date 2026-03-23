#!/usr/bin/env python3
"""
Minimal autopilot mock that sends MAVLink Heartbeat and VFR_HUD

Usage:
    pip install -r autopilot_mock_requirements.txt
    python autopilot_mock.py
"""

import threading
import time

from pymavlink import mavutil
import pyray as rl

g_hud_fields = {
    "airspeed": 15.0,
    "groundspeed": 13.5,
    "throttle": 50,
    "climb": 0.0,
}
g_send_rate_hz = 5.0
g_running = True

g_heartbeat_count = 0
g_vfr_hud_count = 0


def sender_thread():
    global g_heartbeat_count, g_vfr_hud_count, g_hud_fields, g_running

    conn = mavutil.mavlink_connection(
        "udpout:127.0.0.1:14550",
        source_system=1,
        source_component=1,
    )

    last_heartbeat = 0.0
    last_vfr_hud = 0.0

    while g_running:
        now = time.monotonic()

        if now - last_heartbeat >= 1.0:
            conn.mav.heartbeat_send(
                mavutil.mavlink.MAV_TYPE_FIXED_WING,
                mavutil.mavlink.MAV_AUTOPILOT_ARDUPILOTMEGA,
                mavutil.mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,
                0,
                mavutil.mavlink.MAV_STATE_ACTIVE,
            )
            last_heartbeat = now
            g_heartbeat_count += 1

        rate = g_send_rate_hz
        if rate > 0:
            interval = 1.0 / rate
            if now - last_vfr_hud >= interval:
                conn.mav.vfr_hud_send(
                    airspeed=g_hud_fields["airspeed"],
                    groundspeed=g_hud_fields["groundspeed"],
                    heading=0,
                    throttle=int(g_hud_fields["throttle"]),
                    alt=0,
                    climb=g_hud_fields["climb"],
                )
                last_vfr_hud = now
                g_vfr_hud_count += 1

        time.sleep(0.001)


SLIDERS = [
    #  label              dict key        min    max   fmt
    ("Airspeed (m/s)",    "airspeed",     0.0,  50.0, "%.1f"),
    ("Groundspeed (m/s)", "groundspeed",  -50.0,  50.0, "%.1f"),
    ("Throttle (%)",      "throttle",     0.0,  100.0, "%.0f"),
    ("Climb (m/s)",       "climb",      -20.0,   20.0, "%.1f"),
]

ROW_H = 42
LABEL_X = 20
SLIDER_X = 20
SLIDER_W = 330
VALUE_X = 360


def main():
    global g_send_rate_hz, g_running, g_hud_fields, g_heartbeat_count

    WIDTH = 420
    HEIGHT = LABEL_X + len(SLIDERS) * ROW_H + int(ROW_H * 1.5) + 60
    rl.init_window(WIDTH, HEIGHT, "Autopilot Mock")
    rl.set_target_fps(60)

    t = threading.Thread(target=sender_thread, daemon=True)
    t.start()

    ptrs = {}
    for _, key, *_ in SLIDERS:
        ptrs[key] = rl.ffi.new("float *", g_hud_fields[key])
    rate_ptr = rl.ffi.new("float *", g_send_rate_hz)

    while not rl.window_should_close():
        rl.begin_drawing()
        rl.clear_background(rl.get_color(rl.gui_get_style(rl.GuiControl.DEFAULT, rl.GuiDefaultProperty.BACKGROUND_COLOR)))

        y = 20

        rl.draw_text("VFR_HUD rate (Hz)", LABEL_X, y, 14, rl.DARKGRAY)
        rl.draw_text(f"{rate_ptr[0]:.1f}", VALUE_X, y, 14, rl.DARKGRAY)
        y += 18
        rl.gui_slider_bar(rl.Rectangle(SLIDER_X, y, SLIDER_W, 16), "", "", rate_ptr, 0.0, 10.0)
        g_send_rate_hz = rate_ptr[0]
        y += int(ROW_H * 1.5)

        for label, key, lo, hi, fmt in SLIDERS:
            rl.draw_text(label, LABEL_X, y, 14, rl.DARKGRAY)
            rl.draw_text(fmt % ptrs[key][0], VALUE_X, y, 14, rl.DARKGRAY)
            y += 18
            rl.gui_slider_bar(rl.Rectangle(SLIDER_X, y, SLIDER_W, 16), "", "", ptrs[key], lo, hi)
            g_hud_fields[key] = ptrs[key][0]
            y += ROW_H - 18

        rl.draw_text("UDP  udpout:127.0.0.1:14550", LABEL_X, y, 12, rl.GRAY)
        y += 16
        rl.draw_text(
            f"Heartbeats: {g_heartbeat_count}    VFR_HUD: {g_vfr_hud_count}",
            LABEL_X, y, 12, rl.GRAY,
        )

        rl.end_drawing()

    g_running = False
    rl.close_window()


if __name__ == "__main__":
    main()
