#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>

#define DEVICE_FIRST 0
#define DEVICE_COUNT 3
#define DGROUP_NAME "mydummy_driver_group_cdev"

static int major = 0;

static int __init cdrv_init(void);
static void __exit cdrv_exit(void);

module_init(cdrv_init);
module_exit(cdrv_exit);

//Создание региона
static int __init cdrv_init(void) { 
    int result = 0;

    printk(KERN_INFO "Trying to register my_dummy_device region\n");
	dev_t dev = 0;
	result = alloc_chrdev_region(&dev, DEVICE_FIRST, DEVICE_COUNT, DGROUP_NAME);
	major = MAJOR(dev);

    if(result < 0) {
        unregister_chrdev_region(MKDEV(major, DEVICE_FIRST), DEVICE_COUNT);
        printk(KERN_INFO "Can not register my_dummy_device region\n");
        goto err;
    }
    printk(KERN_INFO "my_dummy_device region created: %d:%d...%d\n", major, DEVICE_FIRST, DEVICE_COUNT);


err:
    return result;
}

//освобождение региона
static  void __exit cdrv_exit(void){
	unregister_chrdev_region(MKDEV(major, DEVICE_FIRST), DEVICE_COUNT);
  printk(KERN_INFO "my_dummy_device region %d:%d...%d destroyed\n", major, DEVICE_FIRST, DEVICE_COUNT);
}


MODULE_LICENSE("GPL");
