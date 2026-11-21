#!/usr/bin/env bash

# Установить пакеты
.ci/debian_install.sh && \

# Сборка gta5view
.ci/debian_build.sh && \
cd ${PROJECT_DIR}
