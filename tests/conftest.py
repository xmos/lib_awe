# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XCORE VocalFusion Licence.
import pytest
import subprocess
from pathlib import Path
import platform
import re
import shutil


def pytest_addoption(parser):
    parser.addoption(
        "--level",
        action="store",
        default="smoke",
        choices=["smoke", "nightly", "weekend"],
        help="Test coverage level",
    )

    parser.addini("xk_216_mc_dut", help="XTAG ID for xk_216_mc DUT")
    parser.addini("xk_216_mc_harness", help="XTAG ID for xk_216_mc harness")
    parser.addini("xk_316_mc_dut", help="XTAG ID for xk_316_mc DUT")
    parser.addini("xk_316_mc_harness", help="XTAG ID for xk_316_mc harness")
    parser.addini("xk_evk_xu316_dut", help="XTAG ID for xk_evk_xu316 DUT")
    parser.addini("xk_evk_xu316_harness", help="XTAG ID for xk_evk_xu316 harness")


boards = ["xk_216_mc", "xk_316_mc", "xk_evk_xu316"]


board_configs = {}


all_freqs = [44100, 48000, 88200, 96000, 176400, 192000]

def samp_freqs_upto(max):
    return [fs for fs in all_freqs if fs <= max]


def parse_features(board, config):
    print(board)

    features = dict()
    features["uac"] = 2
    features["chan_i"] = 0
    features["chan_o"] = 2
    features["midi"] = None
    features["spdif_i"] = None
    features["spdif_o"] = None
    features["adat_i"] = None
    features["adat_o"] = None
    features["dsd"] = None
    features["tdm8"] = None
    features["samp_freqs"] = [48000]
    features["i2s_loopback"] = False

    return features


def pytest_sessionstart(session):
    usb_audio_dir = Path(__file__).parents[1]
    app_prefix = "app_"
    exclude_app = ["usb_audio_awe_small_qf60", "usb_audio_awe_small", "zoom_demo"]

    test_level = session.config.getoption("level")

    for app_dir in usb_audio_dir.iterdir():
        app_name = app_dir.name
        if not app_name.startswith(app_prefix) or app_name in exclude_app:
            continue

        board = app_name[len(app_prefix) :]

        # Get all the configs, and determine which will be fully- or partially-tested
        allconfigs_cmd = ["xmake", "allconfigs"]
        ret = subprocess.run(
            allconfigs_cmd, capture_output=True, text=True, cwd=app_dir
        )
        full_configs = ret.stdout.split()

        if test_level in ["nightly", "weekend"]:
            allconfigs_cmd.append("PARTIAL_TEST_CONFIGS=1")
            ret = subprocess.run(
                allconfigs_cmd, capture_output=True, text=True, cwd=app_dir
            )
            configs = ret.stdout.split()
        else:
            configs = full_configs

        # On Windows also collect special configs that will use the built-in driver
        if platform.system() == "Windows":
            winconfigs_cmd = ["xmake", "TEST_SUPPORT_CONFIGS=1", "allconfigs"]
            ret = subprocess.run(
                winconfigs_cmd, capture_output=True, text=True, cwd=app_dir
            )
            configs += [cfg for cfg in ret.stdout.split() if "_winbuiltin" in cfg]

        partial_configs = [config for config in configs if config not in full_configs]

        for config in configs:
            global board_configs
            features = parse_features(board, config)
            # Mark the relevant configs for partial testing only
            features["partial"] = config in partial_configs
            board_configs[f"{board}-{config}"] = features

            # add in the i2s loopback configs used for testing on 316
            if board == "xk_316_mc":
                features_i2sloopback = features.copy()
                features_i2sloopback["i2s_loopback"] = True
                features_i2sloopback["analogue_i"] = 0
                board_configs[f"{board}-{config}_i2sloopback"] = features_i2sloopback

def list_configs():
    return [tuple(k.split("-", maxsplit=1)) for k in board_configs.keys()]


def get_config_features(board, config):
    return board_configs[f"usb_audio_awe-I2S"]
    # return board_configs[f"{board}-{config}"]


# Dictionary indexed by board name, with each entry being the tuple of XTAG IDs for
# the DUT and harness for that board, (None, None) if XTAGs for board not provided.
xtag_dut_harness = {}


def pytest_configure(config):
    global xtag_dut_harness

    # Populate the global xtag_dut_harness dictionary with the XTAG IDs that were set
    # in pytest.ini or overridden on the command-line
    for board in boards:
        dut = config.getini(f"{board}_dut")
        harness = config.getini(f"{board}_harness")
        xtag_dut_harness[board] = (dut, harness)
        print("***", board, dut, harness)


# def pytest_collection_modifyitems(config, items):
#     selected = []
#     deselected = []

#     # Deselect testcases which use hardware that doesn't have an XTAG ID
#     for item in items:
#         m = item.get_closest_marker("uncollect_if")
#         if m:
#             func = m.kwargs["func"]
#             if func(config, **item.callspec.params):
#                 deselected.append(item)
#             else:
#                 selected.append(item)

#     config.hook.pytest_deselected(items=deselected)
#     items[:] = selected


# Print a session-level warning if usbdeview is not available on Windows
def pytest_terminal_summary(terminalreporter):
    if platform.system() == "Windows" and not shutil.which("usbdeview"):
        terminalreporter.section("Session warning")
        terminalreporter.write(
            "usbdeview not on PATH so test device data has not been cleared"
        )
