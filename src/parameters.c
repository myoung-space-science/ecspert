#include <petsc.h>
#include "hybrid.h"
#include "constants.h"
#include "lhs.h"
#include "rhs.h"

/* Supported RHS functions. */
const char *RHSTypes[] = {
  "constant", "sinusoidal", "full", "RHSType", "RHS_", NULL
};

/* Supported LHS functions. */
const char *LHSTypes[] = {
  "identity", "laplacian", "full", "LHSType", "LHS_", NULL
};

/* Supported boundary conditions. */
const char *BoundaryTypes[] = {
  "periodic", "injection", "reflection", "dirichlet", "neumann", "BoundaryType", "BT_", NULL
};


/* Read runtime arguments from the command line or a file. */
PetscErrorCode ProcessOptions(CLI *cli)
{
  PetscInt  intArg;
  PetscReal realArg;
  PetscEnum enumArg;
  PetscBool boolArg;
  PetscBool found;

  PetscFunctionBeginUser;
  ECHO_FUNCTION_ENTER;

  // Read optional parameter values from user input.
  PetscCall(PetscOptionsGetBool(NULL, NULL, "--version", &boolArg, &found));
  if (found) {
    PetscCall(PetscPrintf(PETSC_COMM_WORLD, "%s\n", VERSION));
    PetscCall(PetscEnd());
  }
  PetscCall(PetscOptionsGetEnum(NULL, NULL, "--rhs-type", RHSTypes, &enumArg, &found));
  if (found) {
    cli->rhsType = enumArg;
  } else {
    cli->rhsType = RHS_FULL;
  }
  PetscCall(PetscOptionsGetEnum(NULL, NULL, "--lhs-type", LHSTypes, &enumArg, &found));
  if (found) {
    cli->lhsType = enumArg;
  } else {
    cli->lhsType = LHS_FULL;
  }
  PetscCall(PetscOptionsGetInt(NULL, NULL, "-Nx", &intArg, &found));
  if (found) {
    if (intArg < 0) {
      PRINT_WORLD("Warning: Ignoring negative value for Nx: %d\n", intArg);
    } else {
      cli->Nx = intArg;
    }
  } else {
    cli->Nx = -1;
  }
  PetscCall(PetscOptionsGetInt(NULL, NULL, "-Ny", &intArg, &found));
  if (found) {
    if (intArg < 0) {
      PRINT_WORLD("Warning: Ignoring negative value for Ny: %d\n", intArg);
    } else {
      cli->Ny = intArg;
    }
  } else {
    cli->Ny = -1;
  }
  PetscCall(PetscOptionsGetInt(NULL, NULL, "-Nz", &intArg, &found));
  if (found) {
    if (intArg < 0) {
      PRINT_WORLD("Warning: Ignoring negative value for Nz: %d\n", intArg);
    } else {
      cli->Nz = intArg;
    }
  } else {
    cli->Nz = -1;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-dx", &realArg, &found));
  if (found) {
    cli->dx = realArg;
  } else {
    cli->dx = -1.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-dy", &realArg, &found));
  if (found) {
    cli->dy = realArg;
  } else {
    cli->dy = -1.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-dz", &realArg, &found));
  if (found) {
    cli->dz = realArg;
  } else {
    cli->dz = -1.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-x0", &realArg, &found));
  if (found) {
    cli->x0 = realArg;
  } else {
    cli->x0 = 0.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-y0", &realArg, &found));
  if (found) {
    cli->y0 = realArg;
  } else {
    cli->y0 = 0.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-z0", &realArg, &found));
  if (found) {
    cli->z0 = realArg;
  } else {
    cli->z0 = 0.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-x1", &realArg, &found));
  if (found) {
    cli->x1 = realArg;
  } else {
    cli->x1 = 1.0;
  }
  if (cli->x1 == cli->x0) {
      PRINT_WORLD("Warning: zero-width x dimension\n");
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-y1", &realArg, &found));
  if (found) {
    cli->y1 = realArg;
  } else {
    cli->y1 = 1.0;
  }
  if (cli->y1 == cli->y0) {
      PRINT_WORLD("Warning: zero-width y dimension\n");
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-z1", &realArg, &found));
  if (found) {
    cli->z1 = realArg;
  } else {
    cli->z1 = 1.0;
  }
  if (cli->z1 == cli->z0) {
      PRINT_WORLD("Warning: zero-width z dimension\n");
  }
  PetscCall(PetscOptionsGetEnum(NULL, NULL, "--x0-bc", BoundaryTypes, &enumArg, &found));
  if (found) {
    cli->xBT[0] = enumArg;
  } else {
    cli->xBT[0] = BT_PERIODIC;
  }
  PetscCall(PetscOptionsGetEnum(NULL, NULL, "--x1-bc", BoundaryTypes, &enumArg, &found));
  if (found) {
    cli->xBT[1] = enumArg;
  } else {
    cli->xBT[1] = BT_PERIODIC;
  }
  PetscCall(PetscOptionsGetEnum(NULL, NULL, "--y0-bc", BoundaryTypes, &enumArg, &found));
  if (found) {
    cli->yBT[0] = enumArg;
  } else {
    cli->yBT[0] = BT_PERIODIC;
  }
  PetscCall(PetscOptionsGetEnum(NULL, NULL, "--y1-bc", BoundaryTypes, &enumArg, &found));
  if (found) {
    cli->yBT[1] = enumArg;
  } else {
    cli->yBT[1] = BT_PERIODIC;
  }
  PetscCall(PetscOptionsGetEnum(NULL, NULL, "--z0-bc", BoundaryTypes, &enumArg, &found));
  if (found) {
    cli->zBT[0] = enumArg;
  } else {
    cli->zBT[0] = BT_PERIODIC;
  }
  PetscCall(PetscOptionsGetEnum(NULL, NULL, "--z1-bc", BoundaryTypes, &enumArg, &found));
  if (found) {
    cli->zBT[1] = enumArg;
  } else {
    cli->zBT[1] = BT_PERIODIC;
  }
  PetscCall(PetscOptionsGetInt(NULL, NULL, "-Np", &intArg, &found));
  if (found) {
    cli->Np = intArg;
  } else {
    cli->Np = -1;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-n0", &realArg, &found));
  if (found) {
    cli->n0 = realArg;
  } else {
    cli->n0 = 1.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-qi", &realArg, &found));
  if (found) {
    cli->qi = realArg;
  } else {
    cli->qi = Q;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-mi", &realArg, &found));
  if (found) {
    cli->mi = realArg;
  } else {
    cli->mi = MP;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-nue", &realArg, &found));
  if (found) {
    cli->nue = realArg;
  } else {
    cli->nue = 1.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-nui", &realArg, &found));
  if (found) {
    cli->nui = realArg;
  } else {
    cli->nui = 1.0;
  }
  // The user may provide a single electron drift speed for all components, as
  // well as a drift speed for each component. Any component value not
  // explicitly set will have the common value, which defaults to 0.
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-ve0", &realArg, &found));
  if (found) {
    cli->ve0x = realArg;
    cli->ve0y = realArg;
    cli->ve0z = realArg;
  } else {
    cli->ve0x = 0.0;
    cli->ve0y = 0.0;
    cli->ve0z = 0.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-ve0x", &realArg, &found));
  if (found) {
    cli->ve0x = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-ve0y", &realArg, &found));
  if (found) {
    cli->ve0y = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-ve0z", &realArg, &found));
  if (found) {
    cli->ve0z = realArg;
  }
  // The user may provide a single ion drift speed for all components, as well
  // as a drift speed for each component. Any component value not explicitly set
  // will have the common value, which defaults to 0.
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-vi0", &realArg, &found));
  if (found) {
    cli->vi0x = realArg;
    cli->vi0y = realArg;
    cli->vi0z = realArg;
  } else {
    cli->vi0x = 0.0;
    cli->vi0y = 0.0;
    cli->vi0z = 0.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-vi0x", &realArg, &found));
  if (found) {
    cli->vi0x = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-vi0y", &realArg, &found));
  if (found) {
    cli->vi0y = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-vi0z", &realArg, &found));
  if (found) {
    cli->vi0z = realArg;
  }
  // The user may provide a single electron thermal speed for all components, as
  // well as a thermal speed for each component. Any component value not
  // explicitly set will have the common value, which defaults to 0.
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-veT", &realArg, &found));
  if (found) {
    cli->veTx = realArg;
    cli->veTy = realArg;
    cli->veTz = realArg;
  } else {
    cli->veTx = 0.0;
    cli->veTy = 0.0;
    cli->veTz = 0.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-veTx", &realArg, &found));
  if (found) {
    cli->veTx = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-veTy", &realArg, &found));
  if (found) {
    cli->veTy = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-veTz", &realArg, &found));
  if (found) {
    cli->veTz = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-Te", &realArg, &found));
  if (found) {
    cli->Te = realArg;
  } else {
    cli->Te = 0.0;
  }
  // The user may provide a single ion thermal speed for all components, as well
  // as a thermal speed for each component. Any component value not explicitly
  // set will have the common value, which defaults to 0.
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-viT", &realArg, &found));
  if (found) {
    cli->viTx = realArg;
    cli->viTy = realArg;
    cli->viTz = realArg;
  } else {
    cli->viTx = 0.0;
    cli->viTy = 0.0;
    cli->viTz = 0.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-viTx", &realArg, &found));
  if (found) {
    cli->viTx = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-viTy", &realArg, &found));
  if (found) {
    cli->viTy = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-viTz", &realArg, &found));
  if (found) {
    cli->viTz = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-Ti", &realArg, &found));
  if (found) {
    cli->Ti = realArg;
  } else {
    cli->Ti = 0.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-mn", &realArg, &found));
  if (found) {
    cli->mn = realArg;
  } else {
    cli->mn = cli->mi;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-Tn", &realArg, &found));
  if (found) {
    cli->Tn = realArg;
  } else {
    cli->Tn = -1.0;
  }
  // The user may provide a single neutral-species drift speed for all
  // components, as well as a drift speed for each component. Any component
  // value not explicitly set will have the common value, which defaults to 0.
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-vn0", &realArg, &found));
  if (found) {
    cli->vn0x = realArg;
    cli->vn0y = realArg;
    cli->vn0z = realArg;
  } else {
    cli->vn0x = 0.0;
    cli->vn0y = 0.0;
    cli->vn0z = 0.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-vn0x", &realArg, &found));
  if (found) {
    cli->vn0x = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-vn0y", &realArg, &found));
  if (found) {
    cli->vn0y = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-vn0z", &realArg, &found));
  if (found) {
    cli->vn0z = realArg;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-B0", &realArg, &found));
  if (found) {
    cli->B0 = realArg;
  } else {
    cli->B0 = 0.0;
  }
  PetscCall(PetscOptionsGetReal(NULL, NULL, "-E0", &realArg, &found));
  if (found) {
    cli->E0 = realArg;
  } else {
    cli->E0 = 0.0;
  }

  ECHO_FUNCTION_EXIT;
  PetscFunctionReturn(PETSC_SUCCESS);
}


/* Print the value of each runtime parameter to a text file.

The format of output headers and footers is meant to mimic those of the PETSc
options output, in order to simplify parsing by analysis routines.
*/
PetscErrorCode EchoOptions(Context ctx)
{
  PetscViewer viewer;

  PetscFunctionBeginUser;
  ECHO_FUNCTION_ENTER;

  // Open the viewer in "write" mode. This will clobber existing contents.
  PetscCall(PetscViewerASCIIOpen(PETSC_COMM_WORLD, ctx.optionsLog, &viewer));

  // View the PETSc options database.
  PetscCall(PetscOptionsView(NULL, viewer));

  // View common parameter values.
  PetscCall(PetscViewerASCIIPrintf(viewer, "\n\n#Common Parameter Values\n"));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Nx = %d\n", ctx.grid.Nx));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Ny = %d\n", ctx.grid.Ny));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Nz = %d\n", ctx.grid.Nz));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "x0 = %f [m]\n", ctx.grid.x0));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "y0 = %f [m]\n", ctx.grid.y0));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "z0 = %f [m]\n", ctx.grid.z0));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "x1 = %f [m]\n", ctx.grid.x1));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "y1 = %f [m]\n", ctx.grid.y1));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "z1 = %f [m]\n", ctx.grid.z1));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Lx = %f [m]\n", ctx.grid.Lx));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Ly = %f [m]\n", ctx.grid.Ly));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Lz = %f [m]\n", ctx.grid.Lz));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "dx = %f [m]\n", ctx.grid.dx));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "dy = %f [m]\n", ctx.grid.dy));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "dz = %f [m]\n", ctx.grid.dz));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "xBC = %s\n", BCTypes[ctx.ions.xBC]));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "yBC = %s\n", BCTypes[ctx.ions.yBC]));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "zBC = %s\n", BCTypes[ctx.ions.zBC]));
  if (ctx.potential.stencilType == DMDA_STENCIL_BOX) {
    PetscCall(PetscViewerASCIIPrintf(viewer,   "stencil type = box\n"));
  } else if (ctx.potential.stencilType == DMDA_STENCIL_STAR) {
    PetscCall(PetscViewerASCIIPrintf(viewer,   "stencil type = star\n"));
  }
  PetscCall(PetscViewerASCIIPrintf(viewer,     "stencil size = %d\n", ctx.potential.stencilSize));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Np = %d\n", ctx.plasma.Np));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "B0 = %g [T]\n", ctx.plasma.B0));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "E0 = %g [N/C]\n", ctx.plasma.E0));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "qe = %e [C]\n", ctx.electrons.q));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "me = %e [kg]\n", ctx.electrons.m));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "nue = %g [s^-1]\n", ctx.electrons.nu));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "ve0x = %f [m/s]\n", ctx.electrons.v0x));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "ve0y = %f [m/s]\n", ctx.electrons.v0y));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "ve0z = %f [m/s]\n", ctx.electrons.v0z));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "veTx = %f [m/s]\n", ctx.electrons.vTx));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "veTy = %f [m/s]\n", ctx.electrons.vTy));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "veTz = %f [m/s]\n", ctx.electrons.vTz));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Te = %f [K]\n", ctx.electrons.T));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "gamma_e = %f\n", ctx.gammaT));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Omega_e = %g [s^-1]\n", ctx.electrons.Omega));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "kappa_e = %g\n", ctx.electrons.kappa));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "qi = %e [C]\n", ctx.ions.q));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "mi = %e [kg]\n", ctx.ions.m));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "nui = %g [s^-1]\n", ctx.ions.nu));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "vi0x = %f [m/s]\n", ctx.ions.v0x));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "vi0y = %f [m/s]\n", ctx.ions.v0y));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "vi0z = %f [m/s]\n", ctx.ions.v0z));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "viTx = %f [m/s]\n", ctx.ions.vTx));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "viTy = %f [m/s]\n", ctx.ions.vTy));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "viTz = %f [m/s]\n", ctx.ions.vTz));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Ti = %f [K]\n", ctx.ions.T));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Omega_i = %g [s^-1]\n", ctx.ions.Omega));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "kappa_i = %g\n", ctx.ions.kappa));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "mn = %e\n", ctx.neutrals.m));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "Tn = %f\n", ctx.neutrals.T));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "vnT = %f\n", ctx.neutrals.vT));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "vn0x = %f\n", ctx.neutrals.v0x));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "vn0y = %f\n", ctx.neutrals.v0y));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "vn0z = %f\n", ctx.neutrals.v0z));
  PetscCall(PetscViewerASCIIPrintf(viewer,     "#End of Common Parameter Values\n"));

  // Free memory.
  PetscCall(PetscViewerDestroy(&viewer));

  ECHO_FUNCTION_EXIT;
  PetscFunctionReturn(PETSC_SUCCESS);
}
