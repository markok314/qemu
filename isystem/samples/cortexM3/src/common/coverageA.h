#ifndef COVERAGE_A__H
#define COVERAGE_A__H

/**
 * This function should be implemented in target specific project,
 * because assembler is not portable. Declared as C function, because
 * on MPC5554 this is assmebler function and it's name is not mangled.
 */
extern "C" {
  void CoverageA();
}

#endif  /* COVERAGE_A__H */
