#include <gtk/gtk.h>


// https://www.youtube.com/watch?v=6dstft52Rbs&list=PL6A4216D6C8E00CFC&index=1




void btnExit(GtkWidget *widget, gpointer data) {
	gtk_main_quit();
}


int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv);
	GtkWidget *window; // window
	GtkWidget *button; // Button
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "EnergieBox Ver. 4.0 by Johannes Krämer");

	gtk_window_set_default_size(GTK_WINDOW(window), 800, 480);
	gtk_window_fullscreen(GTK_WINDOW(window));
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window),25);


  	button = gtk_button_new_with_label("Button");
  	g_signal_connect(button, "clicked", G_CALLBACK(btnExit), NULL);


  	gtk_container_add(GTK_CONTAINER(window), button);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
	// sudo nano gui.cpp && sudo gcc gui.cpp -o gui $(pkg-config --cflags gtk+-3.0 --libs) && ./gui
}
