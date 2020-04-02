

#include <linux/autoconf.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/device.h>
#include <asm/io.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/page-flags.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <generated/asm-offsets.h>
#include <linux/platform_device.h>
#include <mach/pm.h>
#include <mach/platform.h>
#include <linux/list.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include "mhal_clkm.h"
#include "mhal_clkm_reg.h"
#include "mhal_srampd_reg.h"
#include "mdrvclkm_io.h"
#include "mst_devid.h"
#include <linux/clkm.h>
#include <linux/miscdevice.h>

LIST_HEAD(clkm_log_root_list);

struct clkm_log
{

    struct list_head list;
    S32 handle;
    U32 src_index;

};



#define CMD_GET_HANDLE 1
#define CMD_SET_SOURCE 2


#define SUCCESS_SET_SRC 0
#define ERR_NOT_FOUND_TABLE -1
#define ERR_NOT_SUPPORT_SRC -2
#define ERR_NOT_HAVE_CLK_SEL -1

#define CLK_GATE_ENABLE 1
#define CLK_GATE_DISABLE 0

#define MOD_CLKM_NAME            "clkm_manager"
#define clkm_dump_name           "clkm_dump"
#define sram_pd_dump_name           "srampd_dump"
#define disable                               "disable"
#define enable                              "enable"
#define no_clk_gate                     "no clk gate"

#define ERR_HANDLE (-1)
#define NOT_FOUND (-1)

static DEFINE_MUTEX(clkm_mutex);

static unsigned enable_log = 0;

#if 0
static struct dentry *clkm_debugfs_dir_entry_root;
#endif


static s32 mdrv_clkm_open(struct inode * inode, struct file * file);
static s32 mdrv_clkm_release(struct inode *inode, struct file *file);
static long mdrv_clkm_ioctl(struct file * filp, unsigned int cmd, unsigned long arg);
static long mdrv_compat_clkm_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
char*  get_port_name(clock_table *table_array,S32 handle);
char* get_clk_source(s32 handle);
void  clkm_clear_log();
clock_table* mdrv_clkm_get_clock_table(S32 handle);
u16  get_clk_gate_status(clock_table *table);
void dump_sram_pd_info(char *reg_name);
U16 readregbyte2(U32 regbank,U32 regaddr);

typedef struct
{
    S32                         clkm_major;
    S32                         clkm_minor;
    struct cdev                 clkm_device;
    struct file_operations      clkm_file_operations;

} clkm;


static struct class *clkm_class;

static clkm clkm_dev=
{
    .clkm_major=               MDRV_MAJOR_CLKM,
    .clkm_minor=               MDRV_MINOR_CLKM,
    .clkm_device=
    {
        .kobj=                  {.name= MOD_CLKM_NAME, },
        .owner  =               THIS_MODULE,
    },
    .clkm_file_operations=
    {
    	 .open             =     mdrv_clkm_open,
        .unlocked_ioctl=          mdrv_clkm_ioctl,
     //   #ifdef CONFIG_COMPAT
      //  .compat_ioctl =         MDrv_Compat_Clkm_Ioctl,
      //  #endif
    },
};

static const struct file_operations clkm_fops = {
     .owner = THIS_MODULE,
      .poll = NULL,
      .unlocked_ioctl = mdrv_clkm_ioctl,
      .compat_ioctl = NULL,
       .mmap = NULL,
       .open = mdrv_clkm_open,
       .flush = NULL,
       .release = NULL,

};


static struct miscdevice clkm_miscdev = {
	.minor = MDRV_MINOR_CLKM,
	.name = "clkm",
	.fops = &clkm_fops
};

static struct kobject *clkm_log_kobj;
static struct proc_dir_entry * clkm_proc_file;
static struct proc_dir_entry * sram_pd_proc_file;

static ssize_t mdrv_clkm_enable_log_show(struct kobject *kobj, struct kobj_attribute *attr,
   char *buf)
{
    return sprintf(buf, "%d\n",enable_log);

}


static ssize_t mdrv_clkm_enable_log_store(struct kobject *kobj, struct kobj_attribute *attr,
    const char *buf, size_t count)
{

       unsigned int tmp;
       sscanf(buf, "%du", &tmp);

       enable_log = tmp;

       mutex_lock(&clkm_mutex);

        if(enable_log == 1 || enable_log == 0)
        {
              clkm_clear_log();
        }

	mutex_unlock(&clkm_mutex);

       return count;
}


static struct kobj_attribute enable_log_attribute =
 __ATTR(enable_log, 0644, mdrv_clkm_enable_log_show, mdrv_clkm_enable_log_store);


static struct attribute *attrs[] = {
 &enable_log_attribute.attr,
 NULL,
};


static struct attribute_group attr_group = {
 .attrs = attrs,
};



static s32 mdrv_show_clkm_log(struct seq_file *p, void *v)
{



          struct clkm_log *node;
          struct list_head *pos , *q;
	   char *name;
	   char *src_name;

	   mutex_lock(&clkm_mutex);



	   list_for_each_safe(pos, q, &clkm_log_root_list)
          {
		node = list_entry(pos, struct clkm_log, list);

              name = get_port_name(clock_table_array, node->handle);


	       if(name != NULL)
	       {
                 seq_printf(p, "port name=%s , src=%u",name,node->src_index);
	           seq_putc(p, '\n');
	       }

     	   }


           mutex_unlock(&clkm_mutex);


          return 0;
}

static s32 mdrv_clkm_log_open(struct inode *inode, struct file *file)
{
	return single_open(file, mdrv_show_clkm_log, NULL);
}


static const struct file_operations clkm_log_operations = {
	.open		= mdrv_clkm_log_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};


s32 clkm_dump_procfile_read(char* buffer, char ** buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{

      int ret;

       if(offset > 0)
	   ret = 0;
	else
	    ret = sprintf(buffer,"clkm dump\n");

	return ret;
}

static int  clkm_dump_procfile_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
        char *buf;
	 s32 handle;
	 clock_table *table;
	 u16 val;
	 char *src_name;
	 char *clk_gate_status;
	 int i;

        mutex_lock(&clkm_mutex);

	 if(count == 1)
	     goto end;

        buf = kzalloc(count-1, GFP_KERNEL);

	 if ( copy_from_user(buf, (char*)buffer , count-1) )
	 {
	       goto end;
	  }

	   // buf[count] = '\0';


	   if(strcmp(buf , "all_clk") == 0)
	   {


                clock_table_param *param = &clkm_param;
                clock_table *ptr = param->clock_table_array_ref;


                for(i=0;i<param->num;i++)
	         {
	             src_name = get_clk_source(ptr[i].handle);

                   if(get_clk_gate_status(&ptr[i])  != NO_GATE_REG)
                   {
                        if(get_clk_gate_status(&ptr[i])  == CLK_GATE_DISABLE)
		               clk_gate_status = disable;
	  	         else
		              clk_gate_status = enable;
                   }
		     else
		     {
		         clk_gate_status = no_clk_gate;
		     }

		     if(src_name == NULL)
	   	          printk(KERN_INFO "g_port_name=%s , g_src_name=no clk sel register ,clk_gate=%s\n",ptr[i].g_port_name,clk_gate_status);
	            else
	   	          printk(KERN_INFO "g_port_name=%s , g_src_name=%s ,clk_gate=%s\n",ptr[i].g_port_name,src_name,clk_gate_status);



      	         }


	   }
	   else
	   {

              handle = get_handle(buf);

		if(handle == ERR_HANDLE)
		{
		    printk(KERN_INFO "can not find %s\n",buf);
		    goto end;
		}

	       table = mdrv_clkm_get_clock_table(handle);
	       src_name = get_clk_source(handle);

               if(get_clk_gate_status(table)  != NO_GATE_REG)
               {

		     if(get_clk_gate_status(table)  == CLK_GATE_DISABLE)
		         clk_gate_status = disable;
		     else
		         clk_gate_status = enable;

               }
		 else
		 {
		     clk_gate_status = no_clk_gate;
		 }

	       if(src_name == NULL)
	   	   printk(KERN_INFO "g_port_name=%s , g_src_name= no clk sel register , clock_gate=%s\n",buf,clk_gate_status);
	       else
	   	  printk(KERN_INFO "g_port_name=%s , g_src_name=%s , clock_gate=%s\n",buf,src_name,clk_gate_status);




	   }



end:

         if(!buf)
             kfree(buf);

	   mutex_unlock(&clkm_mutex);



}


static const struct file_operations proc_clkm_dump_operations =
{
       .llseek		= NULL,
	.read		= NULL,
	.write		= clkm_dump_procfile_write,
	.open		= NULL,
	.release	= NULL,
};


static int  sram_pd_dump_procfile_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
        char *buf;

	if(count == 1)
	    goto end;

	 buf = kzalloc(count-1, GFP_KERNEL);

	 if ( copy_from_user(buf, (char*)buffer , count-1) )
	 {
	       kfree(buf);
		return -EFAULT;
	 }

         dump_sram_pd_info(buf);

end:

         if(!buf)
	     kfree(buf);

	 return count;

};


static const struct file_operations proc_sram_pd_dump_operations =
{
       .llseek		= NULL,
	.read		= NULL,
	.write		= sram_pd_dump_procfile_write,
	.open		= NULL,
	.release	= NULL,
};


sram_ip_info* get_sram_pd_info(char *ip_name)
{
       int i,j;
       sram_ip_info *info, *target_ip_info=NULL;

       for(i=0;i<SRAM_PD_TABLE_NUM;i++)
      	{
      	      info = &sram_pd_table_array[i];

	      for(j=0;j<sram_pd_table_array[i].sram_pd_ptr_num;j++)
	      	{
	      	     if(strcmp(ip_name,info[j].ip_name) == 0)
	      	     {
	      	           target_ip_info = &info[j];
	      	     }

	      	}

      	}


	return target_ip_info;

}


u32 get_sram_pd_val(char * ip_name)
{

       u32 val =0;
       u32 u32Low,u32High;
	u32 mask;
       sram_ip_info *info = get_sram_pd_info(ip_name);

       if(info == NULL)
	   	goto end;

	if(u32Low == u32High)
       {
	     mask = 0x1<<u32Low;
       }
       else
       {
	    mask = ( 1 << ( u32High + 1 ) ) - ( 1 << u32Low ) ;
       }


end:

	return val;

}



S32 get_src_name_offset(clock_table *table , char *src_name)
{
          S32 offset = NOT_FOUND;
          S32 i;

         if(table->g_src_name_ptr == NULL || src_name == NULL)
		  goto end;

          char **ptr = table->g_src_name_ptr;
          //printk(KERN_INFO "count=%d name=%s\n",table->src_num,src_name);
          if(strcmp(src_name, "") == 0)
		  goto end;

          for(i=0;i<table->src_num;i++)
          {
               if(strcmp(ptr[i],src_name) == 0)
               {

                     offset = i;
			break;
               }
          }

end:


          //printk("MDrv_Get_Src_Name_Offset offset=%d\n",offset);
          return offset;
}


char* get_port_name(clock_table *table_array,S32 handle)
{
      S32 i;
      clock_table *ptr = table_array;
      char *name = NULL;

      for(i=0;i<CLKM_TABLE_NUM;i++)
      {
           if(ptr[i].handle == handle)
           {
               name = ptr[i].g_port_name;
		 break;
           }

      }

      return name;

}


s32 get_handle(char *name)
{

      S32 handle = ERR_HANDLE;
      S32 i;
      clock_table_param *param = &clkm_param;
      clock_table *ptr = param->clock_table_array_ref;
      //printk("current name %s\n",name);



      for(i=0;i<param->num;i++)
	{

	       //printk("current name %s\n",ptr[i].g_port_name);

		if(strcmp(ptr[i].g_port_name,name) == 0)
		{
		    handle = ptr[i].handle;
		    //printk("get handle:%s\n", ptr[i].handle);
		    break;
		}
	}


       return handle;
}

EXPORT_SYMBOL(get_handle);

 s32  mdrv_get_handle(clock_table_param *clkm_param ,unsigned long arg)
{
	clock_table *ptr = clkm_param->clock_table_array_ref;

	CLKM_GetHandle_PARAM  gethandleparam;

	S32 handle = ERR_HANDLE;

	S32 ret = 0;

       char *name;

	S32 i;


	if(copy_from_user(&gethandleparam, (CLKM_GetHandle_PARAM __user *)arg, sizeof(CLKM_GetHandle_PARAM)))
	{
	     ret = EFAULT;
            goto end;
	}

	name = gethandleparam.s8_Handle_Name;


       handle = get_handle(name);
       //printk(KERN_ERR "MDrv_Get_Handle handle=%d\n",handle);

	if(handle == ERR_HANDLE)
	{
	     gethandleparam.s8_Handle_Name = ERR_HANDLE;
	     ret = ERR_NOT_FOUND_TABLE;
	}
	else
	{
	     gethandleparam.s32_Handle = handle;

	     if (copy_to_user((U8 *) arg, &gethandleparam, sizeof(CLKM_GetHandle_PARAM)))
                ret =  EFAULT;
	}

end:

	return ret;
}




clock_table* mdrv_clkm_get_clock_table(S32 handle)
{


       clock_table *ptr = clkm_param.clock_table_array_ref;
       clock_table *table = NULL;
	S32 i;

	//printk("search handle=%d\n",handle);

       if(handle < -1 || handle >= clkm_param.num)
       {
           goto end;
       }



       table = &ptr[handle];



end:

       return table;

}

void writeregbyte2range(U32 regbank,U32 regaddr, U16 u16Low,U16 u16High,U16 u16Val)
{

      S32 i;

       U16 u16Tmpval;
       unsigned long clkmreg = (unsigned long)_CLKM_REG_BASE +  (unsigned long)(regbank * 0x200) + (unsigned long)(regaddr<<2);
      //unsigned long clkmreg = (unsigned long)(_CLKM_REG_BASE +  RegAddr);

       U16 mask ;

	if(u16Low == u16High)
	{
	     mask = 0x1<<u16Low;
	}
	else
	{
	    mask = ( 1 << ( u16High + 1 ) ) - ( 1 << u16Low ) ;
	}

	u16Tmpval = (*(volatile unsigned short*)clkmreg);
       //printk("u16Val=%d, low=%d mask=%d\n",u16Val,u16Low,mask);
	//printk("WriteRegByte2Range orignal  reg = 0x%x u16Tmpval=0x%x\n",RegAddr,u16Tmpval);

       u16Tmpval = (u16Tmpval & ~mask) | ((u16Val << u16Low) & mask);

	//printk("WriteRegByte2Range reg = 0x%x u16Tmpval=0x%x\n",RegAddr,u16Tmpval);

	(*(volatile unsigned short*)clkmreg) = u16Tmpval;

}


U16 readregbyte2(U32 regbank,U32 regaddr)
{

     unsigned long clkmreg = (unsigned long)_CLKM_REG_BASE +  (unsigned long)(regbank * 0x200) + (unsigned long)(regaddr<<2);
      //unsigned long clkmreg = (unsigned long)(_CLKM_REG_BASE + RegAddr);
     return  (*(volatile U16*)clkmreg);
}

/*
 default unit:2byte
*/
u16 *readregrange(u32 bank,u32 reg,u32 read_count)
{

       int i;
       u16* buf;
	u32 riu_addr;
       buf = kmalloc(read_count, GFP_KERNEL);

	if(!buf)
	      goto end;

	for(i=0;i<read_count;i++)
	{
	     riu_addr = _CLKM_REG_BASE + bank * 0x200 + ((reg+i) <<2);
            buf[i] =  (*(volatile U16*)riu_addr);

	}


end:

	return buf;

}


unsigned get_value_range(unsigned short *ptr, unsigned start_bits , unsigned end_bits)
{

	u32 start_area = start_bits >> 4;
	u32 end_area = end_bits >> 4;
	u32 start_area_end;
	u32 end_area_end;
	u32 mask;
	u32 start_offset,end_offset;

	u32 start_val,end_val,final_val;

	if(start_area != end_area)
	{
		start_offset = start_bits & 0xff;

		if(start_offset == 0xff)
		{
			 mask = 0x1;
		}
		else
		{
			mask = ( 1 << ( 16 ) ) - ( 1 << start_offset ) ;
		}

        start_val = ptr[start_area];

		start_val = (start_val & mask) >> start_offset;

		//---------------------------

		unsigned end_offset = end_bits & 0xf;

		if(end_offset == 0x00)
		{
			 mask = 0x01;
		}
		else
		{
			mask = ( 1 << ( end_offset+1 ) ) - ( 1 << 0 ) ;
		}

        end_val = ptr[start_area+1];
        end_val = (end_val & mask);

		final_val = (end_val << (16-start_offset)) | start_val;

	}
	else
	{
		final_val = ptr[start_area];
		start_offset = start_bits & 0xf;
		end_offset = end_bits & 0xf;

		if(start_offset == end_offset)
		{
			 mask = 0x01;
		}
		else
		{
			mask = ( 1 << ( end_offset + 1 ) ) - ( 1 << start_offset ) ;
		}

		final_val = (final_val & mask) >> start_offset;

	}



	return final_val;

}


unsigned short get_reg_value(unsigned int bank,unsigned int reg,unsigned int offset)
{
     unsigned short val;
     u32 start_area = (offset >> 4);
     u32 start_reg = reg;
     u32 start_offset = (offset & 0xf);

     start_reg += start_area;

    val = readregbyte2(bank,start_reg);

    val >> start_offset;


    return val;
}



sram_pd_table* get_sram_pd_table(char *block_name)
{

       sram_pd_table *table = NULL;
	S32 i;

       if(!block_name || strcmp(block_name,"") == 0)
       {
           goto end;
       }


      for(i=0;i<SRAM_PD_TABLE_NUM;i++)
      {
            //printk(KERN_INFO "sram block name=%s\n",sram_pd_table_array[i].block_name);

             if(strcmp(block_name,sram_pd_table_array[i].block_name) == 0)
             {
                  printk(KERN_INFO "find sram block name=%s\n",sram_pd_table_array[i].block_name);
                  table = &(sram_pd_table_array[i]);
		    break;
             }
      }



end:

      return table;


}


/*
u16 *read_sram_pd_reg(char *reg_name)
{
      sram_pd_table* table;
      u16 *val_ptr = NULL;
      u32 div,remain;
      u32 range;
      int offset;
      table = get_sram_pd_table(reg_name);

      if(table == NULL)
	   goto end;



    range = table->sram_pd_ptr[table->sram_pd_ptr_num-1].end_offset  - table->sram_pd_ptr[0].start_offset + 1;

    div = range >> 4;
    remain = range & 0xf;

    if(remain != 0)
	 div++;

    val_ptr = readregrange(table->riu_bank,table->riu_reg,div);


 end:

    return val_ptr;

}
*/


void dump_sram_pd_info(char *block_name)
{
         sram_pd_table* table;
         int i;
	  u32 low,high;
	  u32 val;

        printk(KERN_INFO "search block:%s\n",block_name);

	  table = get_sram_pd_table(block_name);

         if(table == NULL)
         {
               printk(KERN_INFO "warning can not find ++%s++\n",block_name);
		 goto end;
         }



         for(i=0;i<table->sram_pd_ptr_num;i++)
         {

		val = get_reg_value( table->sram_pd_ptr[i].bank, table->sram_pd_ptr[i].reg,table->sram_pd_ptr[i].offset);

		printk(KERN_INFO "ip=%s ,bank=0x%x,\treg=0x%x,\toffset=%d,\tvalue=%d\n",table->sram_pd_ptr[i].ip_name,table->sram_pd_ptr[i].bank,table->sram_pd_ptr[i].reg,table->sram_pd_ptr[i].offset,val );
         }



end:
         return;
}


u16  get_clk_gate_status(clock_table *table)
{


      if(table->has_clk_gate == NO_GATE_REG)
      	{
      	   printk(KERN_INFO "get_clk_gate_status:no gate reg\n");
	   return NO_GATE_REG;
      	}
      U16 value = readregbyte2(table->clk_gate_bank,table->clk_gate_reg);

      U16 bit = (value >> table->clk_gate_offset) & 0x01;

      //printk(KERN_INFO "value=%d, offset =%d bit=%d\n",value,table->clk_gate_offset,bit);

      if(table->clk_gate_rev == CLK_GATE_NORMAL)
      {
             if(bit == 1)
                return CLK_GATE_DISABLE;
             else
   	         return CLK_GATE_ENABLE;
      	}
	else
       {
             if(bit == 0)
                return CLK_GATE_DISABLE;
             else
   	         return CLK_GATE_ENABLE;

       }


}


void _set_clk_gate(clock_table *table,U16 set)
{
      u16 gate_val=0;

      if(table->clk_gate_rev == CLK_GATE_NORMAL)
      {
              if(set == CLK_GATE_ENABLE)
		  gate_val = 0;
	       else
		  gate_val = 1;
      }
      else if(table->clk_gate_rev == CLK_GATE_REV)
      {
               if(set == CLK_GATE_ENABLE)
		  gate_val = 1;
	       else
		  gate_val = 0;
      }
      else
	   goto end;
      //printk(KERN_INFO "_Set_Clk_Gate reg=0x%x, val=%d\n",table->clk_gate_reg,gate_val);
      writeregbyte2range(table->clk_gate_bank,table->clk_gate_reg,table->clk_gate_offset,table->clk_gate_offset,gate_val);


end:
	return;
}


void clkm_clear_log()
{

     struct clkm_log *node;
     struct list_head *pos , *q;

     list_for_each_safe(pos, q, &clkm_log_root_list)
     {
		node = list_entry(pos, struct clkm_log, list);

              list_del(pos);
		kfree(node);
     	}


}


s32 clk_gate_enable(s32 handle)
{
       clock_table *table;
       S32 ret = 0;
       table = mdrv_clkm_get_clock_table(handle);

       if(table == NULL)
       {

	    printk("warning can not get table\n");
           ret = ERR_NOT_FOUND_TABLE;
	    goto end;
       }

	if(table->has_clk_gate == NO_GATE_REG)
	     goto end;

       mutex_lock(&clkm_mutex);

        _set_clk_gate(table, CLK_GATE_ENABLE);

       mutex_unlock(&clkm_mutex);

end:




	return ret;
}

EXPORT_SYMBOL(clk_gate_enable);



s32 clk_gate_disable(s32 handle)
{
       clock_table *table;
       S32 ret = 0;
       table = mdrv_clkm_get_clock_table(handle);

       if(table == NULL)
       {

	    printk("warning can not get table\n");
           ret = ERR_NOT_FOUND_TABLE;
	    goto end;
       }

	if(table->has_clk_gate == NO_GATE_REG)
		goto end;

	//printk(KERN_INFO "clkm driver clk gate disable\n");

       mutex_lock(&clkm_mutex);

        _set_clk_gate(table, CLK_GATE_DISABLE);

       mutex_unlock(&clkm_mutex);

end:




	return ret;
}

EXPORT_SYMBOL(clk_gate_disable);

s32 mdrv_clk_gate_disable(clock_table_param *clkm_param,unsigned long arg)
{
      S32 ret = 0;
      CLKM_CLK_GATE_DISABLE_PARAM clkgatedisableparam;
      S32 handle;



      if(copy_from_user(&clkgatedisableparam, (CLKM_CLK_GATE_DISABLE_PARAM __user *)arg, sizeof(CLKM_CLK_GATE_DISABLE_PARAM)))
	{
	     ret = EFAULT;
            goto end;
	}

	handle = clkgatedisableparam.s32_Handle;


      ret = clk_gate_disable(handle);


end:



	return ret;

}


char* get_clk_source(s32 handle)
{
     char *src_name = NULL;
     u16 val;
     clock_table *table;
     u16 mask ;
     u16 u16Low,u16High;
     char **ptr = NULL;

     table = mdrv_clkm_get_clock_table(handle);

     if(table == NULL || table->has_clk_sel == NO_SEL_REG)
	    goto end;



     val = readregbyte2(table->clk_sel_bank,table->clk_sel_reg);

     //printk(KERN_INFO "readregbyte2 val=%d\n",val);

     u16Low = table->clksel_offset_start;
     u16High = table->clksel_offset_end;

     if(u16Low == u16High)
     {
	     mask = 0x1<<u16Low;
      }
      else
      {
	    mask = ( 1 << ( u16High + 1 ) ) - ( 1 << u16Low ) ;
      }

      val = (val & mask) >> u16Low;
     // printk("high=%d, low=%d",u16High,u16Low);
     // printk("mask=%d, val=%d\n",mask,val);

	if(val  > table->src_num-1)
		goto end;

       ptr = table->g_src_name_ptr;

	src_name = ptr[val];

end:

    return src_name;

}

s32 set_clk_source(s32 handle , char *clk_src_name)
{
      S32 ret = 0;
      S32 src_offset;
      clock_table *table;
      struct clkm_log *node;


      table = mdrv_clkm_get_clock_table(handle);

      if(table == NULL)
      {

	   printk(KERN_ERR "warning can not get table\n");
          ret = ERR_NOT_FOUND_TABLE;
	   goto end2;
      }


      clk_gate_enable(handle);


      if( table->has_clk_sel== NO_SEL_REG)
      {
	     goto end2;
      }


       mutex_lock(&clkm_mutex);


      src_offset = get_src_name_offset(table,clk_src_name);

     // printk(KERN_INFO "_Set_Clk_Source src_offset=%d\n",src_offset);

      if(src_offset == NOT_FOUND)
      {
          printk("warning can not find %s\n",clk_src_name);
          ret = ERR_NOT_SUPPORT_SRC;
	   goto end1;
      }




      // printk("_MDrv_Set_Clk_Src: bank=0x%x, Reg=0x%x , offset=0x%x, val=0x%x\n", table->bank,table->reg,table->clksel_offset_start,src_offset);

       writeregbyte2range(table->clk_sel_bank , table->clk_sel_reg,table->clksel_offset_start,table->clksel_offset_end,src_offset);



       if(enable_log == 1)
       {

          node  = kzalloc(sizeof(*node), GFP_KERNEL);

          if(node == NULL)
          {
		printk(KERN_ERR "%s: %d malloc failed!!!\n", __FUNCTION__, __LINE__);
	  	goto end1;
          }


           node->handle = handle;
           node->src_index = src_offset;
           list_add(&node->list, &clkm_log_root_list);

       }

end1:

	 mutex_unlock(&clkm_mutex);

end2:

	return ret;

}


EXPORT_SYMBOL(set_clk_source);

s32 mdrv_set_clk_source(clock_table_param *clkm_param,unsigned long arg)
{
     S32 ret = 0;
     CLKM_SET_CLK_SRC_PARAM setclksrcparam;
     S32 handle;
     char *src_name;
     S32 i;



     if(copy_from_user(&setclksrcparam, (CLKM_SET_CLK_SRC_PARAM __user *)arg, sizeof(CLKM_SET_CLK_SRC_PARAM)))
	{
	     ret = EFAULT;
            goto end;
	}

      handle = setclksrcparam.s32_Handle;
      src_name = setclksrcparam.clk_src_name;

      ret = set_clk_source(handle,src_name);

end:

	return ret;
}




static s32 mdrv_clkm_open(struct inode *inode, struct file *file)
{

        //printk("mstar clkm driver open\n");
        return 0;
}


static s32 mdrv_clkm_release(struct inode *inode, struct file *file)
{

      //printk("mstar clkm driver close\n");
      return 0;
}


static long mdrv_clkm_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{

     S32 retval = 0;



     //printk("MDrv_Clkm_Ioctl:%d , arg=%lu \n",cmd,arg);


      switch (cmd)
      {

           case CMD_CLKM_GET_HANDLE:

	   //printk("CMD_CLKM_GET_HANDLE\n");

           retval = mdrv_get_handle(&clkm_param, arg);

	    break;


	    case CMD_CLKM_SET_CLK_SOURCE:

            //printk( "CMD_CLKM_SET_CLK\n");


	    retval = mdrv_set_clk_source(&clkm_param,arg);


	    break;

	     case CMD_CLKM_CLK_GATE_DISABLE:

            //printk( "CMD_CLKM_CLK_GATE_DISABLE\n");



	    retval = mdrv_clk_gate_disable(&clkm_param,arg);

	    break;



	    default:

	    break;
      }





     return retval;
}

/*

static long MDrv_Compat_Clkm_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{

      mutex_lock(&clkm_mutex);


	printk("cmd:%d , arg=%lu \n",cmd,arg);

      switch (cmd)
      {

           case CMD_GET_HANDLE:

	    break;

	    case CMD_CLKM_SET_CLK_SOURCE:

	    break;

	    default:

           break;

      }

       mutex_unlock(&clkm_mutex);

       return 0;
}
*/
#if 0

static int clkm_debugfs_open(struct inode *inode, struct file *filp)
{
    filp->private_data = inode->i_private;
    return 0;
}


static ssize_t clkm_debugfs_read(struct file *filp, char __user *buffer,
        size_t count, loff_t *ppos)
{

    char buf[32];
    len = snprintf(buf, 32, "%lu\n", *p);

     return 0;
}


static ssize_t clkm_debugfs_write(struct file *filp, const char __user *buffer,
        size_t count, loff_t *ppos)
{
      return 0;
}


struct file_operations clkm_debugfs_fops = {
    .owner = THIS_MODULE,
    .open = clkm_debugfs_open,
    .read = clkm_debugfs_read,
    .write = clkm_debugfs_write,
};

#endif

s32 clkm_suspend(void)
{

       return 0;
}


s32 clkm_resume(void)
{


     return 0;

}


static s32 clkm_init(void)
{

    S32 ret;

    #if 0
        S32 s32Ret;
        dev_t dev;





       clkm_class = class_create(THIS_MODULE, "clkm");
        if (IS_ERR(clkm_class))
        {
            return PTR_ERR(clkm_class);
        }



    if (clkm_dev.clkm_major)
        {
            dev = MKDEV(clkm_dev.clkm_major, clkm_dev.clkm_minor);
            s32Ret = register_chrdev_region(dev, 1, MOD_CLKM_NAME);
        }
        else
      {

          s32Ret = alloc_chrdev_region(&dev, clkm_dev.clkm_minor,1, MOD_CLKM_NAME);
           clkm_dev.clkm_major = MAJOR(dev);
       }

	if ( 0 > s32Ret)
       {
            class_destroy(clkm_class);
            return s32Ret;
       }

       cdev_init(&clkm_dev.clkm_device, &clkm_dev.clkm_file_operations);


       if (0!= (s32Ret=cdev_add(&clkm_dev.clkm_device, dev, 1)))
       {
            unregister_chrdev_region(dev, 1);
            class_destroy(clkm_class);
            return s32Ret;
       }


       device_create(clkm_class, NULL, dev, NULL, MOD_CLKM_NAME);

      #else

      ret = misc_register(&clkm_miscdev);

      #endif

       #if 0

   clkm_debugfs_dir_entry_root = debugfs_create_dir("clkm", NULL);
       debugfs_create_file("debugfs", 0644, clkm_debugfs_dir_entry_root, NULL, &clkm_debugfs_fops);
       #endif

       clkm_log_kobj = kobject_create_and_add("clkm_log", kernel_kobj);

	 ret = sysfs_create_group(clkm_log_kobj, &attr_group);

        if (ret)
           kobject_put(clkm_log_kobj);


       proc_create("clkm_log", 0, NULL, &clkm_log_operations);

	clkm_proc_file = proc_create (clkm_dump_name, 0777,NULL, &proc_clkm_dump_operations);

	if(clkm_proc_file == NULL)
		printk("CLKM Err: Can not initialize /proc/%s\n", clkm_dump_name);

	sram_pd_proc_file = proc_create (sram_pd_dump_name, 0777,NULL, &proc_sram_pd_dump_operations);
	if(clkm_proc_file == NULL)
		printk("CLKM Err: Can not initialize /proc/%s\n", sram_pd_dump_name);


	mutex_init(&clkm_mutex);


    return 0;



}





static void clkm_exit(void)
{

     clkm_clear_log();

     cdev_del(&clkm_dev.clkm_device);
     unregister_chrdev_region(MKDEV(clkm_dev.clkm_major, clkm_dev.clkm_minor), 1);

     device_destroy(clkm_class, MKDEV(clkm_dev.clkm_major, clkm_dev.clkm_minor));
     class_destroy(clkm_class);


}


static s32 mstar_clkm_drv_probe(struct platform_device *pdev)
{
      return 0;
}

static s32 mstar_clkm_drv_remove(struct platform_device *pdev)
{
     pdev->dev.platform_data=NULL;
     return 0;
}


static s32 mstar_clkm_drv_suspend(struct platform_device *dev, pm_message_t state)
{
      return clkm_suspend();
}


static s32 mstar_clkm_drv_resume(struct platform_device *dev)
{
      return clkm_resume();
}

static struct platform_driver Mstar_clkm_driver = {
	.probe 		= mstar_clkm_drv_probe,
	.remove 	= mstar_clkm_drv_remove,
       .suspend    = mstar_clkm_drv_suspend,
       .resume     = mstar_clkm_drv_resume,

	.driver = {
		.name	= MOD_CLKM_NAME,
              .owner  = THIS_MODULE,
	}
};




static s32 __init mstar_clkm_drv_init_module(void)
{
      return clkm_init()
;
}

static void __exit mstar_clkm_drv_exit_module(void)
{
      clkm_exit();
}


module_init(mstar_clkm_drv_init_module);
module_exit(mstar_clkm_drv_exit_module);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Mstar clock manager!!");
MODULE_AUTHOR("MSTAR");

