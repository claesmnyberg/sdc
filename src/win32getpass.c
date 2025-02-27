
#include <windows.h>
#include "win32getpass.h"

/* Local routines */
static BOOL CALLBACK DialogFunc(HWND, UINT, WPARAM, LPARAM);


/* Local variables */
static char buf[1024];
static const char *prompt;


/*
 * Get password from dialog box
 */
char *win32getpass(const char *user_prompt)
{
	if (user_prompt != NULL)
		prompt = user_prompt;
	else
		prompt = "Self Decrypting Binary (SDC)";

	DialogBox(NULL, MAKEINTRESOURCE(GETPASS_IDD_MAINDIALOG), 
		NULL, (DLGPROC) DialogFunc);

	if (strlen(buf)) {
		if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = '\0';
	}
	if (strlen(buf)) {
		if (buf[strlen(buf)-1] == '\r')
			buf[strlen(buf)-1] = '\0';
	}
	
	return(buf);
}


/*
 * GetPass dialog box callback
 */
static BOOL
CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		
		/* Init window */
		case WM_INITDIALOG:
			memset(buf, 0x00, sizeof(buf));
			SetDlgItemText(hwndDlg, GETPASS_LTEXT, prompt);
			return(TRUE);
		break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {

				/* Read password from edit field */
				case IDOK:
					memset(buf, 0x00, sizeof(buf));
					GetDlgItemText(hwndDlg, GETPASS_IDPWDINPUT, buf, sizeof(buf));
					EndDialog(hwndDlg,1);
					return(1);

				case IDCANCEL:
					EndDialog(hwndDlg,1);
					exit(EXIT_SUCCESS);
					return(1);
			}
			break;

		/* Window closed */
		case WM_CLOSE:
			EndDialog(hwndDlg,1);
			return(TRUE);
			break;
	}

	return(FALSE);
}
