#!/usr/bin/env bash

# Исходная версия ОС
source /etc/os-release

# Когда Debian добавляет бэкпорты
if [ "${ID}" == "debian" ]; then
    echo "deb http://deb.debian.org/debian ${VERSION_CODENAME}-backports main" >> /etc/apt/sources.list
fi

# Установить пакеты
apt-get update -qq && \
apt-get install -qq ${APT_INSTALL} checkinstall cmake dpkg-dev fakeroot g++ gcc qtbase5-dev qt5-qmake qttranslations5-l10n libqt5svg5-dev
