@echo off
cd build & cmake ../CMakeLists.txt -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -B . & make -j12
cd ..