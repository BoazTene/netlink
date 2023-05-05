from setuptools import Extension, setup
import subprocess

# Check if libnl3 is installed
try:
    subprocess.check_output(['pkg-config', '--exists', 'libnl-3.0'])
except subprocess.CalledProcessError:
    print('Please install libnl3 using your package manager before installing this module')
    print('Check out the README.md for more information.')
    exit()

setup(
    ext_modules=[
        Extension(
            name="netlink",  # as it would be imported
            libraries=['nl-3', 'nl-genl-3'],
            include_dirs=['/usr/include/libnl3'],
            sources=["src/main.c", "src/netlink.c", "src/netlink_class.c", "src/message.c", "src/attribute_policy.c", "src/attribute.c"], # all sources are compiled into a single binary file
        ),
    ]
)
