#define DEBUG 0
#define INTEGRITY_CHECK 1
extern int OUTPUT;
#define DEBUG_INFO(x) {if(DEBUG == 1){x}}

#ifdef DEBUG
#define PRINTD printf
#else
#define PRINTD(format, args...) ((void)0)
#endif

#define IN_DEBUG(x) {if(DEBUG){ x } }
#define IN_OUTPUT(x) {if(OUTPUT){ x } }
#define IN_DEBUG(x) {if(DEBUG){ x } }
#define IN_INTEGRITY_CHECK(x) {if(INTEGRITY_CHECK){ x } }

#include <stdbool.h>

void assert(bool b, char* message);

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
