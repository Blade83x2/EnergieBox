#include <gtk/gtk.h>


// https://www.youtube.com/watch?v=6dstft52Rbs&list=PL6A4216D6C8E00CFC&index=1




void btnExit(GtkWidget *widget, gpointer data) {
	gtk_main_quit();
}



gboolean fadeOutImage(gpointer data) {
    GtkWidget *image = GTK_WIDGET(data);
    gdouble current_opacity = gtk_widget_get_opacity(image);

    // Reduziere die Opazität schrittweise
    if (current_opacity > 0.1) {
        gtk_widget_set_opacity(image, current_opacity - 0.1);
        return TRUE;  // Timer weiterlaufen lassen
    } else {
        // Wenn die Opazität nahe 0 ist, Widget ausblenden und Timer stoppen
        gtk_widget_hide(image);
        return FALSE; // Timer stoppen
    }
}

void displayWelcomeGraphic(GtkWidget *parent) {
    GtkWidget *image;
    image = gtk_image_new_from_file("welcome_image.png");
    gtk_container_add(GTK_CONTAINER(parent), image);

    // Timer einrichten, um das Bild nach 10 Sekunden auszublenden
    g_timeout_add_seconds(10, (GSourceFunc) fadeOutImage, image);
}




int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv);
	GtkWidget *w; // window
	GtkWidget *b; // Button
	w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(w), "EnergieBox Ver. 3.0 by Johannes Krämer");

	gtk_window_set_default_size(GTK_WINDOW(w), 800, 480);
	gtk_window_fullscreen(GTK_WINDOW(w));
	gtk_window_set_position(GTK_WINDOW(w), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(w),25);


	// Willkommensgrafik anzeigen
//	displayWelcomeGraphic(w);

  	b = gtk_button_new_with_label("Button");
  	g_signal_connect(b, "clicked", G_CALLBACK(btnExit), NULL);


  	gtk_container_add(GTK_CONTAINER(w), b);
	gtk_widget_show_all(w);
	gtk_main();
	return 0;
	// sudo nano gui.cpp && sudo gcc gui.cpp -o gui $(pkg-config --cflags gtk+-3.0 --libs) && ./gui
}
