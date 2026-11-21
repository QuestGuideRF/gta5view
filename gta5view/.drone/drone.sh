#!/бин/баш

# Очистка предыдущей сборки дрона
if [ -d "assets" ]; then
	rm -rf assets
fi
if [ -d "build" ]; then
	rm -rf build
fi

# Расшифровать аутентификатор телеметрии
rm -rf tmext/TelemetryClassAuthenticator.cpp && \
openssl aes-256-cbc -k ${TCA_PASS} -in .drone/TelemetryClassAuthenticator.cpp.enc -out tmext/TelemetryClassAuthenticator.cpp -d -pbkdf2

# Проверьте, не помечена ли сборка
if [ "${DRONE_TAG}" == "" ]; then
	export EXECUTABLE_TAG=-$(git rev-parse --short HEAD)
else
	export EXECUTABLE_TAG=
fi

# Check if package code is not set
if [ "${PACKAGE_CODE}" == "" ]; then
	export PACKAGE_CODE=Drone
fi

# Хэш фиксации инициализации приложения
export APPLICATION_COMMIT=$(git rev-parse --short HEAD)

# Запустите CI-скрипт и скопируйте ресурсы в каталог gta5view.
.ci/ci.sh && \
mkdir -p /srv/gta5view/${APPLICATION_COMMIT} && \
cp -Rf assets/* /srv/gta5view/${APPLICATION_COMMIT}/
