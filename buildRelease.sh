git pull
./fbt updater_package
DATE_VAR=`date +%m%d`
TIME_VAR=`date +%H%M`
VER_VAR=`cat scripts/version.py | awk '/VERSION/{ gsub(/[",]/,"",$2); print $2}' | tail -1`
mv dist/f7-C/f7-update-RM420FAP "$VER_VAR-$DATE_VAR-RM$TIME_VAR"
zip -r "$VER_VAR-$DATE_VAR-RM$TIME_VAR.zip" "$VER_VAR-$DATE_VAR-RM$TIME_VAR"
tar -czvf "$VER_VAR-$DATE_VAR-RM$TIME_VAR.tgz" "$VER_VAR-$DATE_VAR-RM$TIME_VAR"
rm -rf "$VER_VAR-$DATE_VAR-RM$TIME_VAR"
echo " BUILD COMPLETED, ZIP AND TGZ GENERATED"
