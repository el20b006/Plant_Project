#include "header.h"

int main(int argc, char** argv)
{
	GtkWidget* window;
	gtk_init(&argc, &argv);

	struct my_widgets *wid = (struct my_widgets*)g_malloc(sizeof(struct my_widgets));

	// create main window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "TempCheck");

	// disable window resize
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	// connect destroy event to main window
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	// fill window
	CreateMenu(vbox);
	
	// Create temperature and Image window
	wid->tempLabel = gtk_label_new("Temp");
	wid->plantPic = gtk_image_new_from_file("OK.PNG");
	gtk_box_pack_start(GTK_BOX(vbox), wid->plantPic, FALSE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(vbox), wid->tempLabel, FALSE, FALSE, 10);
	g_timeout_add(100, on_timeout, (gpointer)wid);

	// open UART port
	m_hParallelComm = CreateFile2(name, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, NULL);
	if (m_hParallelComm == INVALID_HANDLE_VALUE)
	{
		ErrorHandler(1);
		return 1;
	}
	
	gtk_widget_show_all(window);
	gtk_main();

	// close UART port
	CloseHandle(m_hParallelComm);
	g_free(wid);
	wid = NULL;

	return 0;
}

static gboolean on_timeout(gpointer user_data)
{
	struct my_widgets* wid = (struct my_widgets*)user_data;

	// Get Temp via UART port
	float Temp = GetTemp();

	// update to current temperature
	char* text = g_strdup_printf("%.1f °C", Temp);
	gtk_label_set_text(GTK_LABEL(wid->tempLabel), text);
	
	// update picture
	if (Temp < 25 && Temp > 16)
		gtk_image_set_from_file(GTK_IMAGE(wid->plantPic), "planthappy.png");

	else if (Temp > 25)
		gtk_image_set_from_file(GTK_IMAGE(wid->plantPic), "planthot.png");

	else if (Temp < 16)
		gtk_image_set_from_file(GTK_IMAGE(wid->plantPic), "plantcold.png");
	
	g_free(text);
	return G_SOURCE_CONTINUE;
}

float GetTemp()
{
	// read from UART port
	if (!ReadFile(m_hParallelComm, ReadBuffer, sizeof(ReadBuffer), &dwRead, NULL))
		ErrorHandler(1);

	// different calculaten at different length.
	if (ReadBuffer[2] == '.')
	{
		// convert received data (unsigned char) to float
		float Temp = (float(ReadBuffer[0] - 48) * 10.0) + (float(ReadBuffer[1] - 48)) + (float(ReadBuffer[3] - 48) * 0.1);
		return Temp;
	}
	else if (ReadBuffer[1] == '.')
	{
		// convert received data (unsigned char) to float
		float Temp = (float(ReadBuffer[0] - 48)) + (float(ReadBuffer[2] - 48) * 0.1);
		return Temp;
	}
	else
	{
		ErrorHandler(0);
		return 0.0;
	}
}

void CreateMenu(GtkWidget* vbox)
{
	GtkWidget* menubar;
	GtkWidget* menu;
	GtkWidget* menuItem;

	GSList* group = NULL;

	menubar = gtk_menu_bar_new();

	// Menu
	menu = gtk_menu_new();

	menuItem = gtk_menu_item_new_with_label("Menu");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuItem);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), menu);

	menuItem = gtk_menu_item_new_with_label("Description");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(Description), NULL);

	menuItem = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(gtk_main_quit), NULL);

	// help menu
	menu = gtk_menu_new();

	menuItem = gtk_menu_item_new_with_label("?");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuItem);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), menu);

	menuItem = gtk_menu_item_new_with_label("About");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
	g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(ShowAbout), NULL);

	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
}

void Description()
{
	// create dialog
	GtkWidget* aboutDialog;
	aboutDialog = gtk_dialog_new_with_buttons("Description", GTK_WINDOW(vbox), GTK_DIALOG_MODAL, "Close", GTK_RESPONSE_CLOSE, NULL);

	// get content area where widgets can be added
	GtkWidget* dialogVbox = gtk_dialog_get_content_area(GTK_DIALOG(aboutDialog));

	// create text
	GtkWidget* text = gtk_label_new("A Peace Lily needs 16°C - 25°C to grow.\nThe Program will show it to you, if it is too cold or too warm.");
	gtk_widget_show(text);
	gtk_box_pack_start(GTK_BOX(dialogVbox), text, FALSE, FALSE, 10);

	gtk_dialog_run(GTK_DIALOG(aboutDialog));
	gtk_widget_destroy(aboutDialog);
}

void ShowAbout()
{
	// create dialog
	GtkWidget* aboutDialog;
	aboutDialog = gtk_dialog_new_with_buttons("About", GTK_WINDOW(vbox), GTK_DIALOG_MODAL, "Close", GTK_RESPONSE_CLOSE, NULL);

	// get content area where widgets can be added
	GtkWidget* dialogVbox = gtk_dialog_get_content_area(GTK_DIALOG(aboutDialog));

	// create text
	GtkWidget* text = gtk_label_new("TempCheck\nVersion 1.0\nMade by Sarah Froeler\n01/22");
	gtk_widget_show(text);
	gtk_box_pack_start(GTK_BOX(dialogVbox), text, FALSE, FALSE, 10);

	gtk_dialog_run(GTK_DIALOG(aboutDialog));
	gtk_widget_destroy(aboutDialog);
}

void ErrorHandler(int ErrorFlag)
{
	switch (ErrorFlag) {
	case 0:
		printf("Error!Temperetaure received are over 100°C!");
	case 1:
		printf("Error! No UART connection!");
	}
}