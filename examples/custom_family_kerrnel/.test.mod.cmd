cmd_/home/boaz/netlink/tests/kernel_module/test.mod := printf '%s\n'   main.o | awk '!x[$$0]++ { print("/home/boaz/netlink/tests/kernel_module/"$$0) }' > /home/boaz/netlink/tests/kernel_module/test.mod