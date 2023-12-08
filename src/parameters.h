#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <petsc.h>
#include "hybrid.h"

extern PetscErrorCode ProcessOptions(CLI *cli);
extern PetscErrorCode EchoOptions(Context ctx);

#endif // PARAMETERS_H
