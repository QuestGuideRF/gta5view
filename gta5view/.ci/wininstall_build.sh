#!/usr/bin/env bash

# Создание папок
cd ${PROJECT_DIR} && \
echo "Версия сборки gta5view: ${APPLICATION_VERSION}" && \
mkdir -p build && \
mkdir -p assets && \

# Начинаем сборку
cd build && \
mingw64-qt-cmake \
   "${CMAKE_BUILD_TYPE}" \
   "-DGTA5VIEW_BUILDCODE=${PACKAGE_CODE}" \
   "-DGTA5VIEW_APPVER=${APPLICATION_VERSION}" \
   "-DGTA5VIEW_COMMIT=${APPLICATION_COMMIT}" \
   "-DWITH_DONATE=ВКЛ." \
   "-DWITH_TELEMETRY=ВКЛ." \
   "-DDONATE_ADDRESSES=$(cat ${PROJECT_DIR}/.ci/donate.txt)" \
   "-DTELEMETRY_WEBURL=https://dev.syping.de/gta5view-userstats/" \
   "-DQCONF_BUILD=ВКЛ." \
   "-DGTA5VIEW_INLANG=RUNDIR:SEPARATOR:нет" \
   "-DGTA5VIEW_LANG=RUNDIR:SEPARATOR:lang" \
   "-DGTA5VIEW_PLUG=RUNDIR:SEPARATOR:плагины" \
   .. && \
make -j 4 && \
x86_64-w64-mingw32-strip -s gta5view.exe && \
cd ${PROJECT_DIR}/assets && \
makensis "-XTarget amd64-Юникод" -NOCD ${PROJECT_DIR}/.ci/gta5view.nsi && \
mv -f gta5view_setup.exe gta5view-${EXECUTABLE_VERSION}_setup.exe
