#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "sha256.h"
#include "result.h"

#define MAX_PASS_LENGTH 32
#define DB_INI_SCRIPT_PATH "assets/db_ini.sql"

/*
    initialize blog via asking input for pass key and db

 */
void initialize_blog();
