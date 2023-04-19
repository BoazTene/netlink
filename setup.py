from setuptools import Extension, setup

setup(
    ext_modules=[
        Extension(
            name="netlink",  # as it would be imported
                               # may include packages/namespaces separated by `.`

            sources=["src/main.c", "src/netlink.c", "src/netlink_class.c"], # all sources are compiled into a single binary file
        ),
    ]
)
