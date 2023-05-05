/*
    Python client for the Netlink interface.
    Copyright (C) 2023 Boaz Tene

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <net/genetlink.h>
#include <linux/module.h>
#include <linux/kernel.h>


/*
 * Family's attributes.
 */
enum {
    CUSTOM_FAMILY_A_UNSPEC,
    CUSTOM_FAMILY_A_MSG,
    __CUSTOM_FAMILY_A_MAX,
};
#define CUSTOM_FAMILY_A_MAX (__CUSTOM_FAMILY_A_MAX - 1)

/*
 * Family's policy.
 */
static struct nla_policy custom_family_genl_policy[CUSTOM_FAMILY_A_MAX + 1] = {
    [CUSTOM_FAMILY_A_MSG] = { .type = NLA_STRING, .len=300},
};

/**
 * Version Number.
 */
#define CUSTOM_FAMILY_VERSION_NR 1


/* commands: enumeration of all commands (functions), 
 * used by userspace application to identify command to be executed
 */
enum {
    CUSTOM_FAMILY_C_UNSPEC,
    CUSTOM_FAMILY_C_SEND,
    CUSTOM_FAMILY_C_RECV,
    __CUSTOM_FAMILY_C_MAX,
};

#define CUSTOM_FAMILY_C_MAX (__FAMILY_C_MAX - 1)
int custom_family_recv(struct sk_buff *skb_temp, struct genl_info *info);

/*
 * Commands: mapping between the command enumeration and the actual function
 */
struct genl_ops custom_family_gnl_ops_recv[] = {{
    .cmd = CUSTOM_FAMILY_C_SEND,
    .flags = 0,
    .policy = custom_family_genl_policy,
    .doit = custom_family_recv,
    .dumpit = NULL,
}};


static struct genl_family custom_family_gnl_family = {
    .id = 0,      
    .hdrsize = 0,
    .name = "custom_family",
    .version = CUSTOM_FAMILY_VERSION_NR,
    .maxattr = CUSTOM_FAMILY_A_MAX,
    .n_ops = 1,
    .ops = custom_family_gnl_ops_recv,
};

/**
 * Callback for hadnling family data reception
*/
int custom_family_recv(struct sk_buff *skb_temp, struct genl_info *info) {
    struct nlattr *na; 
    int rc;    
    struct sk_buff *replyskb;
    void *msg_head;
    char * recvd_data;
        
    if (info == NULL) {
        printk("Info is null. \n");
    }

    na = info->attrs[CUSTOM_FAMILY_A_MSG];
    if (na) {
        recvd_data = (char *)nla_data(na);
        if (recvd_data == NULL) {
            printk("error while receiving data\n");
        } else {
            printk("received: %s\n", recvd_data);
        }
    } else {
        printk("no info->attrs %i\n", CUSTOM_FAMILY_A_MSG);
    }

    replyskb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
    if (replyskb == NULL) {
        goto out;
    }
   
    msg_head = genlmsg_put(replyskb, info->snd_portid, info->snd_seq, &custom_family_gnl_family, 0, CUSTOM_FAMILY_C_RECV);
    if (msg_head == NULL) {
        rc = -ENOMEM;
        goto out;
    }

    //Add a CUSTOM_FAMILY_A_MSG attribute (actual value to be sent)
    rc = nla_put_string(replyskb, CUSTOM_FAMILY_A_MSG, "Reply from family 2");
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

static int __init custom_family_init(void) {
    int rc;
    printk("Generic Netlink Example Module inserted.\n");
              
    rc = genl_register_family(&custom_family_gnl_family);
    if (rc != 0) {
        goto failure;
    }

    return 0; 

failure:
    printk("An error occured while inserting the generic netlink module\n");
    return -1;
}

static void __exit custom_family_exit(void) {
    printk("Generic netlink example module unloaded\n");

    //Unregister the family
    genl_unregister_family(&custom_family_gnl_family);
}

module_init(custom_family_init);
module_exit(custom_family_exit);
MODULE_AUTHOR("Boaz Tene");
MODULE_LICENSE("GPL");
