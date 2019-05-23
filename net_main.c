#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/udp.h>
#include <linux/ip.h>


MODULE_AUTHOR("Itay Avraham - https://www.linkedin.com/in/itay-avraham-76996b51");
MODULE_DESCRIPTION("Network packet analayzer");
MODULE_LICENSE("N/A");

// Net filter hook option struct
static struct nf_hook_ops nfho;

// Socket buffer.
struct sk_buff *sock_buff;

// UDP header struct.
struct udphdr *udp_header;

// IP header struct
struct iphdr *ip_header;


unsigned int hookfn(const struct nf_hook_ops *ops, struct sk_buff *skb,
			        const struct net_device *in, const struct net_device *out,
#ifndef __GENKSYMS__
			        const struct nf_hook_state *state)
#else
			        int (*okfn)(struct sk_buff *))
#endif
{
    sock_buff =
        skb;
    
    // Grab network header using accessor.
    ip_header =
        (struct iphdr*)skb_network_header(sock_buff);
   
    if (!sock_buff)
        return NF_ACCEPT;

    if (ip_header->protocol == 17)
    {
        // Grab transport header.
        udp_header =
            (struct udphdr *)skb_transport_header(sock_buff);
        
        // Log we’ve got udp packet to /var/log/messages.
        printk(KERN_INFO "got udp packet \n");

        return NF_DROP;
    }
           
    return NF_ACCEPT;
}
 



static int init(void)
{
    printk("Netfilter get udp packet\n");
    nfho.hook = hookfn;
    nfho.hooknum = NF_INET_PRE_ROUTING;
    nfho.pf = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;

    nf_register_hook(&nfho);
   
    return 0;
}




static void clear(void)
{
    nf_unregister_hook(&nfho);

    printk("net_main close\n");
}


module_init(init);
module_exit(clear);
