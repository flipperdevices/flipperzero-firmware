rm -rf RM*-*-*.tgz RM*-*-*.zip .sconsign.dblite dist build assets/resources/apps assets/resources/nfc/RMProTrainedAmiibo
git pull
./fbt updater_package
DATE_VAR=`date +%m%d`
TIME_VAR=`date +%H%M`
VER_VAR=`cat scripts/version.py | grep "or \"0" | awk -F\" '{print $2}'`
HASH_VAR=`git rev-parse \`git branch -r --sort=committerdate | tail -1\` | awk '{print substr($0,1,7)}' | tail -1`
mv dist/f7-C/f7-update-RM420FAP "RM$DATE_VAR-$TIME_VAR"
zip -rq "RM$DATE_VAR-$TIME_VAR-$VER_VAR-$HASH_VAR.zip" "RM$DATE_VAR-$TIME_VAR"
tar -czf "RM$DATE_VAR-$TIME_VAR-$VER_VAR-$HASH_VAR.tgz" "RM$DATE_VAR-$TIME_VAR"
rm -rf assets/dolphin/internal/*
cp -rf ../dolphinIntBlank/* assets/dolphin/internal/
cp -rf ../animation_managerBLANK.c applications/services/desktop/animations/animation_manager.c
./fbt updater_package
mv dist/f7-C/f7-update-RM420FAP "RM$DATE_VAR-$TIME_VAR-NoAnim"
rm -rf assets/resources/dolphin/*
cd assets/resources
tar -cf "../../RM$DATE_VAR-$TIME_VAR-NoAnim/resources.tar" *
cd ../../
zip -rq "RM$DATE_VAR-$TIME_VAR-$VER_VAR-$HASH_VAR-NoAnim.zip" "RM$DATE_VAR-$TIME_VAR-NoAnim"
tar -czf "RM$DATE_VAR-$TIME_VAR-$VER_VAR-$HASH_VAR-NoAnim.tgz" "RM$DATE_VAR-$TIME_VAR-NoAnim"
rm -rf "RM$DATE_VAR-$TIME_VAR"
rm -rf "RM$DATE_VAR-$TIME_VAR-NoAnim"
rm -rf assets/dolphin/internal/*
git stash
echo "BUILD COMPLETED, ZIP AND TGZ GENERATED FOR RM$DATE_VAR-$TIME_VAR-$VER_VAR-$HASH_VAR"
