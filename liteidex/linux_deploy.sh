#!/bin/sh

cp -v LICENSE.LGPL liteide
cp -v LGPL_EXCEPTION.TXT liteide
cp -v ../README.md liteide
mkdir -p liteide/share/liteide/
cp -r -v deploy/* liteide/share/liteide/

