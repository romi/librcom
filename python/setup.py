from setuptools import setup
from setuptools import find_packages
import os

setup (
    name = 'rcom',
    description = 'The Python classes for the librcom communication library',
    version = '0.1.0',
    license = 'GPLv3',
    author = 'Peter Hanappe',
    download_url = 'https://github.com/romi/librcom',
    packages = find_packages(include=['rcom', 'rcom.*']),
    install_requires = [
        'websocket-client'
    ],
    classifiers = ["Development Status :: 3 - Alpha",
                   "Intended Audience :: Developers",
                   "License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)",
                   "Programming Language :: Python :: 3",
                   "Framework :: Robot Framework :: Library"]
)
