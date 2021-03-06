/* env.c, envread.c, env.h: environ library
Daniel J. Bernstein, djb@silverton.berkeley.edu.
Depends on str.h, alloc.h.
Requires environ.
19960113: rewrite. warning: interface is different.
No known patent problems.
*/

#include "str.h"
#include "alloc.h"
#include "env.h"

int env_isinit = 0; /* if env_isinit: */
static int ea; /* environ is a pointer to ea+1 char*'s. */
static int en; /* the first en of those are ALLOCATED. environ[en] is 0. */

static void env_init(void);

static void env_goodbye(int i)
{
 alloc_free(environ[i]);
 environ[i] = environ[--en];
 environ[en] = 0;
}

static char *null = 0;

void env_clear(void)
{
 if (env_isinit) while (en) env_goodbye(0);
 else environ = &null;
}

static void env_unsetlen(const char *s,int len)
{
 int i;
 for (i = en - 1;i >= 0;--i)
   if (!str_diffn(s,environ[i],len))
     if (environ[i][len] == '=')
       env_goodbye(i);
}

void env_unset(const char *s)
{
 env_init();
 env_unsetlen(s,str_len(s));
}

static void env_add(char *s)
{
 const char *t;
 t = env_findeq(s);
 if (t) env_unsetlen(s,t - s);
 if (en == ea)
  {
   ea += 30;
   alloc_re((void**)environ,(en + 1) * sizeof(char *),(ea + 1) * sizeof(char *));
  }
 environ[en++] = s;
 environ[en] = 0;
}

void env_put(char *s)
{
 char *u;
 env_init();
 u = alloc(str_len(s) + 1);
 str_copy(u,s);
 env_add(u);
}

void env_put2(const char *s,const char *t)
{
 char *u;
 int slen;
 env_init();
 slen = str_len(s);
 u = alloc(slen + str_len(t) + 2);
 str_copy(u,s);
 u[slen] = '=';
 str_copy(u + slen + 1,t);
 env_add(u);
}

static void env_init(void)
{
 char **newenviron;
 if (env_isinit) return;
 for (en = 0;environ[en];++en) ;
 ea = en + 10;
 newenviron = (char **) alloc((ea + 1) * sizeof(char *));
 for (en = 0;environ[en];++en)
  {
   newenviron[en] = alloc(str_len(environ[en]) + 1);
   str_copy(newenviron[en],environ[en]);
  }
 newenviron[en] = 0;
 environ = newenviron;
 env_isinit = 1;
}
