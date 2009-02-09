/* Servicio EuskalIRC, botshispanobdd
 *
 * (C) 2009 donostiarra 
 *
 * Este programa es software libre. Puede redistribuirlo y/o modificarlo
 * bajo los t�rminos de la Licencia P�blica General de GNU seg�n es publicada
 * por la Free Software Foundation.
 */
#include "services.h"
#include "pseudo.h"
 /* Main EuskalServ routine. */

void do_euskal(User *u) /*la colocamos en extern.h y asi la llamamos desde oper*/
{    
        char *cmd, *nick;
        cmd = strtok(NULL, " ");
        nick = strtok(NULL, " ");
        NickInfo *ni;
    
    char adm[BUFSIZE];
      
  
    /*    User *u2 = NULL; */

    if ((!cmd) || ((!stricmp(cmd, "ACEPTA") == 0) && (!stricmp(cmd, "RECHAZA") == 0))) {
       privmsg(s_EuskalIRCServ,u->nick, " /msg %s 2DUDA 12ACEPTA/RECHAZA 5<NICK>",s_OperServ);
    	return;
    }
    
    if (!nick) {
    	privmsg(s_EuskalIRCServ, u-> nick, "4Falta un Nick /msg %s 2DUDA 12ACEPTA/RECHAZA 5<NICK>",s_OperServ);
    	return;
    } else if (!(ni = findnick(nick))) {
	notice_lang(s_EuskalIRCServ, u, NICK_X_NOT_REGISTERED, nick);
	return;
    } else  if (!(ni->status & NI_ON_BDD)) {
        notice_lang(s_EuskalIRCServ, u, NICK_MUST_BE_ON_BDD);
	return;
    }
    
    
   
      if (stricmp(cmd, "ACEPTA") == 0) {
	if (!is_services_oper(u)) {
	    notice_lang(s_EuskalIRCServ, u, PERMISSION_DENIED);
	    return;
	} else 
          privmsg(s_EuskalIRCServ, nick, "El OPERador/a 5%s se pondr� en contacto contigo en breve.Por favor, abandone el canal una vez atendido. Gracias.",u->nick);
	canaladmins( s_EuskalIRCServ,"12OPER 4%s 3ACEPTA DUDA de  2%s",u->nick,nick);
            } 
       
       else if (stricmp(cmd, "RECHAZA") == 0) {
	if (!is_services_oper(u)) {
	    notice_lang(s_EuskalIRCServ, u, PERMISSION_DENIED);
	    return;
	}  else
          privmsg(s_EuskalIRCServ, nick , "El OPERador/a 5%s ha rechazado la solicitud de ayuda.",u->nick);
          canaladmins( s_EuskalIRCServ,"12OPER 4%s 5RECHAZA DUDA de  2%s",u->nick,nick);
	
            }
  
      
}


void euskalserv(const char *source, char *buf)
{
    int num;
    NickInfo *ni;
    char *cmd;
    char *s;
    User *u = finduser(source);

    ni = findnick(source);
    char cyb[BUFSIZE];
snprintf(cyb, sizeof(cyb), "#%s", CanalCybers);

    if (!u) {
	log("%s: user record for %s not found", s_EuskalIRCServ, source);
	notice(s_EuskalIRCServ, source,
		getstring((NickInfo *)NULL, USER_RECORD_NOT_FOUND));
	return;
    }

    //log("yiha %s: %s: %s", s_EuskalIRCServ, source, buf);
  canaladmins( s_EuskalIRCServ,"5CONSULTA! 12%s : 2%s", source,buf);
   if ((ni = u->real_ni))
   if (ni->in_cyb & CYB_NO) {
   send_cmd(s_EuskalIRCServ, "MODE #%s +v %s", CanalCybers, ni->nick);
   privmsg(s_EuskalIRCServ, ni->nick , "Gracias, en breve te informar� del nick del OPERador/a que te va a ayudar. Por favor, no abandones el canal mientras eres atendido/a");
   ni->in_cyb = CYB_SI ;
   }
   else return;
    cmd = strtok(buf, " ");
    if (!cmd) {
	return;
    } else if (stricmp(cmd, "\1PING") == 0) {
	if (!(s = strtok(NULL, "")))
	    s = "\1";
	notice(s_EuskalIRCServ, source, "\1PING %s", s);
    } else if (stricmp(cmd, "\1VERSION\1") == 0) {
        notice(s_EuskalIRCServ, source, "\1VERSION %s %s -- %s\1",
               PNAME, s_EuskalIRCServ, version_build);                
    }/* else {
	run_cmd(s_EuskalIRCServ, u, cmds, cmd); }*/
   
}
void euskalirc_canal(const char *source,const char *chan, char *buf)
 
 {
 
 char adm[BUFSIZE];
char cyb[BUFSIZE];
snprintf(adm, sizeof(adm), "#%s", CanalAdmins);
snprintf(cyb, sizeof(cyb), "#%s", CanalCybers);
   
 
if (!strcmp(chan, adm))  return;
if (!strcmp(chan, cyb)) {
	        privmsg(s_EuskalIRCServ, adm , "Ey! %s me dijo en el canal %s 4%s !!", source,cyb,buf);
			return;
          }
		
	
	
 }
    
 void mirar_pregunta(const char *source, char *buf[BUFSIZE])
 {
 int i;
 User *u = finduser(source);
 const char *cmd = strtok(NULL, "");
if (!cmd) {
	//notice_help(s_OperServ, u, OPER_HELP);
    } else {
	/*help_cmd(s_OperServ, u, cmds, cmd);*/
    }

       //canalopers( s_EuskalIRCServ,"Ey! %s me dijo al privi 4%s !!", source,buf);
			return;

 }

 void mandar_mensaje(const char *source)
 {
    NickInfo *ni;
    User *u = finduser(source);
     ni = findnick(source);
    time_t ahora = time(NULL);
    time_t caducado;
     char cyb[BUFSIZE];
     snprintf(cyb, sizeof(cyb), "#%s", CanalCybers);

    struct tm *tm;
      
      if ((ni = u->real_ni) && !(is_services_oper(u))) {
	 ni->in_cyb = CYB_NO;
        privmsg(s_EuskalIRCServ,source, "Hola %s",source);
	privmsg(s_EuskalIRCServ,source, "Soy el encargado de ponerte en contacto con un OPER de Servicios.");
	privmsg(s_EuskalIRCServ,source, "  5,15 Por favor, �podr�as describirme en una l�nea el problema?");
         }
        else if  (!is_services_oper(u)) {
          privmsg(s_EuskalIRCServ,source, "Buenas 2%s5",source);
	  privmsg(s_EuskalIRCServ,source, "Veo que no tienes el Nick Registrado.");
          privmsg(s_EuskalIRCServ,source, "Para Solicitar Soporte debe Registrarse Primero.");
          privmsg(s_EuskalIRCServ,source, "2Instrucciones de Registro: lo encontrara ejecutando el comando 4/msg 2%s %s",s_HelpServ,s_NickServ);
            return;
          } 
   if   (is_services_root(u))  {
    privmsg(s_EuskalIRCServ,cyb, "Hola 4%s,BienVenido/a 5Root de Red ",source);
    return;
     }
     else if (is_services_admin(u))  {
    privmsg(s_EuskalIRCServ,cyb, "Hola 4%s,BienVenido/a 5Administrador de Red ",source);
    return;
     }
     else if (is_services_cregadmin(u))  {
    privmsg(s_EuskalIRCServ,cyb, "Hola 4%s,BienVenido/a 5Co-Administrador de Red ",source);
    return;
     }
    else if (is_services_devel(u))  {
    privmsg(s_EuskalIRCServ,cyb, "Hola 4%s,BienVenido/a 5Devel de Red ",source);
    return;
    }
    else if (is_services_oper(u))  {
    privmsg(s_EuskalIRCServ,cyb, "Hola 4%s,BienVenido/a 5Operador de Red ",source);
    return;
    }
     
}
