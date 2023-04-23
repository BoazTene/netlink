#include <net/genetlink.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netlink.h>

#define NL_ECHO_FAMILY_NAME "echo"
#define NL_ECHO_FAMILY_VERSION 0x01

static int nl_echo_recv_msg(struct sk_buff *skb, struct genl_info *info);

enum nl_echo_attrs {
    NL_ECHO_MSG_ATTR = 0,
    NL_ECHO_MAX_ATTR,
};

static const struct nla_policy my_policy[NL_ECHO_MAX_ATTR+1] = {
     [NL_ECHO_MAX_ATTR] = { .type = NLA_STRING, .len = 4096 },
};

static const struct genl_ops nl_echo_ops[] = {
    {
        .cmd = 0,
        .flags = 0,
        .policy = my_policy,
        .doit = nl_echo_recv_msg,
        .dumpit = NULL,
    },
};

static struct genl_family nl_echo_family = {
    .id = 0,
    .hdrsize = 0,
    .name = NL_ECHO_FAMILY_NAME,
    .version = NL_ECHO_FAMILY_VERSION,
    .maxattr = NL_ECHO_MAX_ATTR,
    .ops = nl_echo_ops,
};

static int nl_echo_recv_msg(struct sk_buff *skb, struct genl_info *info) {
    struct sk_buff *skb_out;
    char *msg = "Echo from kernel: ";
    int msg_size =
        strlen(msg) + strlen(nla_data(info->attrs[NL_ECHO_MSG_ATTR]));
    int res;

    skb_out = genlmsg_new(msg_size, GFP_KERNEL);
    if (!skb_out) {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return -ENOMEM;
    }

    genlmsg_put(skb_out, 0, info->snd_seq, &nl_echo_family, 0, 1);
    nla_put_string(skb_out, NL_ECHO_MSG_ATTR,
                   nla_data(info->attrs[NL_ECHO_MSG_ATTR]));

    genlmsg_end(skb_out, NULL);
    
    res = genlmsg_unicast(genl_info_net(info), skb_out, info->snd_portid);
    if (res < 0) {
        printk(KERN_ERR "Error while sending back to user\n");
        return res;
    }

    return 0;
}



static int __init
nl_echo_init(void) {
    int rc = 0;

    printk(KERN_INFO "Initializing Netlink Echo module\n");

    rc = genl_register_family(&nl_echo_family);
    if (rc < 0) {
        printk(KERN_ERR "Failed to register netlink family: %d\n", rc);
        return rc;
    }

    rc = genl_register_family(&nl_echo_family);
    if (rc < 0) {
        printk(KERN_ERR "Failed to register netlink ops: %d\n", rc);
        genl_unregister_family(&nl_echo_family);
        return rc;
    }

    return 0;
}

static void __exit nl_echo_exit(void) {
    printk(KERN_INFO "Exiting Netlink Echo module\n");

    genl_unregister_family(&nl_echo_family);
}

module_init(nl_echo_init);
module_exit(nl_echo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Netlink Echo Module");
