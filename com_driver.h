// Static variables.
#define STATIC_ALLOC            0
#define MAJOR_NUMBER            0
#define FIRST_MINOR_NUMBER      0
#define MINOR_COUNT_NUMBER      1
#define KERNEL_BUFFER_SIZE      1024
#define EOK                     0

typedef int                 ERROR;

#define COM_WRITE_VALUE _IOW('a','a',int32_t*)
#define COM_READ_VALUE _IOR('a','b',int32_t*)


ssize_t com_read(struct file* ps_file, char __user* psz_user_buffer,
                 size_t i_count, loff_t* pu_offset_pointer);

ssize_t com_write(struct file* ps_file, const char __user* psz_user_buffer,
                  size_t i_count, loff_t* pu_offset_pointer);

ERROR com_open(struct inode* ps_inode, struct file* ps_file);

ERROR com_release(struct inode* ps_inode, struct file* ps_file);

static long etx_ioctl(struct file* s_file, unsigned int i_cmd,
                      unsigned long ul_arg);

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
    .write = com_write,
    .unlocked_ioctl = etx_ioctl
};
