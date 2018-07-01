#include <gtk/gtk.h>
#include <gio/gio.h>
#include <fstream>
#include <string>
#define MESSAGE "Welcome\n"
#define CONSUC "Connected.."
#define RECORDING "Recording.."
#define DONE "Finished Recording.."
#define NOTCONSUC "Lost Connection to Client.."
using namespace std;

int counter = 0;
GOutputStream  *sig;
GtkWidget *bar;
GtkWidget *recBar;
GtkWidget *labelDist;
GtkWidget *labelSpeed;
GtkWidget *labelCommand;
gint barId;
gint recId;


static gboolean handler(GThreadedSocketService *service,
                        GSocketConnection *connection,
                        GSocketListener *listener,
                        GtkLabel *labelMode,
                        gpointer user_data)
{
    GOutputStream *out;
    GInputStream *in;
    char buffer[1024];
    gssize size;
    char *modeStr;
    char *speedStr;
    char *distStr;
    char *commandStr;
    char *str;
    gtk_statusbar_pop(GTK_STATUSBAR(bar), barId); 
    gtk_statusbar_push(GTK_STATUSBAR(bar), barId, CONSUC);
   
    out = g_io_stream_get_output_stream(G_IO_STREAM(connection));
    in = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    sig = out;
   
    g_output_stream_write_all(out, MESSAGE, strlen(MESSAGE), NULL, NULL,
                             NULL);
    while(0 < (size = g_input_stream_read(in, buffer, sizeof buffer, NULL,
                                        NULL)))
    {
        str = g_strdup_printf("%s", buffer);
        modeStr = strtok (str, ";");
        distStr = strtok (NULL, ";");
        speedStr = strtok (NULL, ";");
        commandStr = strtok(NULL, ";");

        for(int i = 0; i < (sizeof buffer); ++i) {
             buffer[i] = 0;
        }
        gtk_label_set_text(labelMode, modeStr);
        gtk_label_set_text(GTK_LABEL(labelDist), distStr);
        gtk_label_set_text(GTK_LABEL(labelSpeed), speedStr);
    }
    gtk_statusbar_pop(GTK_STATUSBAR(bar), barId);
    gtk_statusbar_push(GTK_STATUSBAR(bar), barId, NOTCONSUC);
    return TRUE;
}
static void sendCom(char *com)
{
    g_output_stream_write_all(sig, com, strlen(com), NULL, NULL,
                             NULL);
}
static void startSpeech(GtkWidget *widget, gpointer user_data) 
{
  ifstream inFile;
  char *com;
  std::string st;
  std::string exec = "python3 sound_recorder.py";
  
  system(exec.c_str());
  
  
  exec = "python3 speech.py";
  system(exec.c_str());
  
  inFile.open("result.txt");
  inFile >> st;
  inFile.close();
  com = g_strdup_printf("%s", st.c_str());
  gtk_label_set_text(GTK_LABEL(labelCommand), com);

  if(st == "go") st = "COMFORWARD";
  else if (st == "stop") st = "COMSTOP";
  else if (st == "right") st = "COMRIGHT";
  else if (st == "left") st = "COMLEFT";
  else if (st == "back") st = "COMBACKWARD";
  else return;

  com = g_strdup_printf("%s", st.c_str());
  sendCom(com);

}
static void sendSig(GtkWidget *widget, gpointer user_data)
{
    const char *mes = gtk_widget_get_name(widget);
    g_output_stream_write_all(sig, mes, strlen(mes), NULL, NULL,
                             NULL);
}


static void sendStopSig(GtkWidget *widget, gpointer user_data)
{
    g_output_stream_write_all(sig, "STOP", strlen("STOP"), NULL, NULL,
                             NULL);
}

static void activate (GtkApplication* app, gpointer user_data)
{
    GSocketService *service;
    GOptionContext *context;
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *labelMode;
    GtkWidget *verSp = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    GtkWidget *herSp = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    GtkWidget *space = gtk_label_new(" ");
    GtkWidget *mo = gtk_label_new("Mode:");
    GtkWidget *mode1 = gtk_label_new("Mode 1 Controls:");
    GtkWidget *mode2 = gtk_label_new("Mode 2 Controls:");

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW (window), "Speech Command");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
    
    gtk_container_add(GTK_CONTAINER(window), grid);

    label = gtk_label_new("        Mode: ");
    gtk_grid_attach(GTK_GRID(grid), label, 7, 0, 3, 1);
    gtk_widget_set_halign(label, GTK_ALIGN_START);

    label = gtk_label_new("        Speed: ");
    gtk_grid_attach(GTK_GRID(grid), label, 7, 1, 3, 1);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    
    label = gtk_label_new("        Distance: ");
    gtk_grid_attach(GTK_GRID(grid), label, 7, 2, 3, 1);
    gtk_widget_set_halign(label, GTK_ALIGN_START);

    label = gtk_label_new("        Command: ");
    gtk_grid_attach(GTK_GRID(grid), label, 7, 4, 3, 1);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
   
    labelMode = gtk_label_new("0");
    gtk_grid_attach(GTK_GRID(grid), labelMode, 11, 0, 3, 1);
    gtk_widget_set_halign(labelMode, GTK_ALIGN_START);

    labelDist = gtk_label_new("0");
    gtk_grid_attach(GTK_GRID(grid), labelDist, 11, 2, 3, 2);
    gtk_widget_set_halign(labelDist, GTK_ALIGN_START);

    labelSpeed = gtk_label_new("0");
    gtk_grid_attach(GTK_GRID(grid), labelSpeed, 11, 1, 3, 1);
    gtk_widget_set_halign(labelSpeed, GTK_ALIGN_START);

    labelCommand = gtk_label_new("0");
    gtk_grid_attach(GTK_GRID(grid), labelCommand, 11, 4, 3, 1);
    gtk_widget_set_halign(labelCommand, GTK_ALIGN_START);

    bar = gtk_statusbar_new();
    barId = gtk_statusbar_get_context_id(GTK_STATUSBAR(bar),
                                         "Connection Status");
    gtk_statusbar_push(GTK_STATUSBAR(bar), barId,
                       "Waiting for Connection....");
    
    gtk_grid_attach(GTK_GRID(grid), bar, 7, 6, 6, 2);
    recId = gtk_statusbar_get_context_id(GTK_STATUSBAR(bar), "recording stat");
    
    service = g_threaded_socket_service_new(10);
    g_socket_listener_add_inet_port(G_SOCKET_LISTENER(service),
            7777, NULL, NULL);
    g_print("Service is listening on port 7777\n");
    g_signal_connect(service, "run", G_CALLBACK(handler),labelMode);
    
    button = gtk_button_new_with_label("L");
    gtk_widget_set_name(button, "LEFT");
    g_signal_connect(button, "pressed", G_CALLBACK(sendSig), label);
    g_signal_connect(button, "released", G_CALLBACK(sendStopSig), label);

    gtk_grid_attach(GTK_GRID(grid), button, 1, 1, 1, 1);

    button = gtk_button_new_with_label("R");
    gtk_widget_set_name(button, "RIGHT");
    g_signal_connect(button, "pressed", G_CALLBACK(sendSig), labelSpeed);
    g_signal_connect(button, "released", G_CALLBACK(sendStopSig), label);

    gtk_grid_attach(GTK_GRID(grid), button, 3, 1, 1, 1);

    button = gtk_button_new_with_label("F");
    gtk_widget_set_name(button, "FORWARD");
    g_signal_connect(button, "pressed", G_CALLBACK(sendSig), labelSpeed);
    g_signal_connect(button, "released", G_CALLBACK(sendStopSig), label);

    gtk_grid_attach(GTK_GRID(grid), button, 2, 0, 1, 1);
    
    button = gtk_button_new_with_label("Command");
    g_signal_connect(button, "pressed", G_CALLBACK(startSpeech), labelSpeed);

    gtk_grid_attach(GTK_GRID(grid), button, 0, 5, 2, 1);

    button = gtk_button_new_with_label("Stop");
    gtk_widget_set_name(button, "COMSTOP");
    g_signal_connect(button, "pressed", G_CALLBACK(sendSig), labelSpeed);

    gtk_grid_attach(GTK_GRID(grid), button, 2, 5, 2, 1);
    
    button = gtk_button_new_with_label("Start");
    gtk_widget_set_name(button, "STARTAUTO");
    g_signal_connect(button, "pressed", G_CALLBACK(sendSig), labelSpeed);

    gtk_grid_attach(GTK_GRID(grid), button, 0, 3, 2, 1);

    button = gtk_button_new_with_label("Stop");
    gtk_widget_set_name(button, "STOPAUTO");
    g_signal_connect(button, "pressed", G_CALLBACK(sendSig), labelSpeed);

    gtk_grid_attach(GTK_GRID(grid), button, 2, 3, 2, 1);

    button = gtk_button_new_with_label("B");
    gtk_widget_set_name(button, "BACKWARD");
    g_signal_connect(button, "pressed", G_CALLBACK(sendSig), labelSpeed);
    g_signal_connect(button, "released", G_CALLBACK(sendStopSig), label);

    gtk_grid_attach(GTK_GRID(grid), button, 2, 1, 1, 1);
    
    button = gtk_button_new_with_label("M1");
    gtk_widget_set_name(button, "MODE1");
    g_signal_connect(button, "pressed", G_CALLBACK(sendSig), label);

    gtk_grid_attach(GTK_GRID(grid), button, 0, 7, 1, 1);

    button = gtk_button_new_with_label("M2");
    gtk_widget_set_name(button, "MODE2");
    g_signal_connect(button, "pressed", G_CALLBACK(sendSig), labelSpeed);

    gtk_grid_attach(GTK_GRID(grid), button, 1, 7, 1, 1);

    button = gtk_button_new_with_label("M3");
    gtk_widget_set_name(button, "MODE3");
    g_signal_connect(button, "pressed", G_CALLBACK(sendSig), labelSpeed);

    gtk_grid_attach(GTK_GRID(grid), button, 2, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mo, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mode1, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), mode2, 0, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), space, 0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), verSp, 6, 0, 1, 7);
    //gtk_grid_attach(GTK_GRID(grid), herSp, 3, 5, 1, 2);

    button = gtk_button_new_with_label("Q");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy),
                            window);

    gtk_grid_attach(GTK_GRID(grid), button, 0, 9, 1, 2);
    

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) 
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("amin.org", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run (G_APPLICATION(app), argc, argv);
    g_object_unref (app);
    return status;
}
