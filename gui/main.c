#include <gtk/gtk.h>

int main (int argc, char** argv) {
   GtkBox *box;
   GtkStack *stack;
   GtkLabel *label1;
   GtkLabel *label2;
   GtkWindow *window;
   GtkStackSwitcher *switcher;
   GValue iconval1 = G_VALUE_INIT;
   GValue iconval2 = G_VALUE_INIT;

   gtk_init (&argc, &argv);

   g_value_init (&iconval1, G_TYPE_STRING);
   g_value_init (&iconval2, G_TYPE_STRING);

   window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
   box    = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6));
   stack  = GTK_STACK(gtk_stack_new ());
   switcher = GTK_STACK_SWITCHER(gtk_stack_switcher_new ());

   label1 = GTK_LABEL(gtk_label_new("12V Sektion"));
   label2 = GTK_LABEL(gtk_label_new("230V Sektion"));

   gtk_stack_add_titled(stack, GTK_WIDGET(label1), "Page 1", "Page 11");
   gtk_stack_add_titled(stack, GTK_WIDGET(label2), "Page 2", "Page 22");

   gtk_widget_set_halign (GTK_WIDGET(switcher), GTK_ALIGN_CENTER);

   g_value_set_string(&iconval1, "zoom-in-symbolic.symbolic");
   g_value_set_string(&iconval2, "zoom-out-symbolic.symbolic");

   gtk_container_child_set_property(GTK_CONTAINER(stack), GTK_WIDGET(label1), "icon-name", &iconval1);
   gtk_container_child_set_property(GTK_CONTAINER(stack), GTK_WIDGET(label2), "icon-name", &iconval2);

   gtk_stack_switcher_set_stack (switcher, stack);
   gtk_box_pack_start (box, GTK_WIDGET(switcher), FALSE, FALSE, 6);
   gtk_box_pack_start (box, GTK_WIDGET(stack), TRUE, TRUE, 6);

   gtk_container_add (GTK_CONTAINER(window), GTK_WIDGET(box));

   g_signal_connect(G_OBJECT(window), "destroy", gtk_main_quit, NULL);
   gtk_window_set_title(GTK_WINDOW(window), "EnergyBox ver. 3.0");
   gtk_window_set_default_size(GTK_WINDOW(window), 800, 480);
   gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
   gtk_widget_show_all (GTK_WIDGET(window));

   gtk_main ();

   return 0;
}
