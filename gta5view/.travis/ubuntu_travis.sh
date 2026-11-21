#!/бин/баш

# Установить пакеты
sudo .ci/debian_install.sh && \

# Сборка gta5view
sudo .ci/debian_build.sh && \
cd ${PROJECT_DIR}
