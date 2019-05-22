#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>


MODULE_AUTHOR("Itay Avraham - https://www.linkedin.com/in/itay-avraham-76996b51");
MODULE_DESCRIPTION("Network packet analayzer");


static int init(void)
{
    printk("Hello");
}




static int clear(void)
{
    printk("Bye");
}

module_init(init);
module_exit(clear);

