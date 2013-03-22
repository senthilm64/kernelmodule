#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<asm/uaccess.h>

#define device_name "chardev"
#define buf_len 80

static int major;
static int Device_open=0;
static char msg[buf_len];
static char *msg_ptr;

static int device_open(struct inode *,struct file *);
static int device_release(struct inode *,struct file *);
static ssize_t device_read(struct file *,char __user *,size_t ,loff_t *);
static ssize_t device_write(struct file *,const char __user *,size_t ,loff_t *);

struct file_operations fops=
  {
    .read=device_read,
    .write=device_write,
    .open=device_open,
    .release=device_release
  };

int init_module(void)
{
  major=register_chrdev(0,device_name,&fops);
  if(major<0)
    {
      printk(KERN_ALERT "registered char device is failed with major no %d ",major);
      return major;
}
  printk(KERN_INFO "/mkdev/%s -c %d \n",device_name,major);
  return 0;
}

void exit_module(void)
{
 unregister_chrdev(major,device_name);
}

int device_open(struct inode *inode,struct file *filp)
{
  static int count;
  if(Device_open)
    return -EBUSY;
  Device_open++;
  msg_ptr=msg;
  sprintf(msg,"Hello world %d\n",count++);
  try_module_get(THIS_MODULE);
  return 0;
}

int device_close(struct inode *inode,struct file *filp)
{
  Device_open--;
  module_put(THIS_MODULE);
  return 0;
}

ssize_t device_read(struct file *filp,char __user *buffer,size_t length,loff_t *offset)
{
  int bytes_read = 0;
  if(*msg_ptr==0)
    return 0;
  while(length && *msg_ptr)
    {
      put_user(*(msg_ptr++),buffer++);
      length--;
      bytes_read++;
}
  return bytes_read;
}

ssize_t device_write(struct file *filp,const char __user *buffer,size_t length,loff_t *offset)
{
  printk(KERN_ALERT "sorry this operation isnt supported\n");
  return -EINVAL;
}

