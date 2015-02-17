#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include "padviz.ui.h"

// Driver event struct
typedef struct {
    uint32_t time;
    int16_t value;
    uint8_t type;
    uint8_t num;
} padev_t;

// Pad ui elements
GtkWidget *up, *down, *left, *right, *sel, *start, *btnA, *btnB;

// Pad device fd
int padfd;

// Process pad event
static void process(padev_t *ev) {
    printf("ts=%-10u type=%-6u val=%-6d num=%-6u\n", ev->time, ev->type, ev->value, ev->num);
    if (ev->type == 2) {
        // D-pad
        if (ev->num == 0) {
            // Left-right
            gtk_widget_hide(left);
            gtk_widget_hide(right);
            if (ev->value > 0) gtk_widget_show(right); else if (ev->value < 0) gtk_widget_show(left);
        } else {
            // Up-down
            gtk_widget_hide(up);
            gtk_widget_hide(down);
            if (ev->value > 0) gtk_widget_show(down); else if (ev->value < 0) gtk_widget_show(up);
        }
    } else if (ev->type == 1) {
        // A, B, sel, or start
        if (ev->num == 0) {
            if (ev->value == 0) gtk_widget_hide(btnA); else gtk_widget_show(btnA);
        } else if (ev->num == 1) {
            if (ev->value == 0) gtk_widget_hide(btnB); else gtk_widget_show(btnB);
        } else if (ev->num == 2) {
            if (ev->value == 0) gtk_widget_hide(sel); else gtk_widget_show(sel);
        } else if (ev->num == 3) {
            if (ev->value == 0) gtk_widget_hide(start); else gtk_widget_show(start);
        }
    }
}

// Read pad events
static gboolean readpad(gpointer userdata) {
    int rc;
    fd_set set;
    struct timeval timeout;
    padev_t ev;

    do {
        // Read pad, timing out after 5ms
        timeout.tv_sec = 0;
        timeout.tv_usec = 5000;
        FD_ZERO(&set);
        FD_SET(padfd, &set);
        rc = select(padfd + 1, &set, NULL, NULL, &timeout);

        if (rc < 0) {
            // Select error
            perror("select");
            return FALSE;
        } else if (rc > 0) {
            // Data was available
            read(padfd, &ev, sizeof(ev));
            process(&ev);
        }

    // Loop again if there was data
    } while (rc > 0);

    return TRUE;
}

// Program entry point
int main (int argc, char **argv) {
    GtkBuilder *builder;
    GtkWidget *wmain;

    // Open pad device
    if ((padfd = open(argc > 1 ? argv[1] : "/dev/input/js0", O_RDONLY)) == -1) {
        perror("open");
        return 1;
    }

    // Make ui
    gtk_init(&argc, &argv);
    builder = gtk_builder_new_from_string(padviz_ui, padviz_ui_len);
    wmain = GTK_WIDGET(gtk_builder_get_object(builder, "wmain"));
    up = GTK_WIDGET(gtk_builder_get_object(builder, "up"));
    down = GTK_WIDGET(gtk_builder_get_object(builder, "down"));
    left = GTK_WIDGET(gtk_builder_get_object(builder, "left"));
    right = GTK_WIDGET(gtk_builder_get_object(builder, "right"));
    sel = GTK_WIDGET(gtk_builder_get_object(builder, "sel"));
    start = GTK_WIDGET(gtk_builder_get_object(builder, "start"));
    btnA = GTK_WIDGET(gtk_builder_get_object(builder, "btnA"));
    btnB = GTK_WIDGET(gtk_builder_get_object(builder, "btnB"));
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    gtk_widget_show_all(wmain);
    g_idle_add(readpad, NULL); // Read pad when idle
    g_signal_connect(wmain, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_hide(up);
    gtk_widget_hide(down);
    gtk_widget_hide(left);
    gtk_widget_hide(right);
    gtk_widget_hide(sel);
    gtk_widget_hide(start);
    gtk_widget_hide(btnA);
    gtk_widget_hide(btnB);

    // Run ui
    gtk_main();

    // Close pad
    close(padfd);

    return 0;
}
