#!/usr/bin/env bash

if [ $(git name-rev --tags --name-only $(git rev-parse HEAD)) == "undefined" ]; then
   export APPLICATION_VERSION=$(lua -e 'for line in io.lines("config.h") do local m = string.match(line, "#define GTA5SYNC_APPVER \"(.+)\"$"); if m then print(m); os.exit(0) end end')
else
   export APPLICATION_VERSION=$(git name-rev --tags --name-only $(git rev-parse HEAD))
fi
export PACKAGE_VERSION=$(grep -oE '^[^\-]*' <<< $APPLICATION_VERSION)
export PACKAGE_BUILD=$(grep -oP '\-\K.+' <<< $APPLICATION_VERSION)
export EXECUTABLE_VERSION=${PACKAGE_VERSION}${PACKAGE_BUILD}${EXECUTABLE_TAG}

export APPLICATION_MAJOR_VERSION=$(cut -d. -f1 <<< $APPLICATION_VERSION)
export APPLICATION_MINOR_VERSION=$(cut -d. -f2 <<< $APPLICATION_VERSION)
export APPLICATION_PATCH_VERSION=$(cut -d. -f3 <<< $APPLICATION_VERSION)

if [ "${PACKAGE_BUILD}" == "" ]; then
   export PACKAGE_BUILD=1
else
   export APPLICATION_BUILD_INT_VERSION=$(grep -oE '[1-9]*$' <<< $PACKAGE_BUILD)
   export APPLICATION_BUILD_STR_VERSION=-${PACKAGE_BUILD}
fi

cat ".ci/app.rc" | sed \
   -e "s/MAJOR_VER/$APPLICATION_MAJOR_VERSION/g" \
   -e "с/MINOR_VER/$APPLICATION_MINOR_VERSION/g" \
   -e "s/PATCH_VER/$APPLICATION_PATCH_VERSION/g" \
   -e "s/INT_BUILD_VER/0/g" \
   -e "s/STR_BUILD_VER/$APPLICATION_BUILD_STR_VERSION/g" \
   -e "s/STR_BUILD_VER/$APPLICATION_BUILD_STR_VERSION/g" \
   > "res/app.rc"

if [ "${BUILD_TYPE}" == "ALPHA" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Альфа"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Альфа\\\\\\\""
elif [ "${BUILD_TYPE}" == "Alpha" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Альфа"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Альфа\\\\\\\""
elif [ "${BUILD_TYPE}" == "BETA" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Бета"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Бета\\\\\\\""
elif [ "${BUILD_TYPE}" == "Beta" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Бета"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Бета\\\\\\\""
elif [ "${BUILD_TYPE}" == "DEV" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Разработчик"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Разработчик\\\\\\\""
elif [ "${BUILD_TYPE}" == "Development" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Разработчик"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Разработчик\\\\\\\""
elif [ "${BUILD_TYPE}" == "DAILY" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Ежедневная сборка"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Ежедневная сборка\\\\\\\""
elif [ "${BUILD_TYPE}" == "Daily" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Ежедневная сборка"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Ежедневная сборка\\\\\\\""
elif [ "${BUILD_TYPE}" == "RC" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Кандидат на выпуск"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Кандидат на выпуск\\\\\\\""
elif [ "${BUILD_TYPE}" == "Кандидат на выпуск" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Кандидат на выпуск"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Кандидат на выпуск\\\\\\\""
elif [ "${BUILD_TYPE}" == "REL" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Выпуск"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Выпуск\\\\\\\""
elif [ "${BUILD_TYPE}" == "Release" ]; then
   export CMAKE_BUILD_TYPE="-DGTA5VIEW_BUILDTYPE=Выпуск"
   export QMAKE_BUILD_TYPE="DEFINES+=GTA5SYNC_BUILDTYPE=\\\\\\\"Выпуск\\\\\\\""
fi

export PROJECT_DIR=$(pwd)

.ci/${BUILD_SCRIPT}
