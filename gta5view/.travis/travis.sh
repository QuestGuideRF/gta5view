#!/бин/баш

# Установить Луа
if [ "${TRAVIS_OS_NAME}" == "osx" ]; then
	brew install lua
else
	sudo apt-get update -qq && \
	sudo apt-get install -qq lua5.2
fi

# Проверьте, не помечена ли сборка
if [ "${TRAVIS_TAG}" == "" ]; then
	export EXECUTABLE_TAG=-$(git rev-parse --short HEAD)
else
	export EXECUTABLE_TAG=
fi

# Check if package code is not set
if [ "${PACKAGE_CODE}" == "" ]; then
	export PACKAGE_CODE=GitHub
fi

# Хэш фиксации инициализации приложения
export APPLICATION_COMMIT=$(git rev-parse --short HEAD)

# Запустить CI-скрипт
.ci/ci.sh
