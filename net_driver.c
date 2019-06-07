#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include "net_types.h"
#include "net_macros.h"

MODULE_AUTHOR("Itay Avraham - https://www.linkedin.com/in/itay-avraham-76996b51");
MODULE_DESCRIPTION("A simple linux char driver");
MODULE_LICENSE("NA");


struct file_operations net_file_operations =
{/*
    .owner = THIS_MODULE,
    .llseek = net_llseek,
    .read = net_read,
    .write = net_write,
    .ioctl = net_ioctl,
    .open = net_open,
    .release = net_release*/
};


struct net_info
{
    struct cdev sCharDevice;
};




// Global variables.
struct net_info m_sNetInfo[NET_MAX_MINOR];


ERROR register_driver(void)
{
    dev_t uVersion;
    int iMajor;
    int iMinor;
    ERROR iError;

    
    // Major version is supplied ?
    if (NET_MAJOR)
    {
        // TODO:
        // Write a script that supplied these major
        //   version number.

        // From kernel 2.6 the major and minor version
        //   are combined to 32 bit: MSB 12 bit
        //   represents the major.
        uVersion = 
            MKDEV(NET_MAJOR,0);
        
        // Register the device class (statically assignment).
        if ((iError = register_chrdev_region(uVersion,NET_MAX_MINOR,
                                             "Net driver by Itay Avraham")) != EOK);
        {
            // Failed to register.
            //print_error("Failed to register device driver (statically assignment)");
            goto ReturnOnError;
        }
    } 
    else
    {
        // Register the device class (dynamically assignment).
        iError = alloc_chrdev_region(&uVersion,NET_MINOR,
                                        NET_MAX_MINOR,"Net driver by Itay Avraham");
        
        // If the iError is less than 0, then the allocation
        //   failed to allocate a major number.
        if (iError < 0)
        {
            // Failed to register.
            //print_error("Failed to register driver (dynamically assignment).");
            goto ReturnOnError;
        }

        // Get the major and minor number.
        iMajor =
            MAJOR(uVersion);
        iMinor =
            MINOR(uVersion);

        // Print the major and minor number.
        print_debug("Major version is:%d, Minor version is:%d",NET_MAJOR,NET_MINOR);
    }

    return EOK;

ReturnOnError:

    return iError;
}




ERROR link_device_to_opeations(void)
{
    unsigned int uNumberOfDevice = 1;
    unsigned int uCounter;
    ERROR iError;


    // Link the character device to the driver.
    for(uCounter = 0;
            uCounter < NET_MAX_MINOR;
            uCounter++)
    {
        // Initialize the char device to the
        //   file opeartion fields.
        cdev_init(&m_sNetInfo[uCounter].sCharDevice,&net_file_operations);
        
        // Add the device to the system.
        if ((iError = 
                cdev_add(&m_sNetInfo[uCounter].sCharDevice,MKDEV(NET_MAJOR, uCounter),
                            uNumberOfDevice)) < 0)
        {
            print_error("Failed to add the device to the system");

            iError = -ENOENT;
            goto ReturnOnError;
        }
    }

    return EOK;

ReturnOnError:

    return iError;
}




static int net_driver_init(void)
{
    ERROR iError;
    
    // Register the driver.
    if ((iError = register_driver() != EOK))
        // Register failed.
        goto ReturnOnError;

    // Associated the cdev structure(represent char
    //   device) to the file_operation
    //   structure. 
    if ((iError = link_device_to_opeations() != EOK))
        // Register failed.
        goto ReturnOnError;

    

    return EOK;

ReturnOnError:

    return iError;
}




static void net_driver_exit(void)
{
    unsigned int uCounter;

    for(uCounter = 0;
            uCounter < NET_MAX_MINOR;
            uCounter++)
        // Release all the devices.
        cdev_del(&m_sNetInfo[uCounter].sCharDevice);

    // Remove allocation of device identifiers.
    unregister_chrdev_region(MKDEV(NET_MAJOR,0),NET_MAX_MINOR);
}


module_init(net_driver_init);
module_exit(net_driver_exit);
