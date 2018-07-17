#!/bin/sh

cp -v ../README.md liteide/bin
cp -v LICENSE.LGPL liteide/bin
cp -v LGPL_EXCEPTION.TXT liteide/bin

cp -r -v deploy/* liteide/bin/LiteIDE.app/Contents/Resources

cp -v src/liteapp/Info.plist liteide/bin/LiteIDE.app/Contents
