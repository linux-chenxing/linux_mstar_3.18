to config and test local dimming, please refer to doc dir
1. mboot打开RT_PM，RT_PM打开local dimming
2. kernel打开local dimming
3. 将如下四个文件放入/config/ldm/
   ldm.ini/Compensation.bin/BacklightGamma.bin/Edge2D.bin
4. testapp 可以测试除STR外的功能
5. testapp 里面的code可作为上层AP call LD ioctl的sample code

testapp的使用可以参考文档：testapp_readme.docx

配置文件及说明文档请参考目录 doc/