savedcmd_/home/boaz/netlink/examples/kernel_module/netlink_example.mod := printf '%s\n'   main.o | awk '!x[$$0]++ { print("/home/boaz/netlink/examples/kernel_module/"$$0) }' > /home/boaz/netlink/examples/kernel_module/netlink_example.mod
