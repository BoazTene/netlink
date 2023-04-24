cmd_/home/boaz/netlink/tests/kernel_module/modules.order := {   echo /home/boaz/netlink/tests/kernel_module/test_k.ko; :; } | awk '!x[$$0]++' - > /home/boaz/netlink/tests/kernel_module/modules.order
