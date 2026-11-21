#!/usr/bin/env bash

# Создание папок
cd ${PROJECT_DIR} && \
echo "Версия сборки gta5view: ${APPLICATION_VERSION}" && \
echo "Имя изображения gta5view — gta5view-osx_${APPLICATION_VERSION}.dmg" && \
mkdir -p build && \
mkdir -p assets && \
cd build && \

/usr/local/bin/cmake \
   "-DCMAKE_PREFIX_PATH=/usr/local/opt/qt" \
   "${CMAKE_BUILD_TYPE}" \
   "-DGTA5VIEW_BUILDCODE=${PACKAGE_CODE}" \
   "-DGTA5VIEW_APPVER=${APPLICATION_VERSION}" \
   "-DGTA5VIEW_COMMIT=${APPLICATION_COMMIT}" \
   "-DWITH_DONATE=ВКЛ." \
   "-DWITH_TELEMETRY=ВКЛ." \
   "-DDONATE_ADDRESSES=$(cat ${PROJECT_DIR}/.ci/donate.txt)" \
   "-DTELEMETRY_WEBURL=https://dev.syping.de/gta5view-userstats/" \
   ../ && \
make -j 4 && \
/usr/local/opt/qt/bin/macdeployqt gta5view.app -dmg && \
cp -Rf gta5view.dmg ../assets/gta5view-osx_${APPLICATION_VERSION}.dmg
