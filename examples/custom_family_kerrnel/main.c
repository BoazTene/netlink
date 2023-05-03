#include <net/genetlink.h>
#include <linux/module.h>
#include <linux/kernel.h>

/* attributes (variables):
 * the index in this enum is used as a reference for the type,
 * userspace application has to indicate the corresponding type
 * the policy is used for security considerations 
 */
enum {
    FAMILY1_A_UNSPEC,
    FAMILY1_A_MSG,
    __FAMILY1_A_MAX,
};
#define FAMILY1_A_MAX (__FAMILY1_A_MAX - 1)

/* attribute policy: defines which attribute has which type (e.g int, char * etc)
 * possible values defined in net/netlink.h 
 */

static struct nla_policy family1_genl_policy[FAMILY1_A_MAX + 1] = {
    [FAMILY1_A_MSG] = { .type = NLA_STRING, .len=100},
};

/**
 * Version Number of various families
 */
#define FAMILY1_VERSION_NR 1


/* commands: enumeration of all commands (functions), 
 * used by userspace application to identify command to be executed
 */
enum {
    FAMILY_C_UNSPEC,
    FAMILY_C_SEND,
    FAMILY_C_RECV,
    __FAMILY_C_MAX,
};

#define FAMILY_C_MAX (__FAMILY_C_MAX - 1)
int family1_recv(struct sk_buff *skb_temp, struct genl_info *info);

/*
 * Commands: mapping between the command enumeration and the actual function
 */
struct genl_ops family1_gnl_ops_recv[] = {{
    .cmd = FAMILY_C_SEND,
    .flags = 0,
    .policy = family1_genl_policy,
    .doit = family1_recv,
    .dumpit = NULL,
}};


static struct genl_family family1_gnl_family = {
    .id = 0,      
    .hdrsize = 0,
    .name = "FAMILY1",
    .version = FAMILY1_VERSION_NR,
    .maxattr = FAMILY1_A_MAX,
    .n_ops = 1,
    .ops = family1_gnl_ops_recv,
};

/**
 * Callback for hadnling family1 data reception
*/
int family1_recv(struct sk_buff *skb_temp, struct genl_info *info) {
    struct nlattr *na, *validate_attrs[FAMILY1_A_MAX + 1];
    struct sk_buff *skb;
    int rc, is_valid;
    void *msg_head;
    char * recvd_data;
    int *recvd_int;
    
    if (info == NULL) {
        printk("Info is null. \n");
    }

    na = info->attrs[FAMILY1_A_MSG];
    if (na) {
        /**
         * Validate attributes
        */
        

        recvd_data = (char *)nla_data(na);
        if (recvd_data == NULL) {
            printk("error while receiving data\n");
        } else {
            printk("received: %s\n", recvd_data);
        }
        // recvd_int = (int *)nla_data(na);
        // printk("received: %d\n", *recvd_int);
    } else {
        printk("no info->attrs %i\n", FAMILY1_A_MSG);
    }

    struct sk_buff *replyskb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
    if (replyskb == NULL) {
        goto out;
    }
   
    msg_head = genlmsg_put(replyskb, info->snd_portid, info->snd_seq, &family1_gnl_family, 0, FAMILY_C_RECV);
    if (msg_head == NULL) {
        rc = -ENOMEM;
        goto out;
    }

    //Add a FAMILY1_A_MSG attribute (actual value to be sent)
    rc = nla_put_string(replyskb, FAMILY1_A_MSG, "Reply from family 2");
    if (rc != 0) {
        goto out;
    }
    
    //Finalize the message
    genlmsg_end(replyskb, msg_head);

    //Send the message back
    // genlmsg_unicast
    genlmsg_reply(replyskb, info);
    if (rc != 0) {
        goto out;
    }

    return 0;
    out:
    printk("An error occured in family 2 receive:\n");
    return 0;

}

/**
    Module entry
*/
static int __init gnKernel_init(void) {
    int rc;
    printk("Generic Netlink Example Module inserted.\n");
              
    rc = genl_register_family(&family1_gnl_family);
    if (rc != 0) {
        goto failure;
    }

    return 0; 

failure:
    printk("An error occured while inserting the generic netlink module\n");
    return -1;
}

static void __exit gnKernel_exit(void) {
    int ret;
    printk("Generic Netlink Example Module unloaded.\n");
    //Unregister the family
    
    ret = genl_unregister_family(&family1_gnl_family);
    if(ret !=0) {
        printk("Unregister family 1: %i\n",ret);
        return;
    }
}

module_init(gnKernel_init);
module_exit(gnKernel_exit);
MODULE_LICENSE("GPL");
