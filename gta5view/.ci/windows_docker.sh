#!/usr/bin/env bash

DOCKER_IMAGE=sypingauto/gta5view-build:1.10-static
PROJECT_DIR_DOCKER=/gta5view

cd ${PROJECT_DIR} && \
docker pull ${DOCKER_IMAGE} && \
docker run --rm \
   -v "${PROJECT_DIR}:${PROJECT_DIR_DOCKER}" \
   ${DOCKER_IMAGE} \
   /bin/bash -c "экспорт PROJECT_DIR=${PROJECT_DIR_DOCKER} && экспорт QT_SELECT=${QT_SELECT} && экспорт APPLICATION_VERSION=${APPLICATION_VERSION} && экспорт APPLICATION_COMMIT=${APPLICATION_COMMIT} && экспорт BUILD_TYPE=${BUILD_TYPE} && экспорт QMAKE_FLAGS_QT4=${QMAKE_FLAGS_QT4} && экспорт QMAKE_FLAGS_QT5=${QMAKE_FLAGS_QT5} && экспорт CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} && экспорт QMAKE_BUILD_TYPE=${QMAKE_BUILD_TYPE} && экспорт PACKAGE_VERSION=${PACKAGE_VERSION} && экспорт PACKAGE_BUILD=${PACKAGE_BUILD} && экспорт PACKAGE_CODE=${PACKAGE_CODE} && экспорт EXECUTABLE_VERSION=${EXECUTABLE_VERSION} && экспорт EXECUTABLE_ARCH=${EXECUTABLE_ARCH} && cd ${PROJECT_DIR_DOCKER} && .ci/windows_build.sh" && \

# Подготовьте переменную среды
export GTA5VIEW_EXECUTABLE=gta5view-${EXECUTABLE_VERSION}${EXECUTABLE_ARCH}.exe && \

# Загрузить ресурсы в Dropbox
if [ "${PACKAGE_CODE}" == "gta5-mods" ]; then
   ${PROJECT_DIR}/.ci/dropbox_uploader.sh mkdir gta5-mods/${PACKAGE_VERSION}
   ${PROJECT_DIR}/.ci/dropbox_uploader.sh upload ${PROJECT_DIR}/assets/${GTA5VIEW_EXECUTABLE} gta5-mods/${PACKAGE_VERSION}/${GTA5VIEW_EXECUTABLE} && \
   rm -rf ${GTA5VIEW_EXECUTABLE}
elif [ "${PACKAGE_CODE}" == "gtainside" ]; then
   ${PROJECT_DIR}/.ci/dropbox_uploader.sh mkdir gtainside/${PACKAGE_VERSION}
   ${PROJECT_DIR}/.ci/dropbox_uploader.sh upload ${PROJECT_DIR}/assets/${GTA5VIEW_EXECUTABLE} gtainside/${PACKAGE_VERSION}/${GTA5VIEW_EXECUTABLE} && \
   rm -rf ${GTA5VIEW_EXECUTABLE}
fi
