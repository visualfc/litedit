@echo on

copy LICENSE.LGPL liteide
copy LGPL_EXCEPTION.TXT liteide
copy ..\README.md liteide
xcopy deploy liteide\share\liteide  /e /y /i
