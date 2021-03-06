
//                             08.11.2009
#ifndef __Language__
#define __Language__

#include "from_hdd_lg_to_pc.h"

//============================ hdd_lg_language =================================
   LANG Lan_RU[] =
{
  {  0, "Программа уже запущена."},
  {  1, "Вы настаивайте на запуске еще одной копии?"},
  {  2, "Ошибка при создании фоновой кисти."},
  {  3, "Данная программа не работает в операционных системах Windows 95/98/ME."},
  {  4, "Копировать"},
  {  5, "Настройка"},
  {  6, "Выход"},
  {  7, "Ошибка при создании шрифта."},
  {  8, "Вы уверены, что хотите прервать процесс?"},
  {  9, "Ошибка при чтении сектора."},
  { 10, "Ошибка при чтении кластера."},
  { 11, "Обнаружено несоответствие FAT и указателя на кластер файла."},
  { 12, "Неожиданно найден признак конца кластеров."},
  { 13, "Номер кластера превышает допустимое значение."},
  { 14, "Нужное число кластеров прочитано, а признак конца не найден."},
  { 15, "Неизвестный тип записи в строке каталога."},
  { 16, "Строка каталога, опознанная как папка имеет размерность в байтах."},
  { 17, "Размер одного каталога превышает размер кластера."},
  { 18, "Обнаружено несоответствие FAT и указателя на кластер каталога."},
  { 19, "Найден жесткий диск только частично похожий на то, что он из рекордера LG."},
  { 20, "Не найден жесткий диск из рекордера LG."},
  { 21, "Чтение служебной информации"},
  { 22, "Ошибка при чтении FAT."},
  { 23, "Неожиданное начало FAT."},
  { 24, "Копирование файла"},
  { 25, "Данная программа не работает, если у текущего пользователя нет прав администратора."},
  { 26, "Ошибка при проверке уровня доступа"},
  { 27, ""},
  { 28, ""},
  { 29, "Выходной файл заданного размера записан, а признак конца не найден."},
  { 30, "Уровень вложенности папок и файлов превышает возможности программы."},
  { 31, "Суммарное число папок и файлов превышает возможности программы."},
  { 32, "Нет имени выбранного для копирования."},
  { 33, "Ошибка при запросе информации об элементе дерева."},
  { 34, ""},
  { 35, "Ошибка"},
  { 36, "Внимание!"},
  { 37, "Системное сообщение"},
  { 38, "Попытка получить память нулевой длины."},
  { 39, "Нет свободной памяти для работы."},
  { 40, "Ошибка освобождения памяти."},
  { 41, "Ошибка при позиционировании по диску."},
  { 42, "Укажите имя записываемого файла"},
  { 43, "Файлы"},
  { 44, "Все файлы"},
  { 45, "Да"},
  { 46, "Системная ошибка при запросе емкости диска."},
  { 47, "байт"},
  { 48, "Доступно:"},
  { 49, "Требуется:"},
  { 50, "На диске указанном для записи недостаточно свободного места."},
  { 51, "Нет"},
  { 52, "Хотите указать другой путь?"},
  { 53, "Для записи указан диск с файловой системой FAT32"},
  { 54, "максимальный размер файла в которой 4ГБ - 1"},
  { 55, "Хотите указать другой путь?"},
  { 56, "Ошибка при удалении файла"},
  { 57, "Идет поиск жесткого диска из рекордера LG,"},
  { 58, "чтение служебной информации"},
  { 59, "и создание дерева папок и файлов."},
  { 60, "Это может продолжаться несколько минут."},
  { 61, "Пожалуйста подождите."},
  { 62, "Прервать"},
  { 63, "До окончания:"},
  { 64, "Время работы:"},
  { 65, "Файл с таким именем уже существует. Что делать?"},
  { 66, "Заменить файл"},
  { 67, "Записать c другим именем"},
  { 68, "Отменить копирование"},
  { 69, "Папка MEDIA"},
  { 70, "не показывать файлы *.idx"},
  { 71, "переименовывать *.str в *.vro"},
  { 72, "переставлять номер части в конец имени"},
  { 73, "Выбор языка"},
  { 74, "Сохранить"},
  { 75, "Закрыть"},
  { 76, "Замена символа '0' на '_' в номере части"},
  { 77, "Не показывать номер для одной части"},
  { 78, "Сортировка"},
  { 79, "По имени"},
  { 80, "Без сортировки"},
  { 81, "Неопознанная ошибка работы со списком."},
  { 82, "Ошибка при открытии файла для записи."},
  { 83, "Ошибка при записи файла."},
  { 84, "Ошибка при открытии файла для чтения."},
  { 85, "Ошибка при запросе размера файла."},
  { 86, "Недопустимо малый размер файла."},
  { 87, "Ошибка при чтении файла."},
  { 88, "Неверный идентификатор файла."},
  { 89, "Недопустимый индекс строки."},
  { 90, "Не найден текст ограниченный кавычками."},
  { 91, "Копирование папки"},
  { 92, "В указанной папке нет файлов для копирования."},
  { 93, "Системная ошибка при создании папки."},
  { 94, "Укажите диск или папку для записи"},
  { 95, "Число символов в полном имени файла превышает предел для  Windows."},
  { 96, "Нарушена структура каталога."},
  { 97, "Найдены ошибки в структуре записей на диске LG."},
  { 98, "Дерево папок и файлов содержит неполную информацию."},
  { 99, "Недопустимое число папок верхнего уровня."},
  {100, "Недопустимый номер кластера каталога."},
  {101, "На жестком диске не обнаружено ни одной записи."},
  {102, "Структура записей на жестком диске неизвестна автору программы."},
  {103, "Если Вы хотите, чтобы программа работала с жестким диском Вашего рекордера создайте дамп служебной"},
  {104, "информации (программа dump_from_hdd_lg_to_pc) и отошлите его автору на e-mail: val238@mail.ru"},
  {105, "Для выхода из программы прервите процесс копирования."},
  {106, "Число записей в файле MME.DB превысило возможности программы."},
  {107, "Тип рекордера не идентифицирован."},
  {108, "Структура файла MME.DB незнакома автору программы."},
  {109, "Вариант имен в папке MEDIA"},
  {110, "Информативное имя"},
  {111, "Истинное имя"},
  {112, "Тип рекордера"},
  {113, "Cерия RH200"},
  {114, "Другая серия"},
  {115, "Размер файлов"},
  {116, "Байты"},
  {117, "КБайты"},
  {118, "МБайты"},
  {119, "ГБайты"},
  {120, "Показывать расширение *.vro"},
  {121, "Выбрано файлов:"},
  {122, "Копирование выбранных файлов"},
  {123, "Очистить"},
  {124, "Емкость"},
  {125, "Занято"},
  {126, "Свободно"},
  {127, "Дата и время"},
  {128, "Начала записи"},
  {129, "Создания файла"},
  {130, "Качество записи"},
  {131, "Источник записи"},
  {132, "В колонке"},
  {133, "В имени"},
  {134, "Не показывать"},
  {135, "Недопустимый номер сектора начала FAT2."},
  {136, "Число кластеров в одном блоке"},
  {137, "Ошибки FAT"},
  {138, "В структуре FAT обнаружены ошибки."},
  {139, "Файлы с ошибками в FAT выделены иконкой 'e2'."},
  {140, "Размер таких файлов установлен равным нулю."},
  {141, "Обнаружена ошибка размера файла в каталоге и в базе записей."},
  {142, "Файлы с такими ошибками выделены иконкой 'e1'."},
  {143, "Размер таких файлов установлен равным размеру указанному в каталоге."},
  {144, "В файле MME.DB найдены файлы отсутствующие в папке MEDIA."},
  {145, "Такие файлы выделены иконкой 'e3'."},
  {146, "Запись txt"},
  {147, "Выбор папок"},
  {148, "Отмечено начало"},
  {149, "Второе имя группового выбора должно"},
  {150, "находиться в той же папке, что и первое."},
  {151, "Ошибка при изменении информации об элементе дерева."},
  {152, "Размер таких файлов установлен равным безошибочной части FAT."},
  {153, "отличаться от первого."},
  {154, "Показывать папками отредактированные Titles"},
  {155, "Каталог диска LG записан в файл:"},
  {156, "Существующий файл:"},
  {157, "Новый файл:"},
  {158, "Заменять все"},
  {159, "Заменять более старые"},
  {160, "Пропустить"},
  {161, "Пропускать все"},
  {162, "Переименовывать авто."},
  {163, "Переименовать"},
  {164, "Заменить"},
#if defined WRITE_YES                                        //Режим записи разрешен
  {165, "Функции записи"},
  {166, "Включить функции записи"},
  {167, "ВНИМАНИЕ! Функция записи потенциально опасная операция."},
  {168, "Автор программы не имел возможности протестировать функцию на всех моделях рекордеров LG."},
  {169, "Включая эту функцию, Вы должны понимать, что всегда есть риск повредить или полностью"},
  {170, "потерять записи на HDD Вашего рекордера."},
  {171, "Вы хотите включить функцию записи?"},
  {172, "Изменения с функцией записи вступят в силу после перезапуска программы."},
  {173, "Должна быть выбрана папка назначения для копирования."},
  {174, "Укажите файлы для копирования на HDD рекордера"},
  {175, "Ошибка при записи кластера."},
  {176, "Ошибка при записи FAT."},
  {177, "Запись файла"},
  {178, "Запись папки"},
  {179, "Новая папка"},
  {180, "Для работы можно выбирать только папки: JUKEBOX, PHOTO, VIDEO или внутренние в них."},
  {181, "В каталоге нет места для новой записи."},
  {182, "Число файлов для копирования превысило возможности программы."},
  {183, "Введите имя"},
  {184, "Недопустимое имя"},
  {185, "(не более 38 символов)"},
  {186, "Укажите папку для копирования на HDD рекордера"},
  {187, "Удалить"},
  {188, "Указанное имя не найдено в каталоге."},
  {189, "Нельзя изменять имена стандартных папок."},
  {190, "Запись невозможна, на HDD LG недостаточно свободного места."},
  {191, "Вы уверены, что хотите удалить это имя?"},
  {192, "Некоторые имена файлов и папок были ограничены до 38 символов."},
  {193, "Коррекция"},
  {194, "Нет необходимости в исправлении времени доступного для записи."},
  {195, "Время доступное для записи исправлено."},
#endif
};

//------------------------------------------------------------------------------

   LANG Lan_EN[] =
{
  {  0, "The program is already started."},
  {  1, "You wish to start one more copy?"},
  {  2, "Error at creation of a background brush."},
  {  3, "The program does not work in operational systems Windows 95/98/ME."},
  {  4, "Copy"},
  {  5, "Options"},
  {  6, "Quit"},
  {  7, "Error at creation of a font."},
  {  8, "You are assured, what wish to interrupt process?"},
  {  9, "Error at reading of sector."},
  { 10, "Error at reading of cluster."},
  { 11, "Values FAT mismatches references to cluster of the file."},
  { 12, "The attribute of the end of clusters is unexpectedly found."},
  { 13, "Number of cluster exceeds admissible value."},
  { 14, "The necessary number of clusters is read through, but the attribute of the end is not found."},
  { 15, "Unknown type of record in the directory."},
  { 16, "Record in the directory having type as the folder has size in byte."},
  { 17, "The size of one directory exceeds the size of cluster."},
  { 18, "Values FAT mismatches references to cluster of the directory."},
  { 19, "Hard disk from recorder LG is found. But its service information is unknown."},
  { 20, "Hard disk from recorder LG not found."},
  { 21, "Reading of the service information"},
  { 22, "Error at reading FAT."},
  { 23, "Unexpected beginning FAT."},
  { 24, "Copying of a file"},
  { 25, "The program does not work if the current user does not have administrative privileges."},
  { 26, "Error at check of administrative privileges"},
  { 27, ""},
  { 28, ""},
  { 29, "The target file of the set size is written down, but the attribute of the end is not found."},
  { 30, "The level of an enclosure of folders and files exceeds opportunities of the program."},
  { 31, "The total number of folders and files exceeds opportunities of the program."},
  { 32, "The file or folder for copying is not chosen."},
  { 33, "Error at loading the information on an element of a tree."},
  { 34, ""},
  { 35, "Error"},
  { 36, "Attention!"},
  { 37, "The system message"},
  { 38, "Attempt to receive memory of zero length."},
  { 39, "There is no free memory for work."},
  { 40, "Error of clearing of memory."},
  { 41, "Error at positioning on a hard disk LG."},
  { 42, "Specify a name of a written down file"},
  { 43, "Files"},
  { 44, "All files"},
  { 45, "Yes"},
  { 46, "System error at loading capacity of a hard disk."},
  { 47, "bytes"},
  { 48, "Available:"},
  { 49, "Required:"},
  { 50, "On a disk specified for record not enough free space."},
  { 51, "No"},
  { 52, "Wish to specify other path name?"},
  { 53, "For record the partition with file system FAT32 is chosen."},
  { 54, "The maximal size of a file in FAT32 4 Gbyte - 1 byte"},
  { 55, "Wish to specify other path name?"},
  { 56, "Error at removal of a file"},
  { 57, "There is a search of a hard disk from recorder LG,"},
  { 58, "reading of the service information and creation"},
  { 59, "of a tree of  folders and files."},
  { 60, "It can proceed some minutes."},
  { 61, "Please wait."},
  { 62, "Abort"},
  { 63, "Remaining time:"},
  { 64, "Total time:"},
  { 65, "The file with such name already exists. What to do?"},
  { 66, "Replace a file"},
  { 67, "To write down with other name"},
  { 68, "Cancel copying"},
  { 69, "Folder MEDIA"},
  { 70, "not show files *.idx"},
  { 71, "rename *.str in *.vro"},
  { 72, "transfer number of a part to the end of a name"},
  { 73, "Change of language"},
  { 74, "Save"},
  { 75, "Cancel"},
  { 76, "replacement of a character '0' on character '_'"},
  { 77, "not show number for one part"},
  { 78, "Sorting"},
  { 79, "By name"},
  { 80, "Unsorted"},
  { 81, "Unknown error of work with the list."},
  { 82, "Error at opening a file for record."},
  { 83, "Error at record of a file."},
  { 84, "Error at opening a file for reading."},
  { 85, "Error at loading the size of a file."},
  { 86, "The file has too small size"},
  { 87, "Error at reading a file."},
  { 88, "The incorrect identifier of a file."},
  { 89, "The incorrect identifier of a line."},
  { 90, "The text limited by double inverted commas is not found."},
  { 91, "Copying of a folder"},
  { 92, "In the specified folder there are no files for copying."},
  { 93, "System error at creation of a folder."},
  { 94, "Will choose a disk or a folder for recording"},
  { 95, "The number of symbols in a full name of a file exceeds a limit for windows."},
  { 96, "The structure of the directory is broken."},
  { 97, "Errors in structure of recordings on disk LG are found."},
  { 98, "The tree of folders and files contains the incomplete information."},
  { 99, "Invalid number of folders of the top level."},
  {100, "Invalid number of a cluster of the directory."},
  {101, "On a hard disk there is no recording."},
  {102, "The structure of recordings on a hard disk is unknown to the author of the program."},
  {103, "If you want, that the program worked with a hard disk of yours recorder create dump the service"},
  {104, "information (the program dump_from_hdd_lg_to_pc) and send its author on e-mail: val238@mail.ru"},
  {105, "For an exit from the program interrupt process of copying."},
  {106, "The number of records in file MME.DB has exceeded opportunities of the program."},
  {107, "The type recorder is not identified."},
  {108, "The structure of file MME.DB is unfamiliar to the author of the program."},
  {109, "Variant of names in folder MEDIA"},
  {110, "Informative name"},
  {111, "True name"},
  {112, "Type of the recorder"},
  {113, "Series RH200"},
  {114, "Other series"},
  {115, "The size of files"},
  {116, "Bytes"},
  {117, "Kbytes"},
  {118, "Mbytes"},
  {119, "Gbytes"},
  {120, "To show extension *.vro"},
  {121, "Chosen files:"},
  {122, "Copying of the chosen files"},
  {123, "Clear"},
  {124, "Capacity"},
  {125, "Used space"},
  {126, "Free space"},
  {127, "Date and time"},
  {128, "Beginnings of record"},
  {129, "Creations of a file"},
  {130, "Quality of record"},
  {131, "Source of record"},
  {132, "In a column"},
  {133, "In a name"},
  {134, "Not show"},
  {135, "Invalid number of a sector of the FAT2 start."},
  {136, "Number of clusters in one block"},
  {137, "Errors FAT"},
  {138, "Incorrect records are found in the FAT."},
  {139, "Files with errors in FAT are noted by an icon 'e2'."},
  {140, "The size of such files is equal to zero."},
  {141, "The error of the size of a file in the directory and in base of records is found out."},
  {142, "Files with such errors are noted by an icon 'e1'."},
  {143, "The size of such files is equal to the size in the directory."},
  {144, "Files are found in file MME.DB absent in folder MEDIA."},
  {145, "Such files are noted by an icon 'e3'."},
  {146, "Write txt"},
  {147, "Choice of folders"},
  {148, "The beginning is noted"},
  {149, "The Second name of a group choice should"},
  {150, "to be in the same folder, as the first."},
  {151, "Error at change of the information on a tree element."},
  {152, "The Size of such files is established equal to faultless part FAT."},
  {153, "to differ from the first."},
  {154, "To show folders the edited Titles"},
  {155, "Catalogue of disk LG is written down in a file:"},
  {156, "Existing file:"},
  {157, "New file:"},
  {158, "Overwrite all"},
  {159, "Overwrite all older"},
  {160, "Skip"},
  {161, "Skip all"},
  {162, "Rename auto"},
  {163, "Rename"},
  {164, "Overwrite"},
#if defined WRITE_YES                                        //Режим записи разрешен
  {165, "Options write"},
  {166, "Enable write"},
  {167, "WARNING: The function of recording a potentially dangerous operation."},
  {168, "The author of the program had no possibility to test function on all models of recorders LG."},
  {169, "To enable this function, you should understand, that always there is a risk to damage or completely"},
  {170, "to lose records on HDD your recorder."},
  {171, "Do you want to enable the record?"},
  {172, "Changes to the function of recording will take effect when you restart the program."},
  {173, "It must be chosen destination folder to copy."},
  {174, "Give the files to be copied to the HDD recorder"},
  {175, "Error writing cluster."},
  {176, "Error writing FAT."},
  {177, "Write file"},
  {178, "Write folder"},
  {179, "New folder"},
  {180, "For work, you can choose only the folders: JUKEBOX, PHOTO, VIDEO, or internal to them."},
  {181, "The directory is no room for the new record."},
  {182, "The number of files to copy more than its capabilities."},
  {183, "Enter the name"},
  {184, "Invalid name"},
  {185, "(no more than 38 characters)"},
  {186, "Give the folder to copy to the HDD recorder"},
  {187, "Delete"},
  {188, "This name is not found in the directory."},
  {189, "You can not change the names of standard folders."},
  {190, "Can not record on the HDD LG is not enough free space."},
  {191, "Are you sure you want to delete this name?"},
  {192, "Some file names and folders were limited to 38 characters."},
  {193, "Correction"},
  {194, "No need to correct the time available for recording."},
  {195, "Fixed time available for recording."},
#endif
};

//------------------------------------------------------------------------------

   const int numStrLan = sizeof(Lan_RU) / sizeof(LANG);      //Число языковых строк
   LANG Lan_Any[numStrLan];                                  //Произвольный язык

static_assert(sizeof(Lan_RU) / sizeof(LANG) == sizeof(Lan_EN) / sizeof(LANG), "ERROR sizeof(Lan_RU) != sizeof(Lan_EN)");

#endif

