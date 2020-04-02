Default ipanic not enabled in kernel config

if you need enable ipanic, do as following
a. rebuild kernel and enable ipanic in <MStar Platform Configuration>
b. create one partition named "ipanic", size 1M bytes at least
   in scripts\set_partition (mmc create ipanic 0x100000)
c. when panic, reboot board, then enter /sys/kernel/debug/ipanic
   cat upload 
   or dump the info into a file, for example:
   cat upload > /data/app/log.txt
   
note:
   android 5.1 and after version, kernel can't dump android log
   if you want dump android log, you call try below commond in console:
   /system/bin/logcat -b all -n 4 -r 500 -v threadtime -f /data/app/log
   
   dump all android log in to /data/app log* file, (-n 4) denotes 4 files,
   (-r 500) denotes one file size 500K, (-b all) denotes all andorid log 
      
   
   
