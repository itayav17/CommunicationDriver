// Static variables.
#define NET_MAJOR 0
#define NET_MINOR 1
#define NET_MAX_MINOR 5

#define print_debug(msg, ...) \
    printk(KERN_DEBUG "%s:%d - %s(). " msg ".\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);

#define print_error(msg, ...) \
    printk(KERN_DEBUG "%s:%d - %s(). " msg ".\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);

struct file_operations net_file_operations =
{
    .owner = THIS_MODULE,
    .open = net_open,
    .release = net_release,
    .read = net_read,
    .write = net_write
};


typedef struct net_info
{
    struct cdev sCharDevice;
    char* pszBuffer;
}SNET_INFO,
    *PSNET_INFO,
        **PPSNET_INFO;


#define NET_INFO_BUFFER_SIZE 100
