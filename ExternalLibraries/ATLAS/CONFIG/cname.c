
#include <stdio.h>
void c_routine_(void)  { printf("-DAdd_\n"); }
void c_routine(void)   { printf("-DNoChange\n"); }
void C_ROUTINE(void)  { printf("-DUpCase\n"); }
void c_routine__(void) { printf("-DAdd__\n"); }
