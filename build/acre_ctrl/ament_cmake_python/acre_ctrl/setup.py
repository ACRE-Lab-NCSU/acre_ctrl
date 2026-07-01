from setuptools import find_packages
from setuptools import setup

setup(
    name='acre_ctrl',
    version='0.0.0',
    packages=find_packages(
        include=('acre_ctrl', 'acre_ctrl.*')),
)
