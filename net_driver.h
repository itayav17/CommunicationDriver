#include "net_types.h"


//
// Net file operations functions.
//

// Prepare any initialization for later operation.
ERROR net_open(struct inode* psInode, struct file* psFile);

// Shut down the device and deallocate anything that
//   open function allocated. 
ERROR net_release(struct inode* psInode, struct file* psFile);

// Read buffer from the device to the user space.
ssize_t net_read(struct file* psFile, char __user* pszUserBuffer,
                 size_t iCount, loff_t* puOffsetPointer);

// Write buffer from the user space to the device.
ssize_t net_write(struct file* psFile, const char __user* pszUserBuffer,
                  size_t iCount, loff_t* puOffsetPointer);


