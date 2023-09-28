#!/usr/bin/env python3
# GNU General Public License v3.0
#
# This file is part of the GAHM model (https:#github.com/adcirc/gahm).
# Copyright (c) 2023 ADCIRC Development Group.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http:#www.gnu.org/licenses/>.
#
# Author: Zach Cobell
# Contact: zcobell@thewaterinstitute.org
#
# This file adapted from the cmake-build-extension examples
#
import inspect
import sys
import os
from pathlib import Path

import cmake_build_extension
import setuptools

init_py = inspect.cleandoc(
    """
    from . import pygahm
    
    __version__ = "0.0.1"
    """
)

CIBW_CMAKE_OPTIONS = []
if "CIBUILDWHEEL" in os.environ and os.environ["CIBUILDWHEEL"] == "1":
    # The manylinux variant runs in Debian Stretch and it uses lib64 folder
    if sys.platform == "linux":
        CIBW_CMAKE_OPTIONS += ["-DCMAKE_INSTALL_LIBDIR=lib"]

setuptools.setup(
    ext_modules=[
        cmake_build_extension.CMakeExtension(
            name="pygahm",
            install_prefix="pygahm",
            write_top_level_init=init_py,
            source_dir=str(Path(__file__).parent.absolute()),
            cmake_configure_options=[
                "-DPython3_ROOT_DIR={:s}".format(str(Path(sys.prefix).resolve())),
                "-DGAHM_ENABLE_PYTHON:BOOL=ON",
                "-DPYTHON_PACKAGE_BUILD:BOOL=ON",
            ]
            + CIBW_CMAKE_OPTIONS,
        ),
    ],
    cmdclass=dict(
        build_ext=cmake_build_extension.BuildExtension,
    ),
)
