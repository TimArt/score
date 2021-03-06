#!/bin/sh
source "$CONFIG_FOLDER/linux-source-qt.sh"

mkdir build && cd build
$CMAKE_BIN -DCMAKE_BUILD_TYPE=Debug -DCMAKE_UNITY_BUILD=1 -DSCORE_COVERAGE=1 -DSORE_PCH=0 -DCMAKE_UNITY_BUILD=1 $CMAKE_COMMON_FLAGS ..
$CMAKE_BIN --build . -- -j2

echo "$PWD"
gem install coveralls-lcov
export DISPLAY=:99.0
sh -e /etc/init.d/xvfb start
sleep 3
cp -rf ../Tests/testdata .
find . -name "*.o" -print0 | xargs -0 rm -rf
LD_LIBRARY_PATH=/usr/lib64 ./score_testapp
# LD_LIBRARY_PATH=/usr/lib64 $CMAKE_BIN --build . --target score_test_coverage_unity
lcov --compat-libtool --directory .. --capture --output-file coverage.info
lcov --remove coverage.info '*.moc' '*/moc_*' '*/qrc_*' '*/ui_*' '*/tests/*' '/usr/*' '/opt/*' '*/3rdparty/*' --output-file coverage.info.cleaned
mv coverage.info.cleaned coverage.info
coveralls-lcov coverage.info
