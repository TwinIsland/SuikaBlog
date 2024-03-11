#ifndef SUIKA_STATE_H
#define SUIKA_STATE_H

typedef struct
{
    int is_db_first_initialize; // the flag that indicate if the database just created
} SUIKA_STATE;

extern SUIKA_STATE SYSTEM_STATE;

#endif // SUIKA_STATE_H