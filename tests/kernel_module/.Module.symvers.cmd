cmd_/home/boaz/Documents/school/firewallservice/netlink/tests/kernel_module/Module.symvers := sed 's/\.ko$$/\.o/' /home/boaz/Documents/school/firewallservice/netlink/tests/kernel_module/modules.order | scripts/mod/modpost -m -a  -o /home/boaz/Documents/school/firewallservice/netlink/tests/kernel_module/Module.symvers -e -i Module.symvers   -T -
