#ifndef _WIN32_GETPASS_H
#define _WIN32_GETPASS_H

#define GETPASS_LTEXT			102
#define GETPASS_IDPWDINPUT      101
#define GETPASS_IDD_MAINDIALOG  100

extern char *win32getpass(const char *prompt);

#endif /* _WIN32_GETPASS_H */
