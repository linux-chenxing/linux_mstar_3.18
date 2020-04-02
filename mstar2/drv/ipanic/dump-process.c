///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   dump-process.c
/// @brief  dump process info
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include "ipanic.h"

extern union IPANIC_MEM  ipanic_memory;
static void _LOG(const char *fmt, ...)
{
    char buf[256];
    int len=0;
    va_list ap;

    va_start(ap, fmt);
    len = strlen(ipanic_memory.NativeInfo);
    if ((len+sizeof(buf)) < MAX_NATIVEINFO)
        vsnprintf(&ipanic_memory.NativeInfo[len], sizeof(buf), fmt, ap);
    va_end(ap);      
}

static void dump_registers(struct pt_regs *r)
{
#ifdef CONFIG_64BIT
    int i, top_reg;
    u64 lr, sp;
#endif    

    if(r == NULL)
        return;
    
#ifdef CONFIG_64BIT
    if (compat_user_mode(r)) {
        lr = r->compat_lr;
        sp = r->compat_sp;
        top_reg = 12;
    } else {
        lr = r->regs[30];
        sp = r->sp;
        top_reg = 29;
    }

    _LOG(" pc/lr/sp 0x%016llx/0x%016llx/0x%016llx\n", instruction_pointer(r), lr, sp); 
    for (i = top_reg; i >= 0; i--) 
    {
        _LOG("x%-2d: %016llx    ", i, r->regs[i]);
    }
    _LOG("\n"); 
#else
    _LOG(" pc/lr/sp 0x%08x/0x%08x/0x%08x\n", r->ARM_pc, r->ARM_lr, r->ARM_sp); 
    _LOG(" r0 %08x r1 %08x r2 %08x r3 %08x\n",
     r->ARM_r0, r->ARM_r1, r->ARM_r2, r->ARM_r3);
    _LOG(" r4 %08x r5 %08x r6 %08x r7 %08x\n",
     r->ARM_r4, r->ARM_r5, r->ARM_r6, r->ARM_r7);
    _LOG(" r8 %08x r9 %08x 10 %08x fp %08x\n",
     r->ARM_r8, r->ARM_r9, r->ARM_r10, r->ARM_fp);
    _LOG(" ip %08x sp %08x lr %08x pc %08x cpsr %08x\n",
     r->ARM_ip, r->ARM_sp, r->ARM_lr, r->ARM_pc, r->ARM_cpsr);       
#endif    
}

int DumpNativeInfo(void)
{
    struct task_struct *current_task;
    struct pt_regs *user_ret;
    struct vm_area_struct *vma;
    unsigned long userstack_start = 0;
    unsigned long userstack_end = 0;
    int mapcount = 0;
    struct file *file;
    int flags;
    struct mm_struct *mm;
    int i=0;

    memset(ipanic_memory.NativeInfo, 0, sizeof(ipanic_memory.NativeInfo));
    //memset(User_Stack, 0, sizeof(User_Stack));
    
    current_task = get_current();
    //current_task = find_task_by_vpid(982);
    user_ret = task_pt_regs(current_task);

    if (!user_mode(user_ret))
        return 0;

    dump_registers(user_ret);
#ifdef CONFIG_64BIT
    if (compat_user_mode(user_ret))  
        userstack_start = user_ret->compat_sp;         
    else
        userstack_start = user_ret->sp;
#else
    userstack_start = (unsigned long)user_ret->ARM_sp;
#endif    

    if(current_task->mm == NULL)
       return 0;

    vma = current_task->mm->mmap;
    while (vma && (mapcount < current_task->mm->map_count)) {
        file = vma->vm_file;
        flags = vma->vm_flags;
        if(file)
        {
            _LOG("%08lx-%08lx %c%c%c%c %s\n",vma->vm_start,vma->vm_end,
             flags & VM_READ ? 'r' : '-',
             flags & VM_WRITE ? 'w' : '-',
             flags & VM_EXEC ? 'x' : '-',
             flags & VM_MAYSHARE ? 's' : 'p',file->f_path.dentry->d_iname);
         
             //IPANIC_DPRINTK("file %s\n", file->f_path.dentry->d_iname);    
        }
        else 
        {
            const char *name = arch_vma_name(vma);
            mm = vma->vm_mm;
            if(!name) 
            {
                if(mm)
                {
                    if(vma->vm_start <= mm->start_brk && vma->vm_end >= mm->brk) 
                    {
                        name = "[heap]";
                    } 
                    else if(vma->vm_start <= mm->start_stack && vma->vm_end >= mm->start_stack) 
                    {
                        name = "[stack]";
                    }
                }
                else 
                {
                    name = "[vdso]";
                }
            }
            
            //if (name)
            {                  
                _LOG("%08lx-%08lx %c%c%c%c %s\n",vma->vm_start,vma->vm_end,
                 flags & VM_READ ? 'r' : '-',
                 flags & VM_WRITE ? 'w' : '-',
                 flags & VM_EXEC ? 'x' : '-',
                 flags & VM_MAYSHARE ? 's' : 'p',name);            
            }
        }
        vma = vma->vm_next;
        mapcount++;
        
    }

    vma = current_task->mm->mmap;
    while (vma != NULL) 
    {
        if (vma->vm_start <= userstack_start
         && vma->vm_end >= userstack_start) 
        {
            userstack_end = vma->vm_end;            
            break;
        }
        vma = vma->vm_next;
        if (vma == current_task->mm->mmap) 
        {
            break;
        }
    }

    if (userstack_end == 0) {
        IPANIC_DPRINTK("Dump native stack failed:\n");
        return 0;
    }
    
    _LOG ("Dump stack range (0x%08x:0x%08x)\n", userstack_start, userstack_end);
    
    //length = ((userstack_end-userstack_start) < (sizeof(User_Stack) - 1)) ? (userstack_end-userstack_start) : (sizeof(User_Stack) - 1);
    //ret=copy_from_user((void *)(User_Stack), (const void __user *)( userstack_start), length);
    //IPANIC_DPRINTK("copy_from_user ret(0x%08x),len:%x\n",ret,length);
    i=0; 
    while (userstack_start < userstack_end) {
        _LOG ("0x%08x: 0x%08x\n", userstack_start, *((unsigned int *)userstack_start));
        //printk(KERN_ERR "0x%08x: 0x%08x\n", userstack_start, *((unsigned int *)userstack_start));
        userstack_start+=4;  // 4 bytes
        i++;
    }
    _LOG ("end dump native stack:\n");
    return 0;
}
