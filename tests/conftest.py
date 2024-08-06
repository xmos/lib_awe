# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

from pathlib import Path
import platform
import pytest
import time


from hardware_test_tools.UaDut import UaDut
from hardware_test_tools.XcoreApp import XcoreApp
from awe_test_utils import flash_xe, xe_demo_ffs_host, boot_partition_size, dp_with_ffs

def pytest_addoption(parser):
    parser.addini("xtag_dut", help="XTAG ID for the DUT")
    parser.addini("xtag_harness", help="XTAG ID for the audio analyser harness")


def pytest_configure(config):
    global xtag_dut
    global xtag_harness

    # Populate the globals with values from pytest.ini or overridden on the command-line
    xtag_dut = config.getini("xtag_dut")
    xtag_harness = config.getini("xtag_harness")


# Fail if any tests do not have markers to avoid new tests being added but not automatically run
def pytest_collection_modifyitems(config, items):
    unmarked = [item.name for item in items if len(item.own_markers) == 0]
    if len(unmarked) > 0:
        raise pytest.UsageError(f"Unmarked tests {unmarked}")


class AweDut(UaDut):
    def __init__(self, adapter_id, config, xflash=False):
        fw_path = (
            Path(__file__).parents[2]
            / "an02016"
            / "app_an02016"
            / "bin"
            / config
            / f"app_an02016_{config}.xe"
        )
        assert fw_path.exists()

        pid = 0x18
        prod_str = "XMOS xCORE.ai AWE (UAC2.0)"
        chans_in = 2
        chans_out = 2

        super().__init__(
            adapter_id,
            fw_path,
            pid,
            prod_str,
            chans_in,
            chans_out,
            xflash=xflash,
        )

class AweDutNoUA(XcoreApp):
    def __init__(self, adapter_id, config, pid=0x18, timeout=60, xflash=False):
        fw_path = (
            Path(__file__).parents[2]
            / "an02016"
            / "app_an02016"
            / "bin"
            / config
            / f"app_an02016_{config}.xe"
        )
        assert fw_path.exists(), f"Firmware not present at {fw_path}"

        prod_str = "XMOS xCORE.ai AWE (UAC2.0)" # Currently unused - TODO remove?
        self.pid = pid

        # self.winbuiltin = winbuiltin
        # if platform.system() == "Windows" and not self.winbuiltin:
        #     ini_path = (
        #         Path(os.environ["PROGRAMFILES"])
        #         / "XMOS"
        #         / "USB Audio Device Driver"
        #         / "x64"
        #         / "custom.ini"
        #     )
        #     assert (
        #         ini_path.exists()
        #     ), f"tusbaudio SDK custom.ini not found in expected location: {ini_path}"

        #     with open(ini_path, "r") as f:
        #         config = configparser.ConfigParser()
        #         config.read_file(f)
        #     try:
        #         self.driver_guid = config.get("DriverInterface", "InterfaceGUID")
        #     except (configparser.NoSectionError, configparser.NoOptionError):
        #         assert 0, f"Could not find InterfaceGUID in {ini_path}"
        # else:
        #     self.driver_guid = None

        super().__init__(fw_path, adapter_id, timeout=timeout, xflash=xflash)

    def __enter__(self):
        super().__enter__()

        if platform.system() == "Windows":
            # Delay to allow the Windows to set up the new device
            time.sleep(40)

        return self

    def __exit__(self, exc_type, exc_val, exc_tb):

        super().__exit__(exc_type, exc_val, exc_tb)

        if platform.system() == "Windows":
            # Delay to allow the Windows Audio service to settle after the device is removed
            time.sleep(15)

            # If usbdeview program is present on Windows, uninstall the device to force
            # re-enumeration next time and avoid caching of device features by the OS
            usbdeview_path = shutil.which("usbdeview")
            if usbdeview_path:
                subprocess.run(
                    [
                        usbdeview_path,
                        "/RunAsAdmin",
                        "/remove_by_pid",
                        f"0x20b1;{hex(self.pid)}",
                    ],
                    timeout=10,
                )



def get_xtag_ids(pytestconfig):
    xtag_dut = pytestconfig.getini("xtag_dut")
    xtag_harness = pytestconfig.getini("xtag_harness")
    return xtag_dut, xtag_harness


@pytest.fixture
def flash_ua_with_ffs(pytestconfig, scope="session"):
    """
    This fixture programs the UA/FFS binary and the pre-made FFS containing a couple of AWBs.
    """

    adapter_dut, adapter_harness = get_xtag_ids(pytestconfig)
    stdout = flash_xe(xe_demo_ffs_host, adapter_dut, boot_partition_size=boot_partition_size, data_partition_bin=dp_with_ffs)
    print(stdout)
    time.sleep(5) # Wait for HID to come up after boot
