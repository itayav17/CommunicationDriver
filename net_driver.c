#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "net_driver.h"
#include "net_types.h"
#include "net_defs.h"


MODULE_AUTHOR("Itay Avraham - https://www.linkedin.com/in/itay-avraham-76996b51");
MODULE_DESCRIPTION("A simple linux char driver");
MODULE_LICENSE("NA");


//
// Static variables
//

// Counts the number of times the device is opened.
static unsigned int s_uNumberOpens = 0;

//
// Global variables.
//

// Network info (cdev struct is included).
SNET_INFO m_sNetInfo[NET_MAX_MINOR];


ssize_t net_read(struct file* psFile, char __user* pszUserBuffer,
                 size_t iCount, loff_t* puOffsetPointer)
{
    char* pszBuffer = 
        (char*)psFile->private_data;
    ssize_t iLen =
        iCount - *puOffsetPointer;


    if (iLen <= 0)
        return 0;

    // Read data from a buffer in net_info
    //   to user buffer
    if (copy_to_user(psFile->private_data,(void*)(pszBuffer + *puOffsetPointer),
                        iLen))
        return -EFAULT;

    // Advanced the counter.
    *puOffsetPointer +=
        iLen;

    return iLen; 
}




ssize_t net_write(struct file* psFile, const char __user* pszUserBuffer,
                  size_t iCount, loff_t* puOffsetPointer)
{
    char* pszBuffer = 
        (char*)psFile->private_data;
    ssize_t iLen =
        iCount - *puOffsetPointer;

    if (iLen <= 0)
        return 0;

    // Read data from a buffer in net_info
    //   to user buffer.
    if (copy_to_user((void*)(pszBuffer + *puOffsetPointer),psFile->private_data,
                        iLen))
        return -EFAULT;

    // Advanced the counter.
    *puOffsetPointer +=
        iLen;

    return iLen;
}




ERROR net_open(struct inode* psInode, struct file* psFile)
{
    PSNET_INFO psNetInfo = NULL;

    // Advanced the counter.
    s_uNumberOpens++;

    // Get the net info.
    psNetInfo =
        container_of(psInode->i_cdev,SNET_INFO,
                        sCharDevice);

    // Allocate buffer.
    psNetInfo->pszBuffer =
        (char*)kmalloc(NET_INFO_BUFFER_SIZE,GFP_KERNEL);

    // Save the private data.
    // Don't forget to free in
    //   the release method.
    psFile->private_data =
        psNetInfo->pszBuffer;
                        
    return EOK;
}




ERROR net_release(struct inode* psInode, struct file* psFile)
{
    PSNET_INFO psNetInfo = NULL;


    // Get the net info.
    psNetInfo =
        container_of(psInode->i_cdev,SNET_INFO,
                        sCharDevice);

    // Allocate buffer.
    kfree((void*)psNetInfo->pszBuffer);

    return EOK;
}




ERROR net_register_driver(void)
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




ERROR net_cdev_init(void)
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

        // Member initialization.
        m_sNetInfo[uCounter].sCharDevice.owner =
            THIS_MODULE;
        m_sNetInfo[uCounter].sCharDevice.ops =
            &net_file_operations;
        
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
    if ((iError = net_register_driver() != EOK))
        // Register failed.
        goto ReturnOnError;

    // Associated the cdev structure(represent char
    //   device) to the file_operation
    //   structure. 
    if ((iError = net_cdev_init() != EOK))
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
