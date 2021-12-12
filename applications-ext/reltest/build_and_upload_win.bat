cd build & cmake ../CMakeLists.txt -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -B . & make -j12
cd ..
python3 ../../scripts/storage.py -p COM25 send build/plugin.elf /ext/plugin.elf -f