/*$Id$*/
#include "stralloc.h"
#include "scan.h"
#include "fmt.h"
#include "strerr.h"
#include "cookie.h"
#include "slurp.h"
#include "errtxt.h"
#include "subscribe.h"
#include "makehash.h"
#include "die.h"
#include "idx.h"

static stralloc key = {0};
static char hash[COOKIE];
static char strnum[FMT_ULONG];	/* message number as sz */

void tagmsg(const char *dir,		/* db base dir */
	    unsigned long msgnum,	/* number of this message */
	    const char *seed,		/* seed. NULL ok, but less entropy */
	    const char *action,	/* to make it certain the cookie differs from*/
				/* one used for a digest */
	    char *hashout,		/* calculated hash goes here */
	    unsigned long bodysize,
	    unsigned long chunk)
/* This routine creates a cookie from num,seed and the */
/* list key and returns that cookie in hashout. The use of sender/num and */
/* first char of action is used to make cookie differ between messages,   */
/* the key is the secret list key. */
{
  unsigned int i;

  strnum[fmt_ulong(strnum,msgnum)] = '\0';	/* message nr ->string*/

    switch(slurp("key",&key,32)) {
      case -1:
	strerr_die3sys(111,FATAL,ERR_READ,"key: ");
      case 0:
	strerr_die3x(100,FATAL,"key",ERR_NOEXIST);
    }
    cookie(hash,key.s,key.len,strnum,seed,action);
    for (i = 0; i < COOKIE; i++)
      hashout[i] = hash[i];

    return;
}
