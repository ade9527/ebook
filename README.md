# E-book(电子书)
实现一个不依赖图形框架的(自己处理显示)电子书.

##功能模块
1.文件编码识别,解析读取
  encoding_manager (utf-8, utf-16, ascii)
2.编码转化为点阵
  font_manager (ascii, hzk16, freetyp)
3.点阵显示
  display_manager (framebuffer, bga ...)
