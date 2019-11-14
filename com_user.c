#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>

int8_t i8_write_buffer[1024];
int8_t i8_read_buffer[1024];

#define COM_WRITE_VALUE _IOW('a','a',int32_t*)
#define COM_READ_VALUE _IOR('a','b',int32_t*)

int main()
{
    int i_file_descriptor;
    int32_t i32_value;
    char c_option;

    // Open the device.
    i_file_descriptor = 
        open("/dev/com_driver_device", O_RDWR);

    // FD is valid ?
    if(i_file_descriptor < 0)
    {
        printf("Cannot open device file.\n");
        return 0;
    }

    while(1)
    {
        printf("****Please Enter the Option******\n");
        printf("        1. Write                 \n");
        printf("        2. Read                  \n");
        printf("        3. Write value by ioctl  \n");
        printf("        4. Read value by ioctl   \n");
        printf("        5. Exit                  \n");
        printf("*********************************\n");
        scanf(" %c", &c_option);
        printf("Your Option = %c\n", c_option);
        
        switch(c_option)
        {
            case '1':
                printf("Enter the string to write into driver :");
                scanf("  %[^\t\n]s", i8_write_buffer);
                printf("Data Writing ...");
                write(i_file_descriptor, i8_write_buffer, strlen(i8_write_buffer)+1);
                printf("Done!\n");
                break;
            case '2':
                printf("Data Reading ...");
                read(i_file_descriptor, i8_read_buffer, 1024);
                printf("Done!\n\n");
                printf("Data = %s\n\n", i8_read_buffer);
                break;
            case '3':
                printf("Enter the value to write into driver by sending ioctl command:");
                scanf("%d", &i32_value);
                printf("Data Writing ...");
                ioctl(i_file_descriptor, COM_WRITE_VALUE, (int32_t*)&i32_value);
                printf("Done!\n");
                break;
            case '4':
                printf("Value Reading by sending ioctl command...");
                ioctl(i_file_descriptor, COM_READ_VALUE, (int32_t*)&i32_value);
                printf("Done!\n\n");
                printf("Data = %d\n\n", i32_value);
                break;
            case '5':
                printf("Closing Driver\n");                
                close(i_file_descriptor);
                exit(1);
                break;
            default:
                printf("Enter Valid option = %c\n",c_option);
                break;
        }
    }

    // Close the device.
    close(i_file_descriptor);
}
