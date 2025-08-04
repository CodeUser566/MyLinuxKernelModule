#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/version.h> 

#define DEVICE_FIRST 0
#define DEVICE_COUNT 1
#define DGROUP_NAME "mydummy_driver_group_cdev"
#define DEVICE_NAME "mydummy"
#define CLASS_NAME "mydummy_class"
#define EOK 0

static int major = 0;
static int device_open = 0;
static struct class *dummy_class = NULL;
static struct device *dummy_device = NULL;
static dev_t dev = 0;

static struct cdev my_dummy_device;

static int dummy_open(struct inode *n, struct file *f);
static int dummy_release(struct inode *n, struct file *f);
static ssize_t dummy_read(struct file * f, char __user * buffer, size_t counter, loff_t * ppos);

static const struct file_operations dummy_fops = {
    .owner   = THIS_MODULE,
    .open    = dummy_open,
    .release = dummy_release,
    .read    = dummy_read,
};

static int __init dummy_init(void) { 

    int result = 0;
    
    printk(KERN_INFO "Trying to register my_dummy_device region\n");

//Регистрация диапазона устройств
    result = alloc_chrdev_region(&dev, DEVICE_FIRST, DEVICE_COUNT, DGROUP_NAME);
    if(result < 0) {
        printk(KERN_ERR "Can not register device region: %d\n", result);
        return result;
    }
    
    major = MAJOR(dev);
    printk(KERN_INFO "Device region registered: major %d, minor %d\n", major, DEVICE_FIRST);

//Инициализация cdev
    cdev_init(&my_dummy_device, &dummy_fops);
    my_dummy_device.owner = THIS_MODULE;
    
//Добавление устройства в систему
    result = cdev_add(&my_dummy_device, dev, DEVICE_COUNT);
    if (result < 0) {
        printk(KERN_ERR "Cannot add my_dummy_device: %d\n", result);
        goto err_cdev_add;
    }

//Создание класса устройств
    #if LINUX_VERSION_CODE < KERNEL_VERSION(6,4,0)
        dummy_class = class_create(THIS_MODULE, CLASS_NAME);
    #else
        dummy_class = class_create(CLASS_NAME);
    #endif
    
    if (IS_ERR(dummy_class)) {
        result = PTR_ERR(dummy_class);
        printk(KERN_ERR "Cannot create my_dummy_device class: %d\n", result);
        goto err_class;
    }

//Создание файла устройства в /dev
    dummy_device = device_create(dummy_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(dummy_device)) {
        result = PTR_ERR(dummy_device);
        printk(KERN_ERR "Cannot create device file: %d\n", result);
        goto err_device;
    }

    printk(KERN_INFO "my_dummy_device file created: /dev/%s\n", DEVICE_NAME);
    return EOK;
    
    

// Обработка ошибок
err_device:
    class_destroy(dummy_class);
err_class:
    cdev_del(&my_dummy_device);
err_cdev_add:
    unregister_chrdev_region(dev, DEVICE_COUNT);
    return result;
}

static void __exit dummy_exit(void) {

    
//Удаление файла устройства
    if (!IS_ERR(dummy_device)) {
        device_destroy(dummy_class, dev);
    }
    
//Удаление класса
    if (!IS_ERR(dummy_class)) {
        class_destroy(dummy_class);
    }
    
//Удаление cdev
    cdev_del(&my_dummy_device);
    
//Освобождение номера устройства
    unregister_chrdev_region(dev, DEVICE_COUNT);
    
    printk(KERN_INFO "my_dummy_device destroyed!\n");
}

static int dummy_open(struct inode *n, struct file *f) {
    if (device_open) {
        printk(KERN_WARNING "my_dummy_device is busy\n");
        return -EBUSY;
    }
    
    device_open++;
    printk(KERN_INFO "my_dummy_device opened\n");
    return EOK;
}

static int dummy_release(struct inode *n, struct file *f) {
    device_open--;
    printk(KERN_INFO "my_dummy_device closed\n");
    return EOK;
}

static ssize_t dummy_read(struct file * fp, char __user * buf, size_t count, loff_t * ppos) {
    const char * reply = "Hello World!\n";
    size_t len = strlen(reply);
    size_t available = len - *ppos;

    if (available == 0) 
        return 0;
    
    if (count > available) 
        count = available;

    if (copy_to_user(buf, reply + *ppos, count)) 
        return -EFAULT;

    *ppos += count;
    return count;
}

module_init(dummy_init);
module_exit(dummy_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple dummy character device driver");
