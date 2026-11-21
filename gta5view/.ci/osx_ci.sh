#!/usr/bin/env bash

# Установить пакеты
.ci/osx_install.sh && \

# Сборка gta5view
.ci/osx_build.sh && \
cd ${PROJECT_DIR}
