# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
"""
Check library not checked in to repo.
"""


import pytest
import os

exclude_dirs = [".venv"]


@pytest.mark.lib
def test_lib_not_present():
    file_list = []
    for dirpath, dirnames, filenames in os.walk("."):
        for filename in [f for f in filenames if f.endswith(".a")]:
            include = True
            for exclude_dir in exclude_dirs:
                if exclude_dir in dirpath:
                    include = False
            if include:
                file_list.append(os.path.join(dirpath, filename))
    if len(file_list) > 0:
        for item in file_list:
            print(f"Found unexpected archive file in repo: {item}")
        pytest.fail(f"Found one of more archive files in repo")
