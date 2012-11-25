#include "mydef.h"
#include "mldefine.h"

#pragma once

void setMLdefine(MLdefine *mdf);
void initmlAI3(const char *fname);
Hand mlAI3(const fieldInfo &info,Cards myCards,Cards oppCards);
