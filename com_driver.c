#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include<linux/proc_fs.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include "com_driver.h"


MODULE_AUTHOR("Itay Avraham");
MODULE_DESCRIPTION("A simple linux char driver");
MODULE_LICENSE("GPL");

//
// Global variables.
//
dev_t u_dev_t;
static struct class* dev_class;
uint8_t* u8_kernel_buffer;
int32_t i32_value = 0;

// Wait queue
static struct task_struct* wait_thread;
wait_queue_head_t wait_queue_etx;
int wait_queue_flag = 0;

// communication info (cdev struct is included).
SCOM_INFO s_com_info[MINOR_COUNT_NUMBER];

//
// Passing arguments to device driver.
//
int i_value_param = 0;
module_param(i_value_param, int, S_IRUSR|S_IWUSR);


static int wait_function(void *unused)
{
    // That thread will always wait for the event.
    while(true)
    {
        printk(KERN_INFO "Waiting For Event...\n");

        // sleep until a condition gets true.
        wait_event_interruptible(wait_queue_etx, wait_queue_flag != 0 );

        // Interrupt arrive from the exit function?
        if(wait_queue_flag == 2)
        {
            printk(KERN_INFO "Event Came From Exit Function\n");
            return 0;
        }
        
        printk(KERN_INFO "Event Came From Read Function.\n");

        wait_queue_flag = 0;
    }

    return 0;
}




//
// File operation functions
//

static long etx_ioctl(struct file* s_file, unsigned int i_cmd,
                      unsigned long ul_arg)
{
    unsigned long u_return_bytes;

    
    switch(i_cmd)
    {
        case COM_WRITE_VALUE:
            
            printk(KERN_INFO "Driver ioctl (COM_WRITE_VALUE) Function Called...!!!\n");

            u_return_bytes =
                copy_from_user(&i32_value ,(int32_t*)ul_arg, sizeof(int32_t));

            printk(KERN_INFO "Value = %d\n", i32_value);

            break;

        case COM_READ_VALUE:

            printk(KERN_INFO "Driver ioctl (COM_READ_VALUE) Function Called...!!!\n");

            u_return_bytes =
                copy_to_user((int32_t*)ul_arg, &i32_value, sizeof(int32_t));

            break;

        default:

            printk(KERN_INFO "Driver ioctl (INVALID COMMAND) Function Called...!!!\n");
    }

    return 0;
}




ssize_t com_read(struct file* ps_file, char __user* psz_user_buffer,
                 size_t i_size, loff_t* pu_offset_pointer)
{
    unsigned long u_return_bytes;

    printk(KERN_INFO "Driver read Function Called...!!!\n");

    u_return_bytes = copy_to_user(psz_user_buffer, u8_kernel_buffer, i_size);
    
    // Wake up a process.
    wait_queue_flag = 1;
    wake_up_interruptible(&wait_queue_etx);

    return 0;
}




ssize_t com_write(struct file* ps_file, const char __user* psz_user_buffer,
                  size_t i_size, loff_t* pu_offset_pointer)
{
    unsigned long u_return_bytes;

    printk(KERN_INFO "Driver write Function Called...!!!\n");
    
    u_return_bytes = copy_from_user(u8_kernel_buffer, psz_user_buffer, i_size);
    
    printk(KERN_INFO "Data Read : Done!\n");

    return i_size;
}




ERROR com_open(struct inode* ps_inode, struct file* ps_file)
{
    printk(KERN_INFO "Driver open Function Called...!!!\n");

    // Allocating physical memory.
    if((u8_kernel_buffer =
            kmalloc(KERNEL_BUFFER_SIZE, GFP_KERNEL)) == 0)
    {
        printk(KERN_INFO "Cannot allocate memory in kernel.\n");
        return -ENOMEM;
    }

    return EOK;
}




ERROR com_release(struct inode* ps_inode, struct file* ps_file)
{
    printk(KERN_INFO "Driver release Function Called...!!!\n");

    // Deallocate buffer.
    kfree((void*)u8_kernel_buffer);

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
        
        // If i_error is less than 0, then the allocation
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
        //   file operation fields.
        cdev_init(&s_com_info[u_counter].s_cdev,&com_file_operations);

        // Member initialization.
        s_com_info[u_counter].s_cdev.owner =
            THIS_MODULE;
        s_com_info[u_counter].s_cdev.ops =
            &com_file_operations;
        
        // Add the device to the system.
        if ((i_error = 
                cdev_add(&s_com_info[u_counter].s_cdev,MKDEV(MAJOR(u_dev_t), u_counter),
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

    // Associated the cdev structure(represent char
    //   device) to the file_operation
    //   structure. 
    if ((i_error = com_cdev_init() != EOK))
        // Register failed.
        goto ReturnOnErrorDevice;

    // Creating struct class.
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

    //Initialize wait queue
    init_waitqueue_head(&wait_queue_etx);

    // Create the kernel thread with name 'mythread'
    wait_thread = kthread_create(wait_function, NULL, "WaitThread");

    if (wait_thread)
    {
        printk("Thread Created successfully\n");
        
        // Wake up the process.
        wake_up_process(wait_thread);
    }

    else
    {
        printk(KERN_INFO "Thread creation failed\n");
        goto ReturnOnErrorDevice;
    }

    // Creating Proc entry.
    proc_create("com_driver_proc",0666,NULL,&com_file_operations);

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

    // Change the flag.
    wait_queue_flag = 2;

    // Wake up only one process from the wait queue.
    // NOTE: this process must be in interruptible sleep.
    wake_up_interruptible(&wait_queue_etx);

    // Remove the proc entry.
    remove_proc_entry("com_driver_proc",NULL);

    // Destroy the device file.
    device_destroy(dev_class,u_dev_t);
    class_destroy(dev_class);

    for(uCounter = 0;
            uCounter < MINOR_COUNT_NUMBER;
            uCounter++)
        // Release all the devices.
        cdev_del(&s_com_info[uCounter].s_cdev);

    // Unregister the driver.
    unregister_chrdev_region(u_dev_t,MINOR_COUNT_NUMBER);
}


module_init(com_driver_init);
module_exit(com_driver_exit);
