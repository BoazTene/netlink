cmd_/home/boaz/netlink/tests/kernel_module/Module.symvers := sed 's/ko$$/o/' /home/boaz/netlink/tests/kernel_module/modules.order | scripts/mod/modpost -m -a  -o /home/boaz/netlink/tests/kernel_module/Module.symvers -e -i Module.symvers   -T -