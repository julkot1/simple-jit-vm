#pragma once
#include "main.h"

void env_init(program pr);

void store_const(pool_element var);
pool_element *load_const(long id);

void store_var(pool_element var);
pool_element *load_var(long id);
