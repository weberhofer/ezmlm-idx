/*$Id$*/

#include <unistd.h>
#include "strerr.h"
#include "env.h"
#include "subscribe.h"
#include "sgetopt.h"
#include "errtxt.h"
#include "idx.h"
#include "auto_version.h"

const char FATAL[] = "ezmlm-issubn: fatal: ";

void die_nomem(void) { strerr_die2x(111,FATAL,ERR_NOMEM); }

void die_usage(void)
{
  strerr_die1x(100,"ezmlm-issubn: usage: ezmlm-issubn [-nN] dir [dir1 ...]");
}

void die_sender(void)
{
  strerr_die2x(100,FATAL,ERR_NOSENDER);
}

void main(int argc,char **argv)
{
  char *dir;
  char *addr;
  int flagsub = 0;
  int opt;

  addr = env_get("SENDER");
  if (!addr) die_sender();	/* REQUIRE sender */

  while ((opt = getopt(argc,argv,"nNvV")) != opteof)
    switch(opt) {
      case 'n': flagsub = 99; break;
      case 'N': flagsub = 0; break;
      case 'v':
      case 'V': strerr_die2x(0, "ezmlm-issubn version: ",auto_version);
      default:
	die_usage();
    }

  dir = argv[optind];
  if (chdir(dir) == -1)
    strerr_die4sys(111,FATAL,ERR_SWITCH,dir,": ");

  while ((dir = argv[optind++])) {
    if (dir[0] != '/')
      strerr_die2x(100,FATAL,ERR_SLASH);
    if (issub(dir,addr,(char *) 0)) {
      closesql();
      _exit(flagsub);		/* subscriber */
    }
  }
  closesql();
  if (flagsub)			/* not subscriber anywhere */
    _exit(0);
  else
    _exit(99);
}
