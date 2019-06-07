// Static variables.
#define NET_MAJOR 0
#define NET_MINOR 1
#define NET_MAX_MINOR 5

#define print_debug(msg, ...) \
    printk(KERN_DEBUG "%s:%d - %s(). " msg ".\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);

#define print_error(msg, ...) \
    printk(KERN_DEBUG "%s:%d - %s(). " msg ".\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);


