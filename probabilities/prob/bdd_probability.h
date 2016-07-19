#include "bdd.h"

#include "kernel.h"
#include "gmp.h"

#include <math.h>
//#include <stdlib.h>
//#include <unistd.h>
#include <vector>
#include <map>
#include <list>
//#include <set>
#include <algorithm>

double bdd_get_probability(const bdd r, mpq_t *prob);
