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


// Static variables.
static int s_iNetMajor = 0;
static int s_iNetMinor = 1;
static int s_iNetMaxMinor = 5;


static int net_driver_init(void)
{
    dev_t uVersion;
    ERROR iError;
    

    // Major version is supplied ?
    if (s_iNetMajor)
    {
        // TODO:
        // Write a script that supplied these major
        //   version number.

        // From kernel 2.6 the major and minor version
        //   are combined to 32 bit: MSB 12 bit
        //   represents the major.
        uVersion = 
            MKDEV(s_iNetMajor,0);
        
        // Register the device class (statically assignment).
        if ((iError = register_chrdev_region(uVersion,s_iNetMaxMinor,
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
        iError = alloc_chrdev_region(&uVersion,s_iNetMinor,
                                            s_iNetMaxMinor,"Net driver by Itay Avraham");
        
        // If the iError is less than 0, then the allocation
        //   failed to allocate a major number.
        if (iError < 0)
        {
            // Failed to register.
            //print_error("Failed to register driver (dynamically assignment).");
            goto ReturnOnError;
        }

        // Get the major number.
        s_iNetMajor =
            MAJOR(uVersion);

        // Print the major number.
        print_debug("Major version is:%d\n",s_iNetMajor);
    }

    return EOK;

ReturnOnError:

    return iError;

}




static void net_driver_exit(void)
{


}







module_init(net_driver_init);
module_exit(net_driver_exit);
