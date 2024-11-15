from setuptools import Extension,setup
import numpy as np

setup(ext_modules=[
    Extension(
        name="bqn",
        sources=["bqn.c"],
        extra_link_args=["-lcbqn"],
        runtime_library_dirs = ["/usr/local/lib"],
        include_dirs=[np.get_include()]
    )
])
