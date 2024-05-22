# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XCORE VocalFusion Licence.
"""
Check library not checked in to repo.
"""

import pytest
import os


def test_lib_not_present():
    for dirpath, dirnames, filenames in os.walk(".."):
        for filename in [f for f in filenames if f.endswith(".a")]:
            pytest.fail(f"Found unexpected archive file in repo: {os.path.join(dirpath, filename)}")
