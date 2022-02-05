#include <iostream>
#include <gtk/gtk.h>
#include <Windows.h>

void CreateMenu(GtkWidget* vbox_main);
static gboolean on_timeout(gpointer user_data);
float GetTemp();
void ShowAbout();
void Description();
void ErrorHandler(int ErrorFlag);

GtkWidget* vbox;
LPCWSTR name = L"COM3";
BYTE ReadBuffer[6];
DWORD dwRead;
BYTE b = 0;
HANDLE m_hParallelComm;

struct my_widgets {
	GtkWidget* tempLabel;
	GtkWidget* plantPic;
};