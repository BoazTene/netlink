from setuptools import Extension, setup

setup(
    ext_modules=[
        Extension(
            name="netlink",  # as it would be imported
                               # may include packages/namespaces separated by `.`
            libraries=['nl-3', 'nl-genl-3'],
            library_dirs=['/lib/x86_64-linux-gnu/', '/usr/lib/x86_64-linux-gnu'],
            include_dirs=['/usr/include/libnl3'],
            sources=["src/main.c", "src/netlink.c", "src/netlink_class.c", "src/message.c"], # all sources are compiled into a single binary file
        ),
    ]
)
