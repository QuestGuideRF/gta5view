#!/usr/bin/env bash
#
# Загрузчик Dropbox
#
# Copyright (C) 2010–2017 Андреа Фабризи <andrea.fabrizi@gmail.com>
#
# Эта программа является бесплатным программным обеспечением; вы можете распространять его и/или изменять
# это в соответствии с условиями Стандартной общественной лицензии GNU, опубликованной
# Фонд свободного программного обеспечения; либо версия 2 Лицензии, либо
# (по вашему выбору) любая более поздняя версия.
#
# Данная программа распространяется в надежде, что она будет полезна,
# но БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ; даже без подразумеваемой гарантии
# ТОВАРНАЯ ПРИГОДНОСТЬ или ПРИГОДНОСТЬ ДЛЯ ОПРЕДЕЛЕННОЙ ЦЕЛИ.  См.
# Стандартная общественная лицензия GNU для более подробной информации.
#
# Вы должны были получить копию Стандартной общественной лицензии GNU.
# вместе с этой программой; если нет, напишите в Free Software
# Foundation, Inc., 59 Temple Place – Suite 330, Бостон, Массачусетс 02111-1307, США.
#

#Файл конфигурации по умолчанию
CONFIG_FILE=~/.dropbox_uploader

#Размер чанка по умолчанию в МБ для процесса загрузки
#Рекомендуется увеличивать это значение только в том случае, если у вас достаточно свободного места в разделе /tmp.
#Более низкие значения могут увеличить количество HTTP-запросов.
CHUNK_SIZE=50

#Расположение завитка
#If not set, curl will be searched into the $PATH
#CURL_BIN="/usr/bin/curl"

#Значения по умолчанию
TMP_DIR="/tmp"
DEBUG=0
QUIET=0
SHOW_PROGRESSBAR=0
SKIP_EXISTING_FILES=0
ERROR_STATUS=0
EXCLUDE=()

#Не редактируйте эти...
API_LONGPOLL_FOLDER="https://notify.dropboxapi.com/2/files/list_folder/longpoll"
API_CHUNKED_UPLOAD_START_URL="https://content.dropboxapi.com/2/files/upload_session/start"
API_CHUNKED_UPLOAD_FINISH_URL="https://content.dropboxapi.com/2/files/upload_session/finish"
API_CHUNKED_UPLOAD_APPEND_URL="https://content.dropboxapi.com/2/files/upload_session/append_v2"
API_UPLOAD_URL="https://content.dropboxapi.com/2/files/upload"
API_DOWNLOAD_URL="https://content.dropboxapi.com/2/files/download"
API_DELETE_URL="https://api.dropboxapi.com/2/files/delete"
API_MOVE_URL="https://api.dropboxapi.com/2/files/move"
API_COPY_URL="https://api.dropboxapi.com/2/files/copy"
API_METADATA_URL="https://api.dropboxapi.com/2/files/get_metadata"
API_LIST_FOLDER_URL="https://api.dropboxapi.com/2/files/list_folder"
API_LIST_FOLDER_CONTINUE_URL="https://api.dropboxapi.com/2/files/list_folder/continue"
API_ACCOUNT_INFO_URL="https://api.dropboxapi.com/2/users/get_current_account"
API_ACCOUNT_SPACE_URL="https://api.dropboxapi.com/2/users/get_space_usage"
API_MKDIR_URL="https://api.dropboxapi.com/2/files/create_folder"
API_SHARE_URL="https://api.dropboxapi.com/2/sharing/create_shared_link_with_settings"
API_SHARE_LIST="https://api.dropboxapi.com/2/sharing/list_shared_links"
API_SAVEURL_URL="https://api.dropboxapi.com/2/files/save_url"
API_SAVEURL_JOBSTATUS_URL="https://api.dropboxapi.com/2/files/save_url/check_job_status"
API_SEARCH_URL="https://api.dropboxapi.com/2/files/search"
APP_CREATE_URL="https://www.dropbox.com/developers/apps"
RESPONSE_FILE="$TMP_DIR/du_resp_$RANDOM"
CHUNK_FILE="$TMP_DIR/du_chunk_$RANDOM"
TEMP_FILE="$TMP_DIR/du_tmp_$RANDOM"
BIN_DEPS="sed базовое имя дата grep stat dd mkdir"
VERSION="1.0"

umask 077

#Проверьте оболочку
if [ -z "$BASH_VERSION" ]; then
    echo -e "Ошибка: для этого скрипта требуется оболочка BASH!"
    exit 1
fi

shopt -s nullglob #Bash позволяет шаблонам имен файлов, которые не соответствуют ни одному файлу, расширяться до нулевой строки, а не до самих себя.
shopt -s dotglob  #Bash включает имена файлов, начинающиеся с «.» в результатах расширения имени файла

#Проверьте временную папку
if [[ ! -d "$TMP_DIR" ]]; then
    echo -e "Ошибка: временная папка $TMP_DIR не существует!"
    echo -e "Please edit this script and set the TMP_DIR variable to a valid temporary folder to use."
    exit 1
fi

#Найдите дополнительный параметр файла конфигурации.
while getopts ":qpskdhf:x:" opt; do
    case $opt in

    f)
      CONFIG_FILE=$OPTARG
    ;;

    d)
      DEBUG=1
    ;;

    q)
      QUIET=1
    ;;

    p)
      SHOW_PROGRESSBAR=1
    ;;

    k)
      CURL_ACCEPT_CERTIFICATES="-k"
    ;;

    s)
      SKIP_EXISTING_FILES=1
    ;;

    h)
      HUMAN_READABLE_SIZE=1
    ;;

    x)
      EXCLUDE+=( $OPTARG )
    ;;

    \?)
      echo "Неверный параметр: -$OPTARG." >&2
      exit 1
    ;;

    :)
      echo "Опция -$OPTARG требует аргумента." >&2
      exit 1
    ;;

  esac
done

if [[ $DEBUG != 0 ]]; then
    echo $VERSION
    uname -a 2> /dev/null
    cat /etc/issue 2> /dev/null
    set -x
    RESPONSE_FILE="$TMP_DIR/du_resp_debug"
fi

if [[ $CURL_BIN == "" ]]; then
    BIN_DEPS="$BIN_DEPS локон"
    CURL_BIN="curl"
fi

#Проверка зависимостей
which $BIN_DEPS > /dev/null
if [[ $? != 0 ]]; then
    for i in $BIN_DEPS; do
        which $i > /dev/null ||
            NOT_FOUND="$я $NOT_FOUND"
    done
    echo -e "Ошибка: Требуемая программа не найдена: $NOT_FOUND."
    exit 1
fi

#Проверьте, установлен ли readlink и поддерживает ли параметр -m
#В этом нет необходимости, поэтому нет проблем, если он не установлен.
which readlink > /dev/null
if [[ $? == 0 && $(readlink -m "//test" 2> /dev/null) == "/test" ]]; then
    HAVE_READLINK=1
else
    HAVE_READLINK=0
fi

#Принудительное использование встроенного printf, если он присутствует, потому что так лучше
#в противном случае будет использоваться внешняя программа printf.
#Обратите внимание, что внешняя команда printf может вызвать проблемы с кодировкой символов!
builtin printf "" 2> /dev/null
if [[ $? == 0 ]]; then
    PRINTF="встроенный принтер"
    PRINTF_OPT="-в о"
else
    PRINTF=$(which printf)
    if [[ $? != 0 ]]; then
        echo -e "Ошибка: необходимая программа не найдена: printf"
    fi
    PRINTF_OPT=""
fi

#Распечатайте сообщение на основе переменной $QUIET.
function print
{
    if [[ $QUIET == 0 ]]; then
	    echo -ne "$1";
    fi
}

#Возвращает временную метку unix
function utime
{
    date '+%s'
}

#Удалить временные файлы
function remove_temp_files
{
    if [[ $DEBUG == 0 ]]; then
        rm -fr "$RESPONSE_FILE"
        rm -fr "$CHUNK_FILE"
        rm -fr "$TEMP_FILE"
    fi
}

#Преобразует байты в удобочитаемый формат.
function convert_bytes
{
    if [[ $HUMAN_READABLE_SIZE == 1 && "$1" != "" ]]; then
	    if (($1 > 1073741824));then
	        echo $(($1/1073741824)).$(($1%1073741824/100000000))"G";
	    elif (($1 > 1048576));then
	        echo $(($1/1048576)).$(($1%1048576/100000))"M";
	    elif (($1 > 1024));then
	        echo $(($1/1024)).$(($1%1024/100))"K";
	    else
	        echo $1;
	    fi
    else
	    echo $1;
    fi
}

#Возвращает размер файла в байтах
function file_size
{
    #Общий GNU
    SIZE=$(stat --format="%s" "$1" 2> /dev/null)
    if [ $? -eq 0 ]; then
        echo $SIZE
        return
    fi

    #Некоторые встроенные устройства Linux
    SIZE=$(stat -c "%s" "$1" 2> /dev/null)
    if [ $? -eq 0 ]; then
        echo $SIZE
        return
    fi

    #BSD, OSX и другие ОС
    SIZE=$(stat -f "%z" "$1" 2> /dev/null)
    if [ $? -eq 0 ]; then
        echo $SIZE
        return
    fi

    echo "0"
}


#Usage
function usage
{
    echo -e "Dropbox Uploader v$VERSION"
    echo -e "Андреа Фабризи - andrea.fabrizi@gmail.com\n"
    echo -e "Использование: $0 КОМАНДА [ПАРАМЕТРЫ]..."
    echo -e "\nCommands:"

    echo -e "\t upload   <LOCAL_FILE/DIR ...>  <REMOTE_FILE/DIR>"
    echo -e "\t скачать <REMOTE_FILE/DIR> [LOCAL_FILE/DIR]"
    echo -e "\t удалить <REMOTE_FILE/DIR>"
    echo -e "\t move     <REMOTE_FILE/DIR> <REMOTE_FILE/DIR>"
    echo -e "\t copy     <REMOTE_FILE/DIR> <REMOTE_FILE/DIR>"
    echo -e "\t mkdir <REMOTE_DIR>"
    echo -e "\t список [REMOTE_DIR]"
    echo -e "\t монитор [REMOTE_DIR] [TIMEOUT]"
    echo -e "\t поделиться <REMOTE_FILE>"
    echo -e "\t saveurl  <URL> <REMOTE_DIR>"
    echo -e "\t поиск <QUERY>"
    echo -e "\t информация"
    echo -e "\t пробел"
    echo -e "\t отсоединить"

    echo -e "\nНеобязательные параметры:"
    echo -e "\t-f <FILENAME> Загрузить файл конфигурации из определенного файла."
    echo -e "\t-s Пропускать уже существующие файлы при загрузке/загрузке. По умолчанию: перезаписать"
    echo -e "\t-d Включить режим отладки"
    echo -e "\t-q Тихий режим. Не показывать сообщения"
    echo -e "\t-h Показывать размеры файлов в удобочитаемом формате."
    echo -e "\t-p Показать индикатор прогресса cURL"
    echo -e "\t-k            Doesn't check for SSL certificates (insecure)"
    echo -e "\t-x Игнорирует/исключает каталоги или файлы из синхронизации. -x имя файла -x имя каталога. пример: -x .git"

    echo -en "\nДополнительную информацию и примеры можно найти в файле README.\n\n"
    remove_temp_files
    exit 1
}

#Проверьте код выхода Curl
function check_http_response
{
    CODE=$?

    #Проверка кода выхода Curl
    case $CODE in

        #OK
        0)

        ;;

        #Ошибка прокси
        5)
            print "\nОшибка: не удалось разрешить прокси. Не удалось разрешить данный прокси-хост.\n"

            remove_temp_files
            exit 1
        ;;

        #Отсутствуют сертификаты CA
        60|58|77)
            print "\nОшибка: cURL не может выполнить проверку однорангового SSL-сертификата.\n"
            print "Пожалуйста, установите пакет сертификатов CA по умолчанию.\n"
            print "Чтобы сделать это в системе на базе Debian/Ubuntu, попробуйте:\n"
            print "  sudo apt-get install ca-сертификаты\n\n"
            print "If the problem persists, try to use the -k option (insecure).\n"

            remove_temp_files
            exit 1
        ;;

        6)
            print "\nОшибка: не удалось разрешить хост.\n"

            remove_temp_files
            exit 1
        ;;

        7)
            print "\nОшибка: не удалось подключиться к хосту.\n"

            remove_temp_files
            exit 1
        ;;

    esac

    #Проверка файла ответов на наличие общих ошибок
    if grep -q "HTTP/1.1 400" "$RESPONSE_FILE"; then
        ERROR_MSG=$(sed -n -e 's/{"error": "\([^"]*\)"}/\1/p' "$RESPONSE_FILE")

        case $ERROR_MSG in
             *access?attempt?failed?because?this?app?is?not?configured?to?have*)
                echo -e "\nОшибка: настроенный тип разрешения/уровень доступа не соответствует настройкам приложения DropBox!\nЗапустите \"$0 unlink\" и повторите попытку."
                exit 1
            ;;
        esac

    fi

}

#Urlencode
function urlencode
{
    #printf необходим для правильного декодирования последовательностей Юникода.
    local string=$($PRINTF "${1}")
    local strlen=${#нить}
    local encoded=""

    for (( pos=0 ; pos<strlen ; pos++ )); do
        c=${string:$pos:1}
        case "$c" in
            [-_.~a-zA-Z0-9] ) o="${c}" ;;
            * ) $PRINTF $PRINTF_OPT '%%%02x' "'$c"
        esac
        encoded="${encoded}${o}"
    done

    echo "$закодировано"
}

function normalize_path
{
    #printf необходим для правильного декодирования последовательностей Юникода.
    path=$($PRINTF "${1//\/\///}")
    if [[ $HAVE_READLINK == 1 ]]; then
        new_path=$(readlink -m "$путь")

        #Добавление последней косой черты, если она есть в исходном коде.
        if [[ ${path: -1} == "/" && ${#путь} -gt 1 ]]; затем
            new_path="$new_path/"
        fi

        echo "$new_path"
    else
        echo "$путь"
    fi
}

#Проверьте, является ли это файлом или каталогом
#Возвращает ФАЙЛ/КАТАЛОГ/ОШИБКА
function db_stat
{
    local FILE=$(normalize_path "$1")

    if [[ $FILE == "/" ]]; then
        echo "DIR"
        return
    fi

    #Проверка, является ли это файлом или каталогом
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"path\": \"$FILE\"}" "$API_METADATA_URL" 2> /dev/null
    check_http_response

    local TYPE=$(sed -n 's/{".tag": *"*\([^"]*\)"*.*/\1/p' "$RESPONSE_FILE")

    case $TYPE in

        file)
            echo "FILE"
        ;;

        folder)
            echo "DIR"
        ;;

        deleted)
            echo "ERR"
        ;;

        *)
            echo "ERR"
        ;;

    esac
}

#Общая оболочка загрузки для функций db_upload_file и db_upload_dir.
#$1 = локальный исходный файл/каталог
#$2 = файл/каталог удаленного назначения
function db_upload
{
    local SRC=$(normalize_path "$1")
    local DST=$(normalize_path "$2")

    for j in "${EXCLUDE[@]}"
        do :
            if [[ $(echo "$SRC" | grep "$j" | wc -l) -gt 0 ]]; then
                print "Пропуск исключенного файла/каталога: "$j
                return
            fi
    done

    #Проверка существования файла/каталога
    if [[ ! -e $SRC && ! -d $SRC ]]; then
        print " > Нет такого файла или каталога: $SRC\n"
        ERROR_STATUS=1
        return
    fi

    #Проверка наличия у файла/каталога разрешений на чтение
    if [[ ! -r $SRC ]]; then
        print " > Ошибка чтения файла $SRC: разрешение отклонено\n"
        ERROR_STATUS=1
        return
    fi

    TYPE=$(db_stat "$летнее время")

    #Если DST — это файл, ничего не делайте, это поведение по умолчанию.
    if [[ $TYPE == "FILE" ]]; then
        DST="$летнее время"

    #if DST doesn't exists and doesn't ends with a /, it will be the destination file name
    elif [[ $TYPE == "ERR" && "${DST: -1}" != "/" ]]; then
        DST="$летнее время"

    #if DST doesn't exists and ends with a /, it will be the destination folder
    elif [[ $TYPE == "ERR" && "${DST: -1}" == "/" ]]; then
        local filename=$(basename "$SRC")
        DST="$DST/$имя_файла"

    #Если DST — это каталог, это будет папка назначения.
    elif [[ $TYPE == "DIR" ]]; then
        local filename=$(basename "$SRC")
        DST="$DST/$имя_файла"
    fi

    #Это каталог
    if [[ -d $SRC ]]; then
        db_upload_dir "$SRC" "$летнее время"

    #Это файл
    elif [[ -e $SRC ]]; then
        db_upload_file "$SRC" "$летнее время"

    #Неподдерживаемый объект...
    else
        print " > Пропуск не обычного файла \"$SRC\"\n"
    fi
}

#Общая оболочка загрузки для db_chunked_upload_file и db_simple_upload_file.
#Окончательная функция загрузки будет выбрана в зависимости от размера файла.
#$1 = локальный исходный файл
#$2 = файл удаленного назначения
function db_upload_file
{
    local FILE_SRC=$(normalize_path "$1")
    local FILE_DST=$(normalize_path "$2")

    shopt -s nocasematch

    #Проверка запрещенных имен файлов
    basefile_dst=$(basename "$FILE_DST")
    if [[ $basefile_dst == "thumbs.db" || \
          $basefile_dst == "desktop.ini" || \
          $basefile_dst == ".ds_store" || \
          $basefile_dst == "icon\r" || \
          $basefile_dst == ".dropbox" || \
          $basefile_dst == ".dropbox.attr" \
       ]]; then
        print " > Пропуск запрещенного имени файла \"$FILE_DST\"\n"
        return
    fi

    shopt -u nocasematch

    #Проверка размера файла
    FILE_SIZE=$(file_size "$FILE_SRC")

    #Проверяем, существует ли файл уже
    TYPE=$(db_stat "$FILE_DST")
    if [[ $TYPE != "ERR" && $SKIP_EXISTING_FILES == 1 ]]; then
        print " > Пропуск существующего файла \"$FILE_DST\"\n"
        return
    fi

    # Проверка наличия у файла правильной контрольной суммы
    if [[ $TYPE != "ERR" ]]; then
        sha_src=$(db_sha_local "$FILE_SRC")
        sha_dst=$(db_sha "$FILE_DST")
        if [[ $sha_src == $sha_dst && $sha_src != "ERR" ]]; then
            print "> Пропуск файла \"$FILE_SRC\", файл существует с таким же хешем\n"
            return
        fi
    fi

    if [[ $FILE_SIZE -gt 157286000 ]]; then
        #Если размер файла превышает 150 МБ, будет использоваться API chunked_upload.
        db_chunked_upload_file "$FILE_SRC" "$FILE_DST"
    else
        db_simple_upload_file "$FILE_SRC" "$FILE_DST"
    fi

}

#Простая загрузка файла
#$1 = локальный исходный файл
#$2 = файл удаленного назначения
function db_simple_upload_file
{
    local FILE_SRC=$(normalize_path "$1")
    local FILE_DST=$(normalize_path "$2")

    if [[ $SHOW_PROGRESSBAR == 1 && $QUIET == 0 ]]; then
        CURL_PARAMETERS="--progress-bar"
        LINE_CR="\n"
    else
        CURL_PARAMETERS="-Л -с"
        LINE_CR=""
    fi

    print " > Загрузка \"$FILE_SRC\" в \"$FILE_DST\"... $LINE_CR"
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES $CURL_PARAMETERS -X POST -i --globoff -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Dropbox-API-Arg: {\"path\": \"$FILE_DST\",\"mode\": \"overwrite\",\"autorename\": true,\"mute\": false}" --header "Тип контента: приложение/октет-поток" --data-binary @"$FILE_SRC" "$API_UPLOAD_URL"
    check_http_response

    #Check
    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
        print "DONE\n"
    else
        print "FAILED\n"
        print "Произошла ошибка при запросе /upload\n"
        ERROR_STATUS=1
    fi
}

#Загрузка фрагментированного файла
#$1 = локальный исходный файл
#$2 = файл удаленного назначения
function db_chunked_upload_file
{
    local FILE_SRC=$(normalize_path "$1")
    local FILE_DST=$(normalize_path "$2")


    if [[ $SHOW_PROGRESSBAR == 1 && $QUIET == 0 ]]; then
        VERBOSE=1
        CURL_PARAMETERS="--progress-bar"
    else
        VERBOSE=0
        CURL_PARAMETERS="-Л -с"
    fi



    local FILE_SIZE=$(file_size "$FILE_SRC")
    local OFFSET=0
    local UPLOAD_ID=""
    local UPLOAD_ERROR=0
    local CHUNK_PARAMS=""

    ## Разделение потолка
    let NUMBEROFCHUNK=($FILE_SIZE/1024/1024+$CHUNK_SIZE-1)/$CHUNK_SIZE

    if [[ $VERBOSE == 1 ]]; then
        print " > Загрузка \"$FILE_SRC\" в \"$FILE_DST\" частями $NUMBEROFCHUNK ...\n"
    else
        print " > Загрузка \"$FILE_SRC\" в \"$FILE_DST\" частями $NUMBEROFCHUNK "
    fi

    #Начинаем новый сеанс загрузки
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Dropbox-API-Arg: {\"close\": false}" --header "Тип контента: приложение/октет-поток" --data-binary @/dev/null "$API_CHUNKED_UPLOAD_START_URL" 2> /dev/null
    check_http_response

    SESSION_ID=$(sed -n 's/{"session_id": *"*\([^"]*\)"*.*/\1/p' "$RESPONSE_FILE")

    chunkNumber=1
    #Загрузка фрагментов...
    while ([[ $OFFSET != "$FILE_SIZE" ]]); do

        let OFFSET_MB=$OFFSET/1024/1024

        #Создать чанк
        dd if="$FILE_SRC" of="$CHUNK_FILE" bs=1048576 skip=$OFFSET_MB count=$CHUNK_SIZE 2> /dev/null
        local CHUNK_REAL_SIZE=$(file_size "$CHUNK_FILE")

        if [[ $VERBOSE == 1 ]]; then
            print " >> Загрузка фрагмента $chunkNumber из $NUMBEROFCHUNK\n"
        fi

        #Загрузка фрагмента...
        echo > "$RESPONSE_FILE"
        $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST $CURL_PARAMETERS --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Dropbox-API-Arg: {\"курсор\": {\"session_id\": \"$SESSION_ID\",\"offset\": $OFFSET},\"закрыть\": false}" --header "Тип контента: приложение/октет-поток" --data-binary @"$CHUNK_FILE" "$API_CHUNKED_UPLOAD_APPEND_URL"
        #check_http_response не нужен, так как в случае ошибки придется повторить запрос

        #Check
        if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
            let OFFSET=$OFFSET+$CHUNK_REAL_SIZE
            UPLOAD_ERROR=0
            if [[ $VERBOSE != 1 ]]; then
                print "."
            fi
            ((chunkNumber=chunkNumber+1))
        else
            if [[ $VERBOSE != 1 ]]; then
                print "*"
            fi
            let UPLOAD_ERROR=$UPLOAD_ERROR+1

            #В случае ошибки загрузка повторяется максимум 3 раза.
            if [[ $UPLOAD_ERROR -gt 2 ]]; then
                print " FAILED\n"
                print "Произошла ошибка при запросе /chunked_upload\n"
                ERROR_STATUS=1
                return
            fi
        fi

    done

    UPLOAD_ERROR=0

    #Подтвердить загрузку
    while (true); do

        echo > "$RESPONSE_FILE"
        $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Dropbox-API-Arg: {\"курсор\": {\"session_id\": \"$SESSION_ID\",\"offset\": $OFFSET},\"фиксация\": {\"path\": \"$FILE_DST\",\"mode\": \"overwrite\",\"autorename\": true,\"mute\": false}}" --header "Тип контента: приложение/октет-поток" --data-binary @/dev/null "$API_CHUNKED_UPLOAD_FINISH_URL" 2> /dev/null
        #check_http_response не нужен, так как в случае ошибки придется повторить запрос

        #Check
        if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
            UPLOAD_ERROR=0
            break
        else
            print "*"
            let UPLOAD_ERROR=$UPLOAD_ERROR+1

            #В случае ошибки фиксация повторяется максимум 3 раза.
            if [[ $UPLOAD_ERROR -gt 2 ]]; then
                print " FAILED\n"
                print "Произошла ошибка при запросе /commit_chunked_upload\n"
                ERROR_STATUS=1
                return
            fi
        fi

    done

    print " DONE\n"
}

#Загрузка каталога
#$1 = локальный исходный каталог
#$2 = каталог удаленного назначения
function db_upload_dir
{
    local DIR_SRC=$(normalize_path "$1")
    local DIR_DST=$(normalize_path "$2")

    #Создание удаленного каталога
    db_mkdir "$DIR_DST"

    for file in "$DIR_SRC/"*; do
        db_upload "$файл" "$DIR_DST"
    done
}

#Общая оболочка загрузки
#$1 = удаленный исходный файл/каталог
#$2 = локальный файл/каталог назначения
function db_download
{
    local SRC=$(normalize_path "$1")
    local DST=$(normalize_path "$2")

    TYPE=$(db_stat "$SRC")

    #Это каталог
    if [[ $TYPE == "DIR" ]]; then

        #Если папка DST не указана, я предполагаю, что это текущий каталог.
        if [[ $DST == "" ]]; then
            DST="."
        fi

        #Проверка существования каталога назначения
        if [[ ! -d $DST ]]; then
            local basedir=""
        else
            local basedir=$(basename "$SRC")
        fi

        local DEST_DIR=$(normalize_path "На основе $летнего времени/$")
        print " > Загрузка папки \"$SRC\" в \"$DEST_DIR\"... \n"

        if [[ ! -d "$DEST_DIR" ]]; then
            print " > Создание локального каталога \"$DEST_DIR\"... "
            mkdir -p "$DEST_DIR"

            #Check
            if [[ $? == 0 ]]; then
                print "DONE\n"
            else
                print "FAILED\n"
                ERROR_STATUS=1
                return
            fi
        fi

        if [[ $SRC == "/" ]]; then
            SRC_REQ=""
        else
            SRC_REQ="$SRC"
        fi

        OUT_FILE=$(db_list_outfile "$SRC_REQ")
        if [ $? -ne 0 ]; then
            # При сбое db_list_outfile появляется сообщение об ошибке OUT_FILE.
            print "$OUT_FILE\n"
            ERROR_STATUS=1
            return
        fi

        #За каждую запись...
        while read -r line; do

            local FILE=${line%:*}
            local META=${line##*:}
            local TYPE=${META%;*}
            local SIZE=${META#*;}

            #Удаление ненужного/
            FILE=${FILE##*/}

            if [[ $TYPE == "file" ]]; then
                db_download_file "$SRC/$ФАЙЛ" "$DEST_DIR/$FILE"
            elif [[ $TYPE == "folder" ]]; then
                db_download "$SRC/$ФАЙЛ" "$DEST_DIR"
            fi

        done < $OUT_FILE

        rm -fr $OUT_FILE

    #Это файл
    elif [[ $TYPE == "FILE" ]]; then

        #Проверка летнего времени
        if [[ $DST == "" ]]; then
            DST=$(basename "$SRC")
        fi

        #Если местом назначения является каталог, файл будет загружен в
        if [[ -d $DST ]]; then
            DST="$DST/$SRC"
        fi

        db_download_file "$SRC" "$летнее время"

    #Не существует
    else
        print " > Нет такого файла или каталога: $SRC\n"
        ERROR_STATUS=1
        return
    fi
}

#Простая загрузка файла
#$1 = удаленный исходный файл
#$2 = локальный файл назначения
function db_download_file
{
    local FILE_SRC=$(normalize_path "$1")
    local FILE_DST=$(normalize_path "$2")

    if [[ $SHOW_PROGRESSBAR == 1 && $QUIET == 0 ]]; then
        CURL_PARAMETERS="-L --прогресс-бар"
        LINE_CR="\n"
    else
        CURL_PARAMETERS="-Л -с"
        LINE_CR=""
    fi

    #Проверяем, существует ли файл уже
    if [[ -e $FILE_DST && $SKIP_EXISTING_FILES == 1 ]]; then
        print " > Пропуск существующего файла \"$FILE_DST\"\n"
        return
    fi

    # Проверка наличия у файла правильной контрольной суммы
    if [[ $TYPE != "ERR" ]]; then
        sha_src=$(db_sha "$FILE_SRC")
        sha_dst=$(db_sha_local "$FILE_DST")
        if [[ $sha_src == $sha_dst && $sha_src != "ERR" ]]; then
            print "> Пропуск файла \"$FILE_SRC\", файл существует с таким же хешем\n"
            return
        fi
    fi

    #Создание пустого файла по двум причинам:
    #1) In this way I can check if the destination file is writable or not
    #2) Curl не создает автоматически файлы размером 0 байт.
    dd if=/dev/zero of="$FILE_DST" count=0 2> /dev/null
    if [[ $? != 0 ]]; then
        print " > Ошибка записи файла $FILE_DST: разрешение отклонено\n"
        ERROR_STATUS=1
        return
    fi

    print " > Загрузка \"$FILE_SRC\" в \"$FILE_DST\"... $LINE_CR"
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES $CURL_PARAMETERS -X POST --globoff -D "$RESPONSE_FILE" -o "$FILE_DST" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Dropbox-API-Arg: {\"path\": \"$FILE_SRC\"}" "$API_DOWNLOAD_URL"
    check_http_response

    #Check
    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
        print "DONE\n"
    else
        print "FAILED\n"
        rm -fr "$FILE_DST"
        ERROR_STATUS=1
        return
    fi
}

#Saveurl
#$1 = URL-адрес
#$2 = удаленное место назначения файла
function db_saveurl
{
    local URL="$1"
    local FILE_DST=$(normalize_path "$2")
    local FILE_NAME=$(basename "$URL")

    print " > Загрузка \"$URL\" в \"$FILE_DST\"..."
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"path\": \"$FILE_DST/$FILE_NAME\", \"url\": \"$URL\"}" "$API_SAVEURL_URL" 2> /dev/null
    check_http_response

    JOB_ID=$(sed -n 's/.*"async_job_id": *"*\([^"]*\)"*.*/\1/p' "$RESPONSE_FILE")
    if [[ $JOB_ID == "" ]]; then
        print " > Ошибка при получении идентификатора задания\n"
        return
    fi

    #Проверка статуса
    while (true); do

        $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"async_job_id\": \"$JOB_ID\"}" "$API_SAVEURL_JOBSTATUS_URL" 2> /dev/null
        check_http_response

        STATUS=$(sed -n 's/{".tag": *"*\([^"]*\)"*.*/\1/p' "$RESPONSE_FILE")
        case $STATUS in

            in_progress)
                print "+"
            ;;

            complete)
                print " DONE\n"
                break
            ;;

            failed)
                print " ERROR\n"
                MESSAGE=$(sed -n 's/.*"error_summary": *"*\([^"]*\)"*.*/\1/p' "$RESPONSE_FILE")
                print " > Ошибка: $MESSAGE\n"
                break
            ;;

        esac

        sleep 2

    done
}

#Распечатывает информацию об учетной записи
function db_account_info
{
    print "Программа загрузки Dropbox v$VERSION\n\n"
    print " > Получение информации... "
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" "$API_ACCOUNT_INFO_URL" 2> /dev/null
    check_http_response

    #Check
    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then

        name=$(sed -n 's/.*"display_name": "\([^"]*\).*/\1/p' "$RESPONSE_FILE")
        echo -e "\n\nИмя:\t\t$имя"

        uid=$(sed -n 's/.*"account_id": "\([^"]*\).*/\1/p' "$RESPONSE_FILE")
        echo -e "UID:\t\t$uid"

        email=$(sed -n 's/.*"email": "\([^"]*\).*/\1/p' "$RESPONSE_FILE")
        echo -e "Электронная почта:\t\t$email"

        country=$(sed -n 's/.*"country": "\([^"]*\).*/\1/p' "$RESPONSE_FILE")
        echo -e "Страна:\t$country"

        echo ""

    else
        print "FAILED\n"
        ERROR_STATUS=1
    fi
}

#Распечатывает информацию об использовании пространства учетной записи.
function db_account_space
{
    print "Программа загрузки Dropbox v$VERSION\n\n"
    print " > Получение информации об использовании пространства... "
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" "$API_ACCOUNT_SPACE_URL" 2> /dev/null
    check_http_response

    #Check
    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then

        quota=$(sed -n 's/.*"выделено": \([0-9]*\).*/\1/p' "$RESPONSE_FILE")
        let quota_mb=$quota/1024/1024
        echo -e "\n\nQuota:\t$quota_mb Мб"

        used=$(sed -n 's/.*"used": \([0-9]*\).*/\1/p' "$RESPONSE_FILE")
        let used_mb=$used/1024/1024
        echo -e "Использовано:\t$used_mb Мб"

		let free_mb=$((quota-used))/1024/1024
        echo -e "Бесплатно:\t$free_mb Мб"

        echo ""

    else
        print "FAILED\n"
        ERROR_STATUS=1
    fi
}

#Отмена привязки аккаунта
function db_unlink
{
    echo -ne "Вы уверены, что хотите отменить связь этого скрипта с вашей учетной записью Dropbox? [да/нет]"
    read -r answer
    if [[ $answer == "y" ]]; then
        rm -fr "$CONFIG_FILE"
        echo -ne "DONE\n"
    fi
}

#Удалить удаленный файл
#$1 = удаленный файл для удаления
function db_delete
{
    local FILE_DST=$(normalize_path "$1")

    print " > Удаление \"$FILE_DST\"... "
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"path\": \"$FILE_DST\"}" "$API_DELETE_URL" 2> /dev/null
    check_http_response

    #Check
    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
        print "DONE\n"
    else
        print "FAILED\n"
        ERROR_STATUS=1
    fi
}

#Переместить/переименовать удаленный файл
#$1 = удаленный файл для переименования или перемещения.
#$2 = новое имя или местоположение файла.
function db_move
{
    local FILE_SRC=$(normalize_path "$1")
    local FILE_DST=$(normalize_path "$2")

    TYPE=$(db_stat "$FILE_DST")

    #Если местом назначения является каталог, источник будет перемещен в него.
    if [[ $TYPE == "DIR" ]]; then
        local filename=$(basename "$FILE_SRC")
        FILE_DST=$(normalize_path "$FILE_DST/$имя_файла")
    fi

    print " > Перемещение \"$FILE_SRC\" в \"$FILE_DST\"... "
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"from_path\": \"$FILE_SRC\", \"to_path\": \"$FILE_DST\"}" "$API_MOVE_URL" 2> /dev/null
    check_http_response

    #Check
    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
        print "DONE\n"
    else
        print "FAILED\n"
        ERROR_STATUS=1
    fi
}

#Скопируйте удаленный файл в удаленное место
#$1 = удаленный файл для переименования или перемещения.
#$2 = новое имя или местоположение файла.
function db_copy
{
    local FILE_SRC=$(normalize_path "$1")
    local FILE_DST=$(normalize_path "$2")

    TYPE=$(db_stat "$FILE_DST")

    #Если местом назначения является каталог, в него будет скопирован источник.
    if [[ $TYPE == "DIR" ]]; then
        local filename=$(basename "$FILE_SRC")
        FILE_DST=$(normalize_path "$FILE_DST/$имя_файла")
    fi

    print " > Копирование \"$FILE_SRC\" в \"$FILE_DST\"... "
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"from_path\": \"$FILE_SRC\", \"to_path\": \"$FILE_DST\"}" "$API_COPY_URL" 2> /dev/null
    check_http_response

    #Check
    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
        print "DONE\n"
    else
        print "FAILED\n"
        ERROR_STATUS=1
    fi
}

#Создать новый каталог
#$1 = удаленный каталог для создания
function db_mkdir
{
    local DIR_DST=$(normalize_path "$1")

    print " > Создание каталога \"$DIR_DST\"... "
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"path\": \"$DIR_DST\"}" "$API_MKDIR_URL" 2> /dev/null
    check_http_response

    #Check
    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
        print "DONE\n"
    elif grep -q "^HTTP/1.1 403 Запрещено" "$RESPONSE_FILE"; then
        print "УЖЕ СУЩЕСТВУЕТ\n"
    else
        print "FAILED\n"
        ERROR_STATUS=1
    fi
}

#Перечислить удаленную папку и вернуть путь к файлу, содержащему выходные данные.
#$1 = удаленный каталог
#$2 = курсор (необязательно)
function db_list_outfile
{

    local DIR_DST="$1"
    local HAS_MORE="false"
    local CURSOR=""

    if [[ -n "$2" ]]; then
        CURSOR="$2"
        HAS_MORE="true"
    fi

    OUT_FILE="$TMP_DIR/du_tmp_out_$RANDOM"

    while (true); do

        if [[ $HAS_MORE == "true" ]]; then
            $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"cursor\": \"$CURSOR\"}" "$API_LIST_FOLDER_CONTINUE_URL" 2> /dev/null
        else
            $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"path\": \"$DIR_DST\",\"include_media_info\": false,\"include_deleted\": false,\"include_has_explicit_shared_members\": false}" "$API_LIST_FOLDER_URL" 2> /dev/null
        fi

        check_http_response

        HAS_MORE=$(sed -n 's/.*"has_more": *\([a-z]*\).*/\1/p' "$RESPONSE_FILE")
        CURSOR=$(sed -n 's/.*"cursor": *"\([^"]*\)".*/\1/p' "$RESPONSE_FILE")

        #Check
        if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then

            #Извлечение содержимого каталога [...]
            #и заменив "}, {" with "}\n{"
            #Мне не нравится этот фрагмент кода... но, похоже, это единственный способ сделать это с помощью SED - написать переносимый код...
            local DIR_CONTENT=$(sed -n 's/.*: \[{\(.*\)/\1/p' "$RESPONSE_FILE" | sed 's/}, *{/}\
    {/g')

            #Преобразование экранированных кавычек в формат Юникода
            echo "$DIR_CONTENT" | sed 's/\\"/\\u0022/' > "$TEMP_FILE"

            #Извлечение файлов и подпапок
            while read -r line; do

                local FILE=$(echo "$строка" | sed -n 's/.*"path_display": *"\([^"]*\)".*/\1/p')
                local TYPE=$(echo "$строка" | sed -n 's/.*".tag": *"\([^"]*\).*/\1/p')
                local SIZE=$(convert_bytes $(echo "$строка" | sed -n 's/.*"size": *\([0-9]*\).*/\1/p'))

                echo -e "$FILE:$TYPE;$SIZE" >> "$OUT_FILE"

            done < "$TEMP_FILE"

            if [[ $HAS_MORE == "false" ]]; then
                break
            fi

        else
            return
        fi

    done

    echo $OUT_FILE
}

#Список удаленных каталогов
#$1 = удаленный каталог
function db_list
{
    local DIR_DST=$(normalize_path "$1")

    print " > Листинг \"$DIR_DST\"... "

    if [[ "$DIR_DST" == "/" ]]; then
        DIR_DST=""
    fi

    OUT_FILE=$(db_list_outfile "$DIR_DST")
    if [ -z "$OUT_FILE" ]; then
        print "FAILED\n"
        ERROR_STATUS=1
        return
    else
        print "DONE\n"
    fi

    #Ищем самый большой размер файла
    #для расчета заполнения, которое будет использоваться
    local padding=0
    while read -r line; do
        local FILE=${line%:*}
        local META=${line##*:}
        local SIZE=${META#*;}

        if [[ ${#РАЗМЕР} -gt $padding ]]; затем
            padding=${#РАЗМЕР}
        fi
    done < "$OUT_FILE"

    #Для каждой записи распечатка каталогов...
    while read -r line; do

        local FILE=${line%:*}
        local META=${line##*:}
        local TYPE=${META%;*}
        local SIZE=${META#*;}

        #Удаление ненужного/
        FILE=${FILE##*/}

        if [[ $TYPE == "folder" ]]; then
            FILE=$(echo -e "$ФАЙЛ")
            $PRINTF " [D] %-${padding}с %s\n" "$SIZE" "$ФАЙЛ"
        fi

    done < "$OUT_FILE"

    #Для каждой записи распечатка файлов...
    while read -r line; do

        local FILE=${line%:*}
        local META=${line##*:}
        local TYPE=${META%;*}
        local SIZE=${META#*;}

        #Удаление ненужного/
        FILE=${FILE##*/}

        if [[ $TYPE == "file" ]]; then
            FILE=$(echo -e "$ФАЙЛ")
            $PRINTF " [F] %-${padding}s %s\n" "$SIZE" "$ФАЙЛ"
        fi

    done < "$OUT_FILE"

    rm -fr "$OUT_FILE"
}

#Удаленный каталог Longpoll только один раз
#$1 = тайм-аут
#$2 = удаленный каталог
function db_monitor_nonblock
{
    local TIMEOUT=$1
    local DIR_DST=$(normalize_path "$2")

    if [[ "$DIR_DST" == "/" ]]; then
        DIR_DST=""
    fi

    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"path\": \"$DIR_DST\",\"include_media_info\": false,\"include_deleted\": false,\"include_has_explicit_shared_members\": false}" "$API_LIST_FOLDER_URL" 2> /dev/null
    check_http_response

    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then

        local CURSOR=$(sed -n 's/.*"cursor": *"\([^"]*\)".*/\1/p' "$RESPONSE_FILE")

        $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Тип контента: приложение/json" --data "{\"курсор\": \"$CURSOR\", \"тайм-аут\": ${TIMEOUT}}" "$API_LONGPOLL_FOLDER" 2> /dev/null
        check_http_response

        if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
            local CHANGES=$(sed -n 's/.*"изменения" *: *\([a-z]*\).*/\1/p' "$RESPONSE_FILE")
        else
            ERROR_MSG=$(grep "Ошибка при звонке" "$RESPONSE_FILE")
            print "НЕ удалось выполнить длинный опрос (ошибка http): $ERROR_MSG\n"
            ERROR_STATUS=1
            return 1
        fi

        if [[ -z "$ИЗМЕНЕНИЯ" ]]; then
            print "НЕ удалось выполнить длинный опрос (неожиданный ответ)\n"
            ERROR_STATUS=1
            return 1
        fi

        if [ "$ИЗМЕНЕНИЯ" == "true" ]; then

            OUT_FILE=$(db_list_outfile "$DIR_DST" "$КУРСОР")

            if [ -z "$OUT_FILE" ]; then
                print "НЕ удалось вывести список изменений\n"
                ERROR_STATUS=1
                return
            fi

            #Для каждой записи распечатка каталогов...
            while read -r line; do

                local FILE=${line%:*}
                local META=${line##*:}
                local TYPE=${META%;*}
                local SIZE=${META#*;}

                #Удаление ненужного/
                FILE=${FILE##*/}

                if [[ $TYPE == "folder" ]]; then
                    FILE=$(echo -e "$ФАЙЛ")
                    $PRINTF " [D] %s\n" "$ФАЙЛ"
                elif [[ $TYPE == "file" ]]; then
                    FILE=$(echo -e "$ФАЙЛ")
                    $PRINTF " [F] %s %s\n" "$SIZE" "$ФАЙЛ"
                elif [[ $TYPE == "deleted" ]]; then
                    FILE=$(echo -e "$ФАЙЛ")
                    $PRINTF " [-] %s\n" "$ФАЙЛ"
                fi

            done < "$OUT_FILE"

            rm -fr "$OUT_FILE"
        fi

    else
        ERROR_STATUS=1
        return 1
    fi

}

#Longpoll постоянно удаленный каталог
#$1 = тайм-аут
#$2 = удаленный каталог
function db_monitor
{
    local TIMEOUT=$1
    local DIR_DST=$(normalize_path "$2")

    while (true); do
        db_monitor_nonblock "$TIMEOUT" "$2"
    done
}

#Поделиться удаленным файлом
#$1 = удаленный файл
function db_share
{
    local FILE_DST=$(normalize_path "$1")

    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"путь\": \"$FILE_DST\", \"настройки\": {\"requested_visibility\": \"public\"}}" "$API_SHARE_URL" 2> /dev/null
    check_http_response

    #Check
    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
        print " > Поделиться ссылкой: "
        SHARE_LINK=$(sed -n 's/.*"url": "\([^"]*\).*/\1/p' "$RESPONSE_FILE")
        echo "$SHARE_LINK"
    else
        get_Share "$FILE_DST"
    fi
}

#Запросить существующую общую ссылку
#$1 = удаленный файл
function get_Share
{
    local FILE_DST=$(normalize_path "$1")
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"path\": \"$FILE_DST\",\"direct_only\": true}" "$API_SHARE_LIST"
    check_http_response

    #Check
    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
        print " > Поделиться ссылкой: "
        SHARE_LINK=$(sed -n 's/.*"url": "\([^"]*\).*/\1/p' "$RESPONSE_FILE")
        echo "$SHARE_LINK"
    else
        print "FAILED\n"
        MESSAGE=$(sed -n 's/.*"error_summary": *"*\([^"]*\)"*.*/\1/p' "$RESPONSE_FILE")
        print " > Ошибка: $MESSAGE\n"
        ERROR_STATUS=1
    fi
}

#Искать в Dropbox
#$1 = запрос
function db_search
{
    local QUERY="$1"

    print " > Поиск \"$QUERY\"... "

    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"path\": \"\",\"query\": \"$QUERY\",\"start\": 0,\"max_results\": 1000,\"mode\": \"filename\"}" "$API_SEARCH_URL" 2> /dev/null
    check_http_response

    #Check
    if grep -q "^HTTP/1.1 200 ОК" "$RESPONSE_FILE"; then
        print "DONE\n"
    else
        print "FAILED\n"
        ERROR_STATUS=1
    fi

    #Извлечение содержимого каталога [...]
    #и заменив "}, {" with "}\n{"
    #Мне не нравится этот фрагмент кода... но, похоже, это единственный способ сделать это с помощью SED - написать переносимый код...
    local DIR_CONTENT=$(sed 's/}, *{/}\
{/g' "$RESPONSE_FILE")

    #Преобразование экранированных кавычек в формат Юникода
    echo "$DIR_CONTENT" | sed 's/\\"/\\u0022/' > "$TEMP_FILE"

    #Извлечение файлов и подпапок
    rm -fr "$RESPONSE_FILE"
    while read -r line; do

        local FILE=$(echo "$строка" | sed -n 's/.*"path_display": *"\([^"]*\)".*/\1/p')
        local TYPE=$(echo "$строка" | sed -n 's/.*".tag": *"\([^"]*\).*/\1/p')
        local SIZE=$(convert_bytes $(echo "$строка" | sed -n 's/.*"size": *\([0-9]*\).*/\1/p'))

        echo -e "$FILE:$TYPE;$SIZE" >> "$RESPONSE_FILE"

    done < "$TEMP_FILE"

    #Ищем самый большой размер файла
    #для расчета заполнения, которое будет использоваться
    local padding=0
    while read -r line; do
        local FILE=${line%:*}
        local META=${line##*:}
        local SIZE=${META#*;}

        if [[ ${#РАЗМЕР} -gt $padding ]]; затем
            padding=${#РАЗМЕР}
        fi
    done < "$RESPONSE_FILE"

    #Для каждой записи распечатка каталогов...
    while read -r line; do

        local FILE=${line%:*}
        local META=${line##*:}
        local TYPE=${META%;*}
        local SIZE=${META#*;}

        if [[ $TYPE == "folder" ]]; then
            FILE=$(echo -e "$ФАЙЛ")
            $PRINTF " [D] %-${padding}с %s\n" "$SIZE" "$ФАЙЛ"
        fi

    done < "$RESPONSE_FILE"

    #Для каждой записи распечатка файлов...
    while read -r line; do

        local FILE=${line%:*}
        local META=${line##*:}
        local TYPE=${META%;*}
        local SIZE=${META#*;}

        if [[ $TYPE == "file" ]]; then
            FILE=$(echo -e "$ФАЙЛ")
            $PRINTF " [F] %-${padding}s %s\n" "$SIZE" "$ФАЙЛ"
        fi

    done < "$RESPONSE_FILE"

}

#Запросить сумму sha256-dropbox удаленного файла
#see https://www.dropbox.com/developers/reference/content-hash for more information
#$1 = удаленный файл
function db_sha
{
    local FILE=$(normalize_path "$1")

    if [[ $FILE == "/" ]]; then
        echo "ERR"
        return
    fi

    #Проверяем, файл это или каталог, и получаем ша-сумму
    $CURL_BIN $CURL_ACCEPT_CERTIFICATES -X POST -L -s --show-error --globoff -i -o "$RESPONSE_FILE" --header "Авторизация: носитель $OAUTH_ACCESS_TOKEN" --header "Тип контента: приложение/json" --data "{\"path\": \"$FILE\"}" "$API_METADATA_URL" 2> /dev/null
    check_http_response

    local TYPE=$(sed -n 's/{".tag": *"*\([^"]*\)"*.*/\1/p' "$RESPONSE_FILE")
    if [[ $TYPE == "folder" ]]; then
        echo "ERR"
        return
    fi

    local SHA256=$(sed -n 's/.*"content_hash": "\([^"]*\).*/\1/p' "$RESPONSE_FILE")
    echo "$SHA256"
}

#Запросить сумму sha256-dropbox локального файла
#see https://www.dropbox.com/developers/reference/content-hash for more information
#$1 = локальный файл
function db_sha_local
{
    local FILE=$(normalize_path "$1")
    local FILE_SIZE=$(file_size "$ФАЙЛ")
    local OFFSET=0
    local SKIP=0
    local SHA_CONCAT=""

    which shasum > /dev/null
    if [[ $? != 0 ]]; then
        echo "ERR"
        return
    fi

    while ([[ $OFFSET -lt "$FILE_SIZE" ]]); do
        dd if="$ФАЙЛ" of="$CHUNK_FILE" bs=4194304 skip=$SKIP count=1 2> /dev/null
        local SHA=$(shasum -a 256 "$CHUNK_FILE" | awk '{print $1}')
        SHA_CONCAT="${SHA_CONCAT}${SHA}"

        let OFFSET=$OFFSET+4194304
        let SKIP=$SKIP+1
    done

    shaHex=$(echo $SHA_CONCAT | sed 's/\([0-9A-F]\{2\}\)/\\x\1/gI')
    echo -ne $shaHex | shasum -a 256 | awk '{print $1}'
}

################
#### НАСТРАИВАТЬ  ####
################

#ПРОВЕРКА ФАЙЛА АВТОРИЗАЦИИ
if [[ -e $CONFIG_FILE ]]; then

    #Загрузка данных... и при необходимости измените конфигурацию старого формата.
    source "$CONFIG_FILE" 2>/dev/null || {
        sed -i'' 's/:/=/' "$CONFIG_FILE" && source "$CONFIG_FILE" 2>/dev/null
    }

    #Проверяем, является ли это файлом конфигурации API v1.
    if [[ $APPKEY != "" || $APPSECRET != "" ]]; then
        echo -ne "Файл конфигурации содержит старые устаревшие токены oauth v1.\n"
        echo -ne "Пожалуйста, запустите сценарий еще раз и следуйте указаниям мастера настройки. Резервная копия старого файла конфигурации сохранена в $CONFIG_FILE.old\n."
        mv "$CONFIG_FILE" "$CONFIG_FILE".old
        exit 1
    fi

    #Проверка загруженных данных
    if [[ $OAUTH_ACCESS_TOKEN = "" ]]; then
        echo -ne "Ошибка загрузки данных из $CONFIG_FILE...\n"
        echo -ne "Рекомендуется запустить $0 unlink\n"
        remove_temp_files
        exit 1
    fi

#НОВАЯ НАСТРОЙКА...
else

    echo -ne "\n Вы запускаете этот скрипт впервые, следуйте инструкциям:\n\n"
    echo -ne " 1) Откройте следующий URL-адрес в своем браузере и войдите в систему, используя свою учетную запись: $APP_CREATE_URL\n"
    echo -ne " 2) Нажмите «Создать приложение», затем выберите «Приложение Dropbox API»\n"
    echo -ne " 3) Теперь приступайте к настройке, выбирая разрешения приложения и ограничения доступа к папке DropBox\n."
    echo -ne " 4) Введите желаемое «Имя приложения» (например, MyUploader$RANDOM$RANDOM$RANDOM)\n\n"

    echo -ne " Теперь нажмите кнопку «Создать приложение».\n\n"

    echo -ne " Когда ваше новое приложение будет успешно создано, нажмите кнопку «Создать».\n"
    echo -ne " в разделе «Сгенерированный токен доступа», затем скопируйте и вставьте сюда новый токен доступа:\n\n"

    echo -ne " # Токен доступа: "
    read -r OAUTH_ACCESS_TOKEN

    echo -ne "\n > Токен доступа — $OAUTH_ACCESS_TOKEN. Выглядит нормально? [да/нет]: "
    read -r answer
    if [[ $answer != "y" ]]; then
        remove_temp_files
        exit 1
    fi

    echo "OAUTH_ACCESS_TOKEN=$OAUTH_ACCESS_TOKEN" > "$CONFIG_FILE"
    echo "   Конфигурация сохранена."

    remove_temp_files
    exit 0
fi

################
#### НАЧИНАТЬ  ####
################

COMMAND="${*:$OPTIND:1}"
ARG1="${*:$OPTIND+1:1}"
ARG2="${*:$OPTIND+2:1}"

let argnum=$#-ВЫБОР

#ПРОВЕРКА ЗНАЧЕНИЙ ПАРАМЕТРОВ
case $COMMAND in

    upload)

        if [[ $argnum -lt 2 ]]; then
            usage
        fi

        FILE_DST="${*:$#:1}"

        for (( i=OPTIND+1; i<$#; я++ )); делать
            FILE_SRC="${*:$i:1}"
            db_upload "$FILE_SRC" "/$FILE_DST"
        done

    ;;

    download)

        if [[ $argnum -lt 1 ]]; then
            usage
        fi

        FILE_SRC="$ARG1"
        FILE_DST="$ARG2"

        db_download "/$FILE_SRC" "$FILE_DST"

    ;;

    saveurl)

        if [[ $argnum -lt 1 ]]; then
            usage
        fi

        URL=$ARG1
        FILE_DST="$ARG2"

        db_saveurl "$URL" "/$FILE_DST"

    ;;

    share)

        if [[ $argnum -lt 1 ]]; then
            usage
        fi

        FILE_DST="$ARG1"

        db_share "/$FILE_DST"

    ;;

    info)

        db_account_info

    ;;

    space)

        db_account_space

    ;;

    delete|remove)

        if [[ $argnum -lt 1 ]]; then
            usage
        fi

        FILE_DST="$ARG1"

        db_delete "/$FILE_DST"

    ;;

    move|rename)

        if [[ $argnum -lt 2 ]]; then
            usage
        fi

        FILE_SRC="$ARG1"
        FILE_DST="$ARG2"

        db_move "/$FILE_SRC" "/$FILE_DST"

    ;;

    copy)

        if [[ $argnum -lt 2 ]]; then
            usage
        fi

        FILE_SRC="$ARG1"
        FILE_DST="$ARG2"

        db_copy "/$FILE_SRC" "/$FILE_DST"

    ;;

    mkdir)

        if [[ $argnum -lt 1 ]]; then
            usage
        fi

        DIR_DST="$ARG1"

        db_mkdir "/$DIR_DST"

    ;;

    search)

        if [[ $argnum -lt 1 ]]; then
            usage
        fi

        QUERY=$ARG1

        db_search "$QUERY"

    ;;

    list)

        DIR_DST="$ARG1"

        #Проверка DIR_DST
        if [[ $DIR_DST == "" ]]; then
            DIR_DST="/"
        fi

        db_list "/$DIR_DST"

    ;;

    monitor)

        DIR_DST="$ARG1"
        TIMEOUT=$ARG2

        #Проверка DIR_DST
        if [[ $DIR_DST == "" ]]; then
            DIR_DST="/"
        fi

        print " > Мониторинг изменений \"$DIR_DST\"...\n"

        if [[ -n $TIMEOUT ]]; then
            db_monitor_nonblock $TIMEOUT "/$DIR_DST"
        else
            db_monitor 60 "/$DIR_DST"
        fi

    ;;

    unlink)

        db_unlink

    ;;

    *)

        if [[ $COMMAND != "" ]]; then
            print "Ошибка: Неизвестная команда: $COMMAND\n\n"
            ERROR_STATUS=1
        fi
        usage

    ;;

esac

remove_temp_files

if [[ $ERROR_STATUS -ne 0 ]]; then
    echo "Произошла какая-то ошибка. Пожалуйста, проверьте журнал."
fi

exit $ERROR_STATUS
