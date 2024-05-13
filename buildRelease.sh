rm -rf RM*-*-*.tgz RM*-*-*.zip .sconsign.dblite dist build build/f7-firmware-C/resources/apps build/f7-firmware-C/resources/nfc/RMProTrainedAmiibo
git pull
DATE_VAR=`date +%m%d`
TIME_VAR=`date +%H%M`
VER_VAR=`cat scripts/version.py | grep "or \"0" | awk -F\" '{print $4}'`
HASH_VAR=`git rev-parse \`git branch -r --sort=committerdate | tail -1\` | awk '{print substr($0,1,7)}' | tail -1`
# ./fbt updater_package
# mv dist/f7-C/f7-update-RM420FAP "RM$DATE_VAR-$TIME_VAR"
# zip -rq "RM$DATE_VAR-$TIME_VAR-$VER_VAR-$HASH_VAR.zip" "RM$DATE_VAR-$TIME_VAR"
# tar -czf "RM$DATE_VAR-$TIME_VAR-$VER_VAR-$HASH_VAR.tgz" "RM$DATE_VAR-$TIME_VAR"
rm -rf assets/dolphin/internal/*
cp -rf .blank_files/dolphinIntBlank/* assets/dolphin/internal/
cp -rf .blank_files/animation_managerBLANK.c applications/services/desktop/animations/animation_manager.c
./fbt updater_package
mv dist/f7-C/f7-update-RM420FAP "RM$DATE_VAR-$TIME_VAR"
# rm -rf build/f7-firmware-C/resources/dolphin/*
cp -rf .blank_files/MjK_blank_128x64 build/f7-firmware-C/resources/dolphin/
cp -rf .blank_files/manifest_Minimal.txt build/f7-firmware-C/resources/dolphin/manifest.txt
cp -rf .blank_files/manifest_Minimal.txt build/f7-firmware-C/resources/dolphin/manifest_Minimal.txt
cp -rf .blank_files/manifest_None.txt build/f7-firmware-C/resources/dolphin/manifest_None.txt
cd build/f7-firmware-C/resources
tar -czf "../../../RM$DATE_VAR-$TIME_VAR/resources.tar.gz" *
cd ../../../
zip -rq "RM$DATE_VAR-$TIME_VAR-$VER_VAR-$HASH_VAR.zip" "RM$DATE_VAR-$TIME_VAR"
tar -czf "RM$DATE_VAR-$TIME_VAR-$VER_VAR-$HASH_VAR.tgz" "RM$DATE_VAR-$TIME_VAR"
rm -rf "RM$DATE_VAR-$TIME_VAR"
rm -rf assets/dolphin/internal/*
git stash
echo "BUILD COMPLETED, ZIP AND TGZ GENERATED FOR RM$DATE_VAR-$TIME_VAR-$VER_VAR-$HASH_VAR"
