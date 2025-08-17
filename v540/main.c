#include <gtk/gtk.h>
#include <stdio.h>
//#include <stdint.h>
#include <pthread.h>
//#include <string.h>
//#include <errno.h>

#include "shmem.h"

GtkApplication* app=NULL;
GtkWidget* main_win=NULL;
uint8_t *video_mem=NULL;
char chrfile[256][20];
GtkWidget* grid=NULL;
struct video540_t* mbx=NULL;

implement_fifo(v540_update)


////////////////////////////////////////////////////////////////////////////////
//
// updater object
//
//

#define UPDATER_TYPE_OBJECT updater_object_get_type()

G_DECLARE_FINAL_TYPE(UpdaterObject,updater_object,UPDATER,OBJECT,GObject)
struct _UpdaterObject {
    GObject parent_instance;
};

G_DEFINE_TYPE(UpdaterObject,updater_object,G_TYPE_OBJECT)

enum {
    UPDATE_CHR_SIGNAL,
    UPDATE_ALL_SIGNAL,
    LAST_SIGNAL
};

static guint updater_object_signals[LAST_SIGNAL] = {0};

static void updater_object_class_init(UpdaterObjectClass *klass) {
    updater_object_signals[UPDATE_CHR_SIGNAL] = g_signal_new(
        "update_chr-signal",
        G_TYPE_FROM_CLASS( klass ),
        G_SIGNAL_RUN_LAST,
        0,
        NULL,
        NULL,
        NULL,
        G_TYPE_NONE,
        0
    );

    updater_object_signals[UPDATE_ALL_SIGNAL] = g_signal_new(
        "update_all-signal",
        G_TYPE_FROM_CLASS( klass ),
        G_SIGNAL_RUN_LAST,
        0,
        NULL,
        NULL,
        NULL,
        G_TYPE_NONE,
        0
    );
}

static void updater_object_init(UpdaterObject* self) {};


static UpdaterObject* theUpdater=NULL;

//
// end of updater object
//
////////////////////////////////////////////////////////////////////////////////

static int ready = 0;


static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    printf("Key pressed: keyval=%u, hardware_keycode=%u\n", event->keyval, event->hardware_keycode);
    return FALSE; // Return TRUE to stop further handling
}

static gboolean on_key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    printf("Key released: keyval=%u, hardware_keycode=%u\n", event->keyval, event->hardware_keycode);
    return FALSE;
}



// Create a GtkImage from a PNG file named ch_XX.png

GtkWidget* create_image_from_index(uint16_t index) {
    char* pfile=chrfile[video_mem[index]];
    //printf("Creating image from %s\n", pfile);
    if (index >= 2048) {
        g_warning("Index out of bounds: %d", index);
        return gtk_image_new(); // Empty image
    }
    if (video_mem[index] >= 256) {
        g_warning("Invalid character value: %d", video_mem[index]);
        return gtk_image_new(); // Empty image
    }
    //printf("Video memory at index %d is 0x%2.2X\n", index, video_mem[index]);

    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(pfile, NULL);
    GdkPixbuf* p2=gdk_pixbuf_scale_simple(pixbuf,12,24,GDK_INTERP_NEAREST);
    if (!pixbuf) {
        g_warning("Failed to load image: %s", pfile);
        return gtk_image_new(); // Empty image
    }
    GtkWidget *image = gtk_image_new_from_pixbuf(p2);
    g_object_unref(pixbuf);
    g_object_unref(p2);
    return image;
}


/* static void on_paint_all(GtkWidget* one, gpointer data) {
    printf("on_paint_all called\n");
    if (ready) {
        printf("ready!\n");

        // Remove all children from the grid
        GList *children, *iter;
        children = gtk_container_get_children(GTK_CONTAINER(grid));
        for (iter = children; iter != NULL; iter = g_list_next(iter)) {
            gtk_widget_destroy(GTK_WIDGET(iter->data));
        }
        g_list_free(children);

        for (int i = 0; i < 2048; i++) {
            int row = i >> 6;
            int col = i & 0x3f;
            GtkWidget* chr = create_image_from_index(i);
            gtk_grid_attach(GTK_GRID(grid), chr, col, row, 1, 1);
            gtk_widget_queue_draw(chr);
        }
    }
    gtk_widget_queue_draw(main_win);
} */

/* GtkWidget* get_grid_child_at(GtkGrid *grid, int col, int row) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(grid));
    for (GList *l = children; l != NULL; l = l->next) {
        GtkWidget *child = GTK_WIDGET(l->data);
        int c, r, w, h;
        gtk_grid_query_child(grid, child, &c, &r, &w, &h);
        if (c == col && r == row) {
            g_list_free(children);
            return child;
        }
    }
    g_list_free(children);
    return NULL;
} */

static void on_paint_all( GtkWidget* one, gpointer data ) {
    //printf("on_paint_all called\n");
    if (ready) {
        //printf("ready!\n");
        for (int i=0; i<2048; i++) {
            int row=i>>6;
            int col=i&0x3f;
            GtkWidget* existing_chr = gtk_grid_get_child_at(GTK_GRID(grid), col, row);

            if (existing_chr) {
                gtk_widget_destroy(existing_chr);
            }
            else {
                printf("No existing child at (%d, %d)\n", col, row);
            }
            GtkWidget* chr = create_image_from_index(i);
            gtk_grid_attach(GTK_GRID(grid),chr,col,row,1,1);
            gtk_widget_show(chr);
            //gtk_widget_queue_draw(chr);
        }
    }
    //gtk_widget_queue_draw_area(main_win, 0, 0, 320, 200);
    gtk_widget_queue_draw(grid);
    shm_update_finalize();
}

static void on_paint_char( GtkWidget* one, gpointer data ) {
    //printf("on_paint_char called\n");
    if (ready) {
        //printf("ready!\n");
        int row;
        int col;
        int addr;
        v540_update* ptr=v540_update_head(&mbx->vm_write);
        if (!ptr) return;

        row=(ptr->addr)>>6;
        col=(ptr->addr)&0x3f;
        addr=(ptr->addr)&0x7FF;
        v540_update_pop(&mbx->vm_write);
        char* pfile=chrfile[video_mem[addr]];
        GtkWidget* existing_chr = gtk_grid_get_child_at(GTK_GRID(grid), col, row);

        if (existing_chr) {
            gtk_widget_destroy(existing_chr);
        }
        else {
            printf("No existing child at (%d, %d)\n", col, row);
        }
        GtkWidget* chr = create_image_from_index(addr);
        gtk_grid_attach(GTK_GRID(grid),chr,col,row,1,1);
        gtk_widget_show(chr);
        //gtk_widget_queue_draw(chr);
    }
    //gtk_widget_queue_draw_area(main_win, 0, 0, 320, 200);
    gtk_widget_queue_draw(grid);
    shm_update_finalize();
}


static void act( GtkApplication* app, gpointer user_data) {
    main_win=gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(main_win),"OSI Challenger 4P");
    gtk_window_set_default_size(GTK_WINDOW(main_win), 320,200);

    // Ensure the window will receive key events
    gtk_widget_add_events(main_win, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

    // Connect the key event signals
    g_signal_connect(main_win, "key-press-event", G_CALLBACK(on_key_press), NULL);
    g_signal_connect(main_win, "key-release-event", G_CALLBACK(on_key_release), NULL);

    gtk_widget_show_all(main_win);
    grid=gtk_grid_new();

    for (int i=0; i<2048; i++) {
        int row=i>>6;
        int col=i&0x3f;
        GtkWidget* chr = create_image_from_index(i);
        gtk_grid_attach(GTK_GRID(grid),chr,col,row,1,1);
    }
    gtk_container_add(GTK_CONTAINER(main_win),grid);
    gtk_widget_show_all(main_win);


    ready = 1;
}


static gboolean emit_paint_all( gpointer data ) {
    //printf("Painting all\n");
    g_signal_emit_by_name(theUpdater,"update_all-signal");
    return G_SOURCE_REMOVE;
}

// called from cmd_loop thread
static void do_paint_all( void ) {
    //printf("Painting all\n");
    g_idle_add(emit_paint_all,NULL);
}


static gboolean emit_paint_chr( gpointer data ) {
    //printf("Painting char\n");
    g_signal_emit_by_name(theUpdater,"update_chr-signal");
    return G_SOURCE_REMOVE;
}

// called from cmd_loop thread
static void do_paint_char( void ) {
    //printf("Painting 0x%4.4X\n",mbx->addr);
    g_idle_add(emit_paint_chr,NULL);
}


int main (int argc, char **argv) {
    int status;
    pthread_t thread_id;
    printf("Starting v540 hardware\n");

    printf("Generating character file names\n");
    for (int i=0; i<256;i++){
        sprintf(chrfile[i],"ch_%2.2X.png",i);
    }

    if (shm_connect(do_paint_all,do_paint_char)) {
        printf("Unable to connect to shared memory\n");
        exit(-1);
    }
    mbx = shm_get_mbx();
    video_mem=(uint8_t*)mbx;

    printf("Initializing Video Memory\n");
    //for (int i=0;i<sizeof(video_mem); i++) {
    //    video_mem[i]=i&0xFF;
    //}

    shm_connect(do_paint_all,do_paint_char);

    app = gtk_application_new ("org.gtk.example", 0);
    g_signal_connect (app, "activate", G_CALLBACK (act), NULL);

    theUpdater = g_object_new(UPDATER_TYPE_OBJECT,NULL);
    g_signal_connect(theUpdater,"update_all-signal",G_CALLBACK(on_paint_all),NULL);
    g_signal_connect(theUpdater,"update_chr-signal",G_CALLBACK(on_paint_char),NULL);

    status = pthread_create(&thread_id,NULL,shm_cmd_loop,NULL);
    if (status) {
        printf( "*ERROR* failed to create thread: %s\n",strerror(errno) );
        exit(-1);
    }
    status = g_application_run (G_APPLICATION (app), argc, argv);
    printf("status = %d\n",status);
    g_object_unref (app);

    return status;
}
