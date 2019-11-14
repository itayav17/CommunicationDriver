// Static variables.
#define STATIC_ALLOC            0
#define MAJOR_NUMBER            0
#define FIRST_MINOR_NUMBER      0
#define MINOR_COUNT_NUMBER      5
#define INFO_BUFFER_SIZE        100
#define EOK                     0

typedef int                 ERROR;

ssize_t com_read(struct file* ps_file, char __user* psz_user_buffer,
                 size_t i_count, loff_t* pu_offset_pointer);

ssize_t com_write(struct file* ps_file, const char __user* psz_user_buffer,
                  size_t i_count, loff_t* pu_offset_pointer);

ERROR com_open(struct inode* ps_inode, struct file* ps_file);

ERROR com_release(struct inode* ps_inode, struct file* ps_file);

//
// Static variables
//

// Counts the number of times the device is opened.
static unsigned int u_open_devices = 0;


//
// Structure definitions.
//

typedef struct com_info
{
    struct cdev s_cdev;
    char* psz_buffer;
}SCOM_INFO,
    *PSCOM_INFO;


struct file_operations com_file_operations =
{
    .owner = THIS_MODULE,
    .open = com_open,
    .release = com_release,
    .read = com_read,
    .write = com_write
};
