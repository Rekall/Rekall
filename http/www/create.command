#!/bin/bash
cd `dirname $0`
rm -R -f createTmp
rm -f create/create.zip
mkdir createTmp
cp -R css createTmp/
cp -R js createTmp/
cp -R php createTmp/
cp -R file_template createTmp/file
cp -R favicon.ico createTmp/
cp -R index.html createTmp/
cp -R .htaccess createTmp/
cd createTmp/
zip -r -y ../create/create.zip *
cd ..
rm -R -f createTmp
