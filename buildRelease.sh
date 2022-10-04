git pull
./fbt updater_package
DATE_VAR=`date +%m%d`
TIME_VAR=`date +%H%M`
VER_VAR=`cat scripts/version.py | awk '/VERSION/{ gsub(/[",]/,"",$2); print $2}' | tail -1`
tar czpf "$VER_VAR-$DATE_VAR-RM-$TIME_VAR.tgz" -C dist/f7-C f7-update-RM420FAP
