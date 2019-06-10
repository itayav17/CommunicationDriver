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
