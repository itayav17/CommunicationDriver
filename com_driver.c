#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include "com_driver.h"


MODULE_AUTHOR("Itay Avraham");
MODULE_DESCRIPTION("A simple linux char driver");
MODULE_LICENSE("GPL");

//
// Global variables.
//

// Device major and minor number.
dev_t u_dev_t;

static struct class *dev_class;

// communication info (cdev struct is included).
SCOM_INFO s_com_info[MINOR_COUNT_NUMBER];

//
// Passing arguments to device driver.
//
int i_value_param = 0;
module_param(i_value_param, int, S_IRUSR|S_IWUSR);

//
// File operation functions
//

ssize_t com_read(struct file* ps_file, char __user* psz_user_buffer,
                 size_t i_count, loff_t* pu_offset_pointer)
{
    char* psz_buffer = 
        (char*)ps_file->private_data;
    ssize_t i_len =
        i_count - *pu_offset_pointer;


    if (i_len <= 0)
        return 0;

    // Read data from a buffer in com_info
    //   to user buffer
    if (copy_to_user(ps_file->private_data,(void*)(psz_buffer + *pu_offset_pointer),
                        i_len))
        return -EFAULT;

    // Advanced the counter.
    *pu_offset_pointer +=
        i_len;

    return i_len; 
}




ssize_t com_write(struct file* ps_file, const char __user* psz_user_buffer,
                  size_t i_count, loff_t* pu_offset_pointer)
{
    char* psz_buffer = 
        (char*)ps_file->private_data;
    ssize_t i_len =
        i_count - *pu_offset_pointer;

    if (i_len <= 0)
        return 0;

    // Read data from a buffer in com_info
    //   to user buffer.
    if (copy_to_user((void*)(psz_buffer + *pu_offset_pointer),ps_file->private_data,
                        i_len))
        return -EFAULT;

    // Advanced the counter.
    *pu_offset_pointer +=
        i_len;

    return i_len;
}




ERROR com_open(struct inode* ps_inode, struct file* ps_file)
{
    PSCOM_INFO ps_com_info = NULL;

    // Advanced the counter.
    u_open_devices++;

    // Get the net info.
    ps_com_info =
        container_of(ps_inode->i_cdev,SCOM_INFO,
                        s_cdev);

    // Allocate buffer.
    ps_com_info->psz_buffer =
        (char*)kmalloc(INFO_BUFFER_SIZE,GFP_KERNEL);

    // Save the private data.
    // Don't forget to free in
    //   the release method.
    ps_file->private_data =
        ps_com_info->psz_buffer;
                        
    return EOK;
}




ERROR com_release(struct inode* ps_inode, struct file* ps_file)
{
    PSCOM_INFO ps_com_info = NULL;


    // Get the com info.
    ps_com_info =
        container_of(ps_inode->i_cdev,SCOM_INFO,
                        s_cdev);

    // Allocate buffer.
    kfree((void*)ps_com_info->psz_buffer);

    return EOK;
}




ERROR com_register_driver(void)
{
    ERROR i_error;

    
    // Major version is supplied ?
    if (STATIC_ALLOC)
    {
        // TODO:
        // Write a script that supplied these major
        //   version number.

        // From kernel 2.6 the major and minor version
        //   are combined to 32 bit: MSB 12 bit
        //   represents the major.
        u_dev_t = 
            MKDEV(MAJOR_NUMBER,0);
        
        // Register the device class (statically assignment).
        if ((i_error = register_chrdev_region(u_dev_t,MINOR_COUNT_NUMBER,
                                             "com_driver")) != EOK);
        {
            // Failed to register.
            // print_error("Failed to register device driver (statically assignment)");
            goto ReturnOnError;
        }
    } 
    else
    {
        // Register the device class (dynamically assignment).
        i_error = alloc_chrdev_region(&u_dev_t,FIRST_MINOR_NUMBER,
                                        MINOR_COUNT_NUMBER,"com_driver");
        
        // If the iError is less than 0, then the allocation
        //   failed to allocate a major number.
        if (i_error < 0)
        {
            // Failed to register.
            //print_error("Failed to register driver (dynamically assignment).");
            goto ReturnOnError;
        }

        // Print the major and minor number.
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(u_dev_t), MINOR(u_dev_t));
    }

    return EOK;

ReturnOnError:

    return i_error;
}




ERROR com_cdev_init(void)
{
    unsigned int u_number_of_devices = 1;
    unsigned int u_counter;
    ERROR i_error;


    // Link the character device to the driver.
    for(u_counter = 0;
            u_counter < MINOR_COUNT_NUMBER;
            u_counter++)
    {
        // Initialize the char device to the
        //   file opeartion fields.
        cdev_init(&s_com_info[u_counter].s_cdev,&com_file_operations);

        // Member initialization.
        s_com_info[u_counter].s_cdev.owner =
            THIS_MODULE;
        s_com_info[u_counter].s_cdev.ops =
            &com_file_operations;
        
        // Add the device to the system.
        if ((i_error = 
                cdev_add(&s_com_info[u_counter].s_cdev,MKDEV(MAJOR_NUMBER, u_counter),
                            u_number_of_devices)) < 0)
        {
            printk(KERN_ERR "Failed to add the device to the system\n");

            i_error = -ENOENT;
            goto ReturnOnError;
        }
    }

    return EOK;

ReturnOnError:

    return i_error;
}




static int com_driver_init(void)
{
    ERROR i_error;
    
    printk(KERN_INFO "%s - %s:%d",__FILE__,__FUNCTION__, __LINE__);
    printk(KERN_INFO "Argument value in init:%d",i_value_param);
    
    // Register the driver.
    if ((i_error = com_register_driver() != EOK))
        // Register failed.
        goto ReturnOnErrorRegister;

    /*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"com_driver_class")) == NULL)
    {
        printk(KERN_INFO "Cannot create the struct class for device.\n");
        goto ReturnOnErrorDevice;
    }

    // Creating device.
    if((device_create(dev_class,NULL,u_dev_t,NULL,"com_driver_device")) == NULL)
    {
        printk(KERN_INFO "Cannot create the Device.\n");
        goto ReturnOnErrorDevice;
    }

    // Associated the cdev structure(represent char
    //   device) to the file_operation
    //   structure. 
    //if ((i_error = com_cdev_init() != EOK))
        // Register failed.
      //  goto ReturnOnError;


    return EOK;


ReturnOnErrorDevice:


    // Delete the device file.
    class_destroy(dev_class);


ReturnOnErrorRegister:


    // Unregister the driver.
    unregister_chrdev_region(u_dev_t,MINOR_COUNT_NUMBER);

    return i_error;
}




static void com_driver_exit(void)
{
    unsigned int uCounter;

    printk(KERN_INFO "On exit function: size of opened devices is %d and argument value:%d.\n",
           u_open_devices, i_value_param);

    // Destroy the device file.
    device_destroy(dev_class,u_dev_t);
    class_destroy(dev_class);

    /*
    for(uCounter = 0;
            uCounter < MINOR_COUNT_NUMBER;
            uCounter++)
        // Release all the devices.
        cdev_del(&s_com_info[uCounter].s_cdev);
*/
    // Unregister the driver.
    unregister_chrdev_region(u_dev_t,MINOR_COUNT_NUMBER);
}


module_init(com_driver_init);
module_exit(com_driver_exit);
