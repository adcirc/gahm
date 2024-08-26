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
                                        "-Dgahm_ENABLE_PYTHON:BOOL=ON",
                                        "-Dgahm_PYTHON_PACKAGE_BUILD:BOOL=ON",
                                        "-Dgahm_ENABLE_SHARED:BOOL=OFF",
                                        "-Dgahm_ENABLE_STATIC:BOOL=OFF",
                                    ]
                                    + CIBW_CMAKE_OPTIONS,
        ),
    ],
    cmdclass=dict(
        build_ext=cmake_build_extension.BuildExtension,
    ),
)