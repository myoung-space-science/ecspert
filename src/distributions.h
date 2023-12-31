#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <petsc.h>
#include "hybrid.h"

// Type to be used for particle distribution functions in Rejection.
typedef PetscErrorCode (*DistributionFunction)(PetscReal x, PetscReal y, PetscReal z, PetscReal *v, Context *ctx);

// Top-level distribution functions.
extern PetscErrorCode UniformDistribution(Context *ctx);
extern PetscErrorCode UniformDistributionFromCoordinates(Context *ctx);
extern PetscErrorCode UniformDistributionCellCentered(Context *ctx);
extern PetscErrorCode SobolDistribution(Context *ctx);
extern PetscErrorCode Rejection(DistributionFunction density, Context *ctx);

// DistributionFunction implementations.
extern PetscErrorCode SinusoidalDistribution(PetscReal x, PetscReal y, PetscReal z, PetscReal *v, Context *ctx);

#endif // DISTRIBUTIONS_H