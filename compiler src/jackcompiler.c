#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compeng.h"

static char filesOfFolder[100][100]; // Jack files of an entered folder are stored here.
                                     // 10 jack files each can contain 100 characters at most.
static int fileCount = 0;

#ifdef _WIN32
#include <windows.h>
void search_directory(const char *path, const char *extension)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char searchPath[MAX_PATH];

    snprintf(searchPath, sizeof(searchPath), "%s\\*%s", path, extension);

    hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("No files found in directory: %s\n", path);
        return;
    }

    char fullFileName[256] = "";
    do
    {
        sprintf(fullFileName, "%s\\%s", path, findFileData.cFileName);
        strcpy(filesOfFolder[fileCount], fullFileName);
        ++fileCount;

    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}
#elif defined(__linux__) || defined(__APPLE__)
#include <dirent.h>
void search_directory(const char *path, const char *extension)
{
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL)
    {
        perror("Unable to open directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {                                                       // Only regular files
            const char *file_ext = strrchr(entry->d_name, '.'); // Get file extension
            if (file_ext && strcmp(file_ext, extension) == 0)
            {
                printf("Found file: %s/%s\n", path, entry->d_name);
                strcpy(filesOfFolder[fileCount], fullFileName);
                ++fileCount;
            }
        }
    }

    closedir(dir);
}
#else
#error "Unsupported OS"
#endif

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <source file/files>/<source folder>\n", argv[0]);
        return 1;
    }

    const char fileFormat[] = ".jack";
    char *pch = strstr(argv[1], fileFormat);

    // If the second argument is a .jack file name
    if (pch != NULL)
    {
        for(int i = 1; i < argc; i++)
            initCompilation(argv[i]);
    }
    
    // else, it is a folder name
    else
    {
        search_directory(argv[1], fileFormat);

        for (int i = 0; i < fileCount; i++)
            initCompilation(filesOfFolder[i]);
    }

    return 0;
}