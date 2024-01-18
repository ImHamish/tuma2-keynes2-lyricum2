#include "stdafx.h"

#include <mysql/mysql_version.h>

void WriteVersion()
{
    FILE* fp(fopen("VERSION.txt", "w"));

    if (fp)
    {
        fprintf(fp, "__TIMESTAMP__: %s\n", __TIMESTAMP__);
        fprintf(fp, "__ARCHITECTURE__: %s\n", __ARCHITECTURE__);
        fprintf(fp, "__TOOLSET__: %s\n", __TOOLSET__);
        fprintf(fp, "__BUILD_TYPE__: %s\n", __BUILD_TYPE__);
        fprintf(fp, "__HOSTNAME__: %s\n", __HOSTNAME__);
        fprintf(fp, "__TARGET_OS__: %s\n", __TARGET_OS__);
        fprintf(fp, "__PREMAKE_VERSION__: %s\n", __PREMAKE_VERSION__);
        fprintf(fp, "__WORKING_DIR__: %s\n", __WORKING_DIR__);
        fprintf(fp, "__MYSQL_SERVER_VERSION__: %s\n", MYSQL_SERVER_VERSION);
        fclose(fp);
    }
    else
    {
        fprintf(stderr, "cannot open VERSION.txt\n");
        exit(0);
    }
}
