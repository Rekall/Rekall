#!/bin/bash
cd `dirname $0`
rm -R -f create
rm -f create.zip
mkdir create
cp -R css create/
cp -R js create/
cp -R php create/
cp -R file_template create/file
cp -R favicon.ico create/
cp -R index.html create/
cp -R .htaccess create/
cd create/
zip -r -y ../create.zip *
cd ..
rm -R -f create
