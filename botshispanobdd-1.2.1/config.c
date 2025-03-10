/* Configuration file handling.
 *
 * Services is copyright (c) 1996-1999 Andy Church.
 *     E-mail: <achurch@dragonfire.net>
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 */

#include "services.h"

/*************************************************************************/

/* Configurable variables: */

char *RemoteServer;
int   RemotePort;
char *RemotePassword;
char *LocalHost;
int   LocalPort;

char *ServerName;
char *ServerDesc;
char *ServerHUB;
char *ServiceUser;
char *ServiceUserSpam;
char *ServiceHost;
char *ServiceHostSpam;
char *ServiceUserEuskalIRC;
char *ServiceHostEuskalIRC;
static char *temp_userhost;
static char *temp_userspam;
static char *temp_usereuskalirc;
#ifdef IRC_UNDERNET_P10
int  ServerNumerico;
#endif

char *s_NickServ;
char *s_ChanServ;
char *s_MemoServ;
char *s_HelpServ;
char *s_OperServ;
char *s_SpamServ;
char *s_GlobalNoticer;
char *s_NewsServ;
char *s_ShadowServ;
char *s_IrcIIHelp;
char *s_mIRCHelp;
char *s_DevNull;
char *s_BddServ;
char *DEntryMsg;
char *s_CregServ;
char *s_IpVirtual;
char *s_EuskalIRCServ;
int  CregApoyos;
char *OperHost;
char *AdminHost;
char *CoAdminHost;
char *DevelHost;
char *PatrocinaHost;




#ifdef IRC_UNDERNET_P10
char s_NickServP10[4];
char s_ChanServP10[4];
char s_MemoServP10[4];
char s_HelpServP10[4];
char s_OperServP10[4];
char s_CregServP10[4];
char s_SpamServP10[4];
char s_IpVirtualP10[4];
char s_EuskalIRCServP10[4];
char s_GlobalNoticerP10[4];
char s_NewsServP10[4];
char s_IrcIIHelpP10[4];
char s_mIRCHelpP10[4];
char s_DevNullP10[4];
char s_ShadowServP10[4];

#endif
char *desc_NickServ;
char *desc_ChanServ;
char *desc_MemoServ;
char *desc_HelpServ;
char *desc_OperServ;
char *desc_SpamServ;
char *desc_CregServ;
char *desc_IpVirtual;
char *desc_EuskalIRCServ;
char *desc_GlobalNoticer;
char *desc_NewsServ;
char *desc_IrcIIHelp;
char *desc_mIRCHelp;
char *desc_DevNull;
char *desc_ShadowServ;
char *desc_IpVirtual;
char *desc_BddServ;

char *PIDFilename;
char *MOTDFilename;
char *HelpDir;
char *NickDBName;
char *ChanDBName;
char *CregDBName;
char *SpamDBName;
char *OperDBName;
char *IpVirtualDBName;
char *EuskalIRCDBName;
char *AutokillDBName;
char *NewsDBName;

int   SpamUsers;
int   NoBackupOkay;
int   NoSplitRecovery;
int   StrictPasswords;
int   BadPassLimit;
int   BadPassTimeout;
int   UpdateTimeout;
int   ExpireTimeout;
int   ReadTimeout;
int   WarningTimeout;
int   TimeoutCheck;

#ifdef REG_NICK_MAIL
#ifdef SENDMAIL
char *SendMailPatch;
#endif
#ifdef SMTP
char *ServerSMTP;
int  PortSMTP;
#endif
int  *NicksMail;
char *SendFrom;
char *WebNetwork;
#endif

static int NSDefNone;
int   NSForceNickChange;
char *NSGuestNickPrefix;
int   NSDefKill;
int   NSDefKillQuick;
int   NSDefSecure;
int   NSDefPrivate;
int   NSDefHideEmail;
int   NSDefHideUsermask;
int   NSDefHideQuit;
int   NSDefMemoSignon;
int   NSDefMemoReceive;
int   NSRegDelay;
int   NSExpire;
int   NSAccessMax;
char *NSEnforcerUser;
char *NSEnforcerHost;
static char *temp_nsuserhost;
int   NSReleaseTimeout;
int   NSAllowKillImmed;
int   NSDisableLinkCommand;
int   NSListOpersOnly;
int   NSListMax;
int   NSSecureAdmins;

int   CSMaxReg;
int   CSExpire;
int   CSAccessMax;
int   CSAutokickMax;
char *CSAutokickReason;
int   CSInhabit;
int   CSRestrictDelay;
int   CSListOpersOnly;
int   CSListMax;
/* int   ShadowServ;
char *NickShadow;
*/
int   MSMaxMemos;
int   MSSendDelay;
int   MSNotifyAll;

char *CanalAdmins;
char *CanalOpers;
char *CanalCybers;
char *CanalAyuda;
char *CanalSpamers;
char *ServicesRoot;
int   LogMaxUsers;
int   AutokillExpiry;

int   KillClonesAkillExpire;


/*************************************************************************/

/* Deprecated directive (dep_) and value checking (chk_) functions: */

static void dep_ListOpersOnly(void)
{
    NSListOpersOnly = 1;
    CSListOpersOnly = 1;
}

/*************************************************************************/

#define MAXPARAMS	8

/* Configuration directives. */

typedef struct {
    char *name;
    struct {
	int type;	/* PARAM_* below */
	int flags;	/* Same */
	void *ptr;	/* Pointer to where to store the value */
    } params[MAXPARAMS];
} Directive;

#define PARAM_NONE	0
#define PARAM_INT	1
#define PARAM_POSINT	2	/* Positive integer only */
#define PARAM_PORT	3	/* 1..65535 only */
#define PARAM_STRING	4
#define PARAM_TIME	5
#define PARAM_SET	-1	/* Not a real parameter; just set the
				 *    given integer variable to 1 */
#define PARAM_DEPRECATED -2	/* Set for deprecated directives; `ptr'
				 *    is a function pointer to call */

/* Flags: */
#define PARAM_OPTIONAL	0x01
#define PARAM_FULLONLY	0x02	/* Directive only allowed if !STREAMLINED */

Directive directives[] = {
    { "AutokillDB",       { { PARAM_STRING, 0, &AutokillDBName } } },
    { "AutokillExpiry",   { { PARAM_TIME, 0, &AutokillExpiry } } },
    { "BadPassLimit",     { { PARAM_POSINT, 0, &BadPassLimit } } },
    { "BadPassTimeout",   { { PARAM_TIME, 0, &BadPassTimeout } } },
    { "ChanServDB",       { { PARAM_STRING, 0, &ChanDBName } } },
    { "ChanServName",     { { PARAM_STRING, 0, &s_ChanServ },
                            { PARAM_STRING, 0, &desc_ChanServ } } },
    { "ShadowServName",	  { { PARAM_STRING, 0, &s_ShadowServ },
    			    { PARAM_STRING, 0, &desc_ShadowServ } } },
    { "CSAccessMax",      { { PARAM_POSINT, 0, &CSAccessMax } } },
    { "CSAutokickMax",    { { PARAM_POSINT, 0, &CSAutokickMax } } },
    { "CSAutokickReason", { { PARAM_STRING, 0, &CSAutokickReason } } },
    { "CSExpire",         { { PARAM_TIME, 0, &CSExpire } } },
    { "CSInhabit",        { { PARAM_TIME, 0, &CSInhabit } } },
    { "CSListMax",        { { PARAM_POSINT, 0, &CSListMax } } },
    { "CSListOpersOnly",  { { PARAM_SET, 0, &CSListOpersOnly } } },
    { "CSMaxReg",         { { PARAM_POSINT, 0, &CSMaxReg } } },
    { "CSRestrictDelay",  { { PARAM_TIME, 0, &CSRestrictDelay } } },
    { "DevNullName",      { { PARAM_STRING, 0, &s_DevNull },
                            { PARAM_STRING, 0, &desc_DevNull } } },
     { "ShadowName",      { { PARAM_STRING, 0, &s_ShadowServ },
                            { PARAM_STRING, 0, &desc_ShadowServ} } },

    { "ExpireTimeout",    { { PARAM_TIME, 0, &ExpireTimeout } } },
    { "GlobalName",       { { PARAM_STRING, 0, &s_GlobalNoticer },
                            { PARAM_STRING, 0, &desc_GlobalNoticer } } },
    { "HelpDir",          { { PARAM_STRING, 0, &HelpDir } } },
    { "HelpServName",     { { PARAM_STRING, 0, &s_HelpServ },
                            { PARAM_STRING, 0, &desc_HelpServ } } },
    { "IrcIIHelpName",    { { PARAM_STRING, 0, &s_IrcIIHelp },
                            { PARAM_STRING, 0, &desc_IrcIIHelp } } },
    { "mIRCHelpName",    { { PARAM_STRING, 0, &s_mIRCHelp },
                            { PARAM_STRING, 0, &desc_mIRCHelp } } },
    { "KillClonesAkillExpire",{{PARAM_TIME, 0, &KillClonesAkillExpire } } },
    { "ListOpersOnly",    { { PARAM_DEPRECATED, 0, dep_ListOpersOnly } } },
    { "LocalAddress",     { { PARAM_STRING, 0, &LocalHost },
                            { PARAM_PORT, PARAM_OPTIONAL, &LocalPort } } },
    { "LogMaxUsers",      { { PARAM_SET, 0, &LogMaxUsers } } },
    { "MemoServName",     { { PARAM_STRING, 0, &s_MemoServ },
                            { PARAM_STRING, 0, &desc_MemoServ } } },
    { "MOTDFile",         { { PARAM_STRING, 0, &MOTDFilename } } },
    { "MSMaxMemos",       { { PARAM_POSINT, 0, &MSMaxMemos } } },
    { "MSNotifyAll",      { { PARAM_SET, 0, &MSNotifyAll } } },
    { "MSSendDelay",      { { PARAM_TIME, 0, &MSSendDelay } } },
    { "NewsDB",           { { PARAM_STRING, 0, &NewsDBName } } },
    { "NewsServName",     { { PARAM_STRING, 0, &s_NewsServ },
                            { PARAM_STRING, 0, &desc_NewsServ } } },
    { "NickservDB",       { { PARAM_STRING, 0, &NickDBName } } },
    { "NickServName",     { { PARAM_STRING, 0, &s_NickServ },
                            { PARAM_STRING, 0, &desc_NickServ } } },
     { "SpamServDB",       { { PARAM_STRING, 0, &SpamDBName } } },
    { "SpamServName",     { { PARAM_STRING, 0, &s_SpamServ },
                            { PARAM_STRING, 0, &desc_SpamServ } } },
    { "NoBackupOkay",     { { PARAM_SET, 0, &NoBackupOkay } } },

    { "BddServName",	  { { PARAM_STRING, 0, &s_BddServ },
    			    { PARAM_STRING, 0, &desc_BddServ } } },
    { "IPVirtualName",	  { { PARAM_STRING, 0, &s_IpVirtual },
    			    { PARAM_STRING, 0, &desc_IpVirtual } } },

        
#ifdef IRC_UNDERNET_P10
    { "ServerNumerico",   { { PARAM_INT, 0, &ServerNumerico } } },
   /* { "ServerHUB",       { { PARAM_STRING, 0, &ServerHUB } } },*/
#endif        

    { "NoSplitRecovery",  { { PARAM_SET, 0, &NoSplitRecovery } } },
    { "NSAccessMax",      { { PARAM_POSINT, 0, &NSAccessMax } } },
    { "NSAllowKillImmed", { { PARAM_SET, 0, &NSAllowKillImmed } } },
    { "NSDefHideEmail",   { { PARAM_SET, 0, &NSDefHideEmail } } },
    { "NSDefHideQuit",    { { PARAM_SET, 0, &NSDefHideQuit } } },
    { "NSDefHideUsermask",{ { PARAM_SET, 0, &NSDefHideUsermask } } },
    { "NSDefKill",        { { PARAM_SET, 0, &NSDefKill } } },
    { "NSDefKillQuick",   { { PARAM_SET, 0, &NSDefKillQuick } } },
    { "NSDefMemoReceive", { { PARAM_SET, 0, &NSDefMemoReceive } } },
    { "NSDefMemoSignon",  { { PARAM_SET, 0, &NSDefMemoSignon } } },
    { "NSDefNone",        { { PARAM_SET, 0, &NSDefNone } } },
    { "NSDefPrivate",     { { PARAM_SET, 0, &NSDefPrivate } } },
    { "NSDefSecure",      { { PARAM_SET, 0, &NSDefSecure } } },
    { "NSDisableLinkCommand",{{PARAM_SET, 0, &NSDisableLinkCommand } } },
    { "NSEnforcerUser",   { { PARAM_STRING, 0, &temp_nsuserhost } } },
    { "NSExpire",         { { PARAM_TIME, 0, &NSExpire } } },
    { "NSForceNickChange",{ { PARAM_SET, 0, &NSForceNickChange } } },
    { "NSGuestNickPrefix",{ { PARAM_STRING, 0, &NSGuestNickPrefix } } },
    { "NSListMax",        { { PARAM_POSINT, 0, &NSListMax } } },
    { "NSListOpersOnly",  { { PARAM_SET, 0, &NSListOpersOnly } } },
    { "NSRegDelay",       { { PARAM_TIME, 0, &NSRegDelay } } },
    { "NSReleaseTimeout", { { PARAM_TIME, 0, &NSReleaseTimeout } } },
    { "NSSecureAdmins",   { { PARAM_SET, 0, &NSSecureAdmins } } },
    { "OperServDB",       { { PARAM_STRING, 0, &OperDBName } } },
    { "OperServName",     { { PARAM_STRING, 0, &s_OperServ },
                            { PARAM_STRING, 0, &desc_OperServ } } },
    { "CregServDB",       { { PARAM_STRING, 0, &CregDBName } } },
    { "CregServName",     { { PARAM_STRING, 0, &s_CregServ },
                            { PARAM_STRING, 0, &desc_CregServ } } },
			    
    { "IpVirtualDB",       { { PARAM_STRING, 0, &IpVirtualDBName } } },
    { "IpVirtualName",     { { PARAM_STRING, 0, &s_IpVirtual},
                            { PARAM_STRING, 0, &desc_IpVirtual } } },
   
    { "EuskalIRCServDB",       { { PARAM_STRING, 0, &EuskalIRCDBName } } },
    { "EuskalIRCServName",     { { PARAM_STRING, 0, &s_EuskalIRCServ },
                            { PARAM_STRING, 0, &desc_EuskalIRCServ } } },
    { "PIDFile",          { { PARAM_STRING, 0, &PIDFilename } } },
    { "ReadTimeout",      { { PARAM_TIME, 0, &ReadTimeout } } },
    { "RemoteServer",     { { PARAM_STRING, 0, &RemoteServer },
                            { PARAM_PORT, 0, &RemotePort },
                            { PARAM_STRING, 0, &RemotePassword } } },
    { "ServerDesc",       { { PARAM_STRING, 0, &ServerDesc } } },
    { "ServerName",       { { PARAM_STRING, 0, &ServerName } } },
     { "SpamUsers",        { { PARAM_INT, 0, &SpamUsers } } },
    { "ServicesRoot",     { { PARAM_STRING, 0, &ServicesRoot } } },
    { "ServiceUser",      { { PARAM_STRING, 0, &temp_userhost } } },
    { "ServiceSpam",      { { PARAM_STRING, 0, &temp_userspam } } },
    { "ServiceEuskalIRC",      { { PARAM_STRING, 0, &temp_usereuskalirc } } },
    { "DEntryMsg",	  { { PARAM_STRING, 0, &DEntryMsg } } },
    { "OperHost",	  { { PARAM_STRING, 0, &OperHost } } },
    { "AdminHost",	  { { PARAM_STRING, 0, &AdminHost } } },
     { "CoAdminHost",	  { { PARAM_STRING, 0, &CoAdminHost } } },
    { "DevelHost",	  { { PARAM_STRING, 0, &DevelHost } } },
    { "PatrocinaHost",	  { { PARAM_STRING, 0, &PatrocinaHost } } },
     { "CregApoyos",       { { PARAM_POSINT, 0, &CregApoyos } } },
#ifdef REG_NICK_MAIL
#ifdef SENDMAIL
    { "SendMailPatch",    { { PARAM_STRING, 0, &SendMailPatch } } },
#endif
#ifdef SMTP
    { "ServerSMTP",       { { PARAM_STRING, 0, &ServerSMTP } } },
    { "PortSMTP",         { { PARAM_PORT, 0, &PortSMTP } } },
#endif
    { "NicksMail",        { { PARAM_POSINT, 0, &NicksMail } } },
    { "SendFrom",         { { PARAM_STRING, 0, &SendFrom } } },
    { "WebNetwork",       { { PARAM_STRING, 0, &WebNetwork } } },
#endif
  /*  { "ShadowServ",       { { PARAM_SET, PARAM_FULLONLY, &ShadowServ } } }, */
   /* { "ShadowServ",       { { PARAM_STRING, 0, &s_ShadowServ } } },    */
    { "CanalAdmins",      { { PARAM_STRING, 0, &CanalAdmins } } },
    { "CanalOpers",       { { PARAM_STRING, 0, &CanalOpers } } },
    { "CanalCybers",       { { PARAM_STRING, 0, &CanalCybers } } }, 
     { "CanalAyuda",       { { PARAM_STRING, 0, &CanalAyuda } } }, 
	  { "CanalSpamers",       { { PARAM_STRING, 0, &CanalSpamers } } }, 
    { "StrictPasswords",  { { PARAM_SET, 0, &StrictPasswords } } },
    { "TimeoutCheck",     { { PARAM_TIME, 0, &TimeoutCheck } } },
    { "UpdateTimeout",    { { PARAM_TIME, 0, &UpdateTimeout } } },
    { "WarningTimeout",   { { PARAM_TIME, 0, &WarningTimeout } } },
};

/*************************************************************************/

/* Print an error message to the log (and the console, if open). */

void error(int linenum, char *message, ...)
{
    char buf[4096];
    va_list args;

    va_start(args, message);
    vsnprintf(buf, sizeof(buf), message, args);
#ifndef NOT_MAIN
    if (linenum)
	log("%s:%d: %s", SERVICES_CONF, linenum, buf);
    else
	log("%s: %s", SERVICES_CONF, buf);
    if (!nofork && isatty(2)) {
#endif
	if (linenum)
	    fprintf(stderr, "%s:%d: %s\n", SERVICES_CONF, linenum, buf);
	else
	    fprintf(stderr, "%s: %s\n", SERVICES_CONF, buf);
#ifndef NOT_MAIN
    }
#endif
}

/*************************************************************************/

/* Parse a configuration line.  Return 1 on success; otherwise, print an
 * appropriate error message and return 0.  Destroys the buffer by side
 * effect.
 */

int parse(char *buf, int linenum)
{
    char *s, *t, *dir;
    int i, n, optind, val;
    int retval = 1;
    int ac = 0;
    char *av[MAXPARAMS];

    dir = strtok(buf, " \t\r\n");
    s = strtok(NULL, "");
    if (s) {
	while (isspace(*s))
	    s++;
	while (*s) {
	    if (ac >= MAXPARAMS) {
		error(linenum, "Warning: too many parameters (%d max)",
			MAXPARAMS);
		break;
	    }
	    t = s;
	    if (*s == '"') {
		t++;
		s++;
		while (*s && *s != '"') {
		    if (*s == '\\' && s[1] != 0)
			s++;
		    s++;
		}
		if (!*s)
		    error(linenum, "Warning: unterminated double-quoted string");
		else
		    *s++ = 0;
	    } else {
		s += strcspn(s, " \t\r\n");
		if (*s)
		    *s++ = 0;
	    }
	    av[ac++] = t;
	    while (isspace(*s))
		s++;
	}
    }

    if (!dir)
	return 1;

    for (n = 0; n < lenof(directives); n++) {
	Directive *d = &directives[n];
	if (stricmp(dir, d->name) != 0)
	    continue;
	optind = 0;
	for (i = 0; i < MAXPARAMS && d->params[i].type != PARAM_NONE; i++) {
	    if (d->params[i].type == PARAM_SET) {
		*(int *)d->params[i].ptr = 1;
		continue;
	    }
#ifdef STREAMLINED
	    if (d->params[i].flags & PARAM_FULLONLY) {
		error(linenum, "Directive `%s' not available in STREAMLINED mode",
			d->name);
		break;
	    }
#endif
	    if (d->params[i].type == PARAM_DEPRECATED) {
		void (*func)(void);
		error(linenum, "Deprecated directive `%s' used", d->name);
		func = (void (*)(void))(d->params[i].ptr);
		func();  /* For clarity */
		continue;
	    }
	    if (optind >= ac) {
		if (!(d->params[i].flags & PARAM_OPTIONAL)) {
		    error(linenum, "Not enough parameters for `%s'", d->name);
		    retval = 0;
		}
		break;
	    }
	    switch (d->params[i].type) {
	      case PARAM_INT:
		val = strtol(av[optind++], &s, 0);
		if (*s) {
		    error(linenum, "%s: Expected an integer for parameter %d",
			d->name, optind);
		    retval = 0;
		    break;
		}
		*(int *)d->params[i].ptr = val;
		break;
	      case PARAM_POSINT:
		val = strtol(av[optind++], &s, 0);
		if (*s || val <= 0) {
		    error(linenum,
			"%s: Expected a positive integer for parameter %d",
			d->name, optind);
		    retval = 0;
		    break;
		}
		*(int *)d->params[i].ptr = val;
		break;
	      case PARAM_PORT:
		val = strtol(av[optind++], &s, 0);
		if (*s) {
		    error(linenum,
			"%s: Expected a port number for parameter %d",
			d->name, optind);
		    retval = 0;
		    break;
		}
		if (val < 1 || val > 65535) {
		    error(linenum,
			"Port numbers must be in the range 1..65535");
		    retval = 0;
		    break;
		}
		*(int *)d->params[i].ptr = val;
		break;
	      case PARAM_STRING:
		*(char **)d->params[i].ptr = strdup(av[optind++]);
		if (!d->params[i].ptr) {
		    error(linenum, "%s: Out of memory", d->name);
		    return 0;
		}
		break;
	      case PARAM_TIME:
		val = dotime(av[optind++]);
		if (val < 0) {
		    error(linenum,
			"%s: Expected a time value for parameter %d",
			d->name, optind);
		    retval = 0;
		    break;
		}
		*(int *)d->params[i].ptr = val;
		break;
	      default:
		error(linenum, "%s: Unknown type %d for param %d",
				d->name, d->params[i].type, i+1);
		return 0;  /* don't bother continuing--something's bizarre */
	    }
	}
	break;	/* because we found a match */
    }

    if (n == lenof(directives)) {
	error(linenum, "Unknown directive `%s'", dir);
	return 1;	/* don't cause abort */
    }

    return retval;
}

/*************************************************************************/

#define CHECK(v) do {			\
    if (!v) {				\
	error(0, #v " missing");	\
	retval = 0;			\
    }					\
} while (0)

#define CHEK2(v,n) do {			\
    if (!v) {				\
	error(0, #n " missing");	\
	retval = 0;			\
    }					\
} while (0)

/* Read the entire configuration file.  If an error occurs while reading
 * the file or a required directive is not found, print and log an
 * appropriate error message and return 0; otherwise, return 1.
 */

int read_config()
{
    FILE *config;
    int linenum = 1, retval = 1;
    char buf[1024], *s;

    config = fopen(SERVICES_CONF, "r");
    if (!config) {
#ifndef NOT_MAIN
	log_perror("Can't open " SERVICES_CONF);
	if (!nofork && isatty(2))
#endif
	    perror("Can't open " SERVICES_CONF);
	return 0;
    }
    while (fgets(buf, sizeof(buf), config)) {
	s = strchr(buf, '#');
	if (s)
	    *s = 0;
	if (!parse(buf, linenum))
	    retval = 0;
	linenum++;
    }
    fclose(config);

    CHECK(RemoteServer);
    CHECK(ServerName);
#ifdef IRC_UNDERNET_P10
    CHECK(ServerNumerico);
    
#endif
    CHECK(ServerDesc);
    CHECK(OperHost);
    CHECK(AdminHost);
    CHECK(CoAdminHost);
    CHECK(DevelHost);
    CHECK(PatrocinaHost);
    CHECK(DEntryMsg);
    CHEK2(temp_userhost, ServiceUser);
    CHEK2(temp_userspam, ServiceSpam);
    CHEK2(temp_usereuskalirc, ServiceEuskalIRC);
    CHEK2(s_NickServ, NickServName);
     CHEK2(s_SpamServ, SpamServName);
    CHEK2(s_ChanServ, ChanServName);
    CHEK2(s_MemoServ, MemoServName);
    CHEK2(s_HelpServ, HelpServName);
    CHEK2(s_OperServ, OperServName);
    CHEK2(s_CregServ, CregServName);
    CHEK2(s_IpVirtual, IPVirtualName);
    CHEK2(s_EuskalIRCServ, EuskalIRCServName);
    CHEK2(s_GlobalNoticer, GlobalName);
    CHEK2(s_NewsServ, NewsServName);
    CHEK2(s_BddServ, BddServName);
    CHEK2(PIDFilename, PIDFile);
    CHEK2(MOTDFilename, MOTDFile);
    CHECK(HelpDir);
    CHEK2(NickDBName, NickServDB);
    CHEK2(ChanDBName, ChanServDB);
    CHEK2(OperDBName, OperServDB);
    CHEK2(SpamDBName, SpamServDB);
    CHEK2(CregDBName, CregServDB);
    CHEK2(IpVirtualDBName, IpVirtualDB);
    CHEK2(EuskalIRCDBName, EuskalIRCServDB);
    CHEK2(AutokillDBName, AutokillDB);
    CHEK2(NewsDBName, NewsDB);
    CHECK(UpdateTimeout);
    CHECK(ExpireTimeout);
    CHECK(ReadTimeout);
    CHECK(WarningTimeout);
    CHECK(TimeoutCheck);
    CHECK(NSAccessMax);
    CHEK2(temp_nsuserhost, NSEnforcerUser);
    CHECK(NSReleaseTimeout);
    CHECK(NSListMax);
    CHECK(CSAccessMax);
    CHECK(CSAutokickMax);
    CHECK(CSAutokickReason);
    CHECK(CSInhabit);
    CHECK(CSListMax);
    CHECK(CanalAdmins);
    CHECK(CanalOpers);
    CHECK(CanalAyuda);
    CHECK(CanalCybers);
    CHECK(CanalSpamers);
    CHECK(ServicesRoot);
    CHECK(AutokillExpiry);
#ifdef REG_NICK_MAIL
#ifdef SENDMAIL
    CHECK(SendMailPatch);
#endif
#ifdef SMTP
    CHECK(ServerSMTP);
    CHECK(PortSMTP);
#endif
    CHECK(NicksMail);
    CHECK(SendFrom);
    CHECK(WebNetwork);
#endif


    if (temp_userhost) {
	if (!(s = strchr(temp_userhost, '@'))) {
	    error(0, "Falta `@' para ServiceUser");
	} else {
	    *s++ = 0;
	    ServiceUser = temp_userhost;
	    ServiceHost = s;
	}
    }
if (temp_userspam) {
	if (!(s = strchr(temp_userspam, '@'))) {
	    error(0, "Falta `@' para ServiceSpam");
	} else {
	    *s++ = 0;
	    ServiceUserSpam = temp_userspam;
	    ServiceHostSpam = s;
	}
    }
if (temp_usereuskalirc) {
	if (!(s = strchr(temp_usereuskalirc, '@'))) {
	    error(0, "Falta `@' para ServiceEuskalIRC");
	} else {
	    *s++ = 0;
	    ServiceUserEuskalIRC = temp_usereuskalirc;
	    ServiceHostEuskalIRC = s;
	}
    }
    if (temp_nsuserhost) {
	if (!(s = strchr(temp_nsuserhost, '@'))) {
	    NSEnforcerUser = temp_nsuserhost;
	    NSEnforcerHost = ServiceHost;
	} else {
	    *s++ = 0;
	    NSEnforcerUser = temp_userhost;
	    NSEnforcerHost = s;
	}
    }

    if (!NSDefNone &&
		!NSDefKill &&
		!NSDefKillQuick &&
		!NSDefSecure &&
		!NSDefPrivate &&
		!NSDefHideEmail &&
		!NSDefHideUsermask &&
		!NSDefHideQuit &&
		!NSDefMemoSignon &&
		!NSDefMemoReceive
    ) {
	NSDefSecure = 1;
	NSDefMemoSignon = 1;
	NSDefMemoReceive = 1;
    }


    if (s_ShadowServ) 
        CHECK(desc_ShadowServ);
     
    return retval;
}

/*************************************************************************/
