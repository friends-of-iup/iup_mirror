#include <stdio.h>
#include <stdlib.h>
#include <iup.h>

/* global variable - to be used inside the menu callbacks */
Ihandle* multitext = NULL;

char* read_file(const char* filename)
{
  int size;
  char* str;
  FILE* file = fopen(filename, "rb");
  if (!file) 
  {
    IupMessagef("Error", "Can't open file: %s", filename);
    return NULL;
  }

  /* calculate file size */
  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);

  /* allocate memory for the file contents + nul terminator */
  str = malloc(size + 1);
  /* read all data at once */
  fread(str, size, 1, file);
  /* set the nul terminator */
  str[size] = 0;

  fclose(file);
  return str;
}

void write_file(const char* filename, const char* str, int count)
{
  FILE* file = fopen(filename, "w");
  if (!file) 
  {
    IupMessagef("Error", "Can't open file: %s", filename);
    return;
  }

  fwrite(str, 1, count, file);
  fclose(file);
}

int open_cb(void)
{
  Ihandle *filedlg = IupFileDlg();
  IupSetAttribute(filedlg, "DIALOGTYPE", "OPEN");
  IupSetAttribute(filedlg, "FILTER", "*.txt");
  IupSetAttribute(filedlg, "FILTERINFO", "Text Files");

  IupPopup(filedlg, IUP_CENTER, IUP_CENTER);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char* filename = IupGetAttribute(filedlg, "VALUE");
    char* str = read_file(filename);
    if (str)
    {
      IupSetStrAttribute(multitext, "VALUE", str);
      free(str);
    }
  }

  IupDestroy(filedlg);
  return IUP_DEFAULT;
}

int save_cb(void)
{
  Ihandle *filedlg = IupFileDlg();
  IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");
  IupSetAttribute(filedlg, "FILTER", "*.txt");
  IupSetAttribute(filedlg, "FILTERINFO", "Text Files");

  IupPopup(filedlg, IUP_CENTER, IUP_CENTER);

  if (IupGetInt(filedlg, "STATUS") != -1)
  {
    char* filename = IupGetAttribute(filedlg, "VALUE");
    char* str = IupGetAttribute(multitext, "VALUE");
    int count = IupGetInt(multitext, "COUNT");
    write_file(filename, str, count);
  }

  IupDestroy(filedlg);
  return IUP_DEFAULT;
}

int font_cb(void)
{
  Ihandle* fontdlg = IupFontDlg();
  char* font = IupGetAttribute(multitext, "FONT");
  IupSetStrAttribute(fontdlg, "VALUE", font);
  IupSetAttribute(fontdlg, "TITLE", "IupFontDlg Test");

  IupPopup(fontdlg, IUP_CENTER, IUP_CENTER);

  if (IupGetInt(fontdlg, "STATUS") == 1)
  {
    char* font = IupGetAttribute(fontdlg, "VALUE");
    IupSetStrAttribute(multitext, "FONT", font);
  }

  IupDestroy(fontdlg);
  return IUP_DEFAULT;
}

int about_cb(void) 
{
  IupMessage("About", "   IUP Tutorial\n\nAutors:\n   Gustavo Lyrio\n   Antonio Scuri");
  return IUP_DEFAULT;
}

int exit_cb(void)
{
  return IUP_CLOSE;
}

int main(int argc, char **argv)
{
  Ihandle *dlg, *vbox;
  Ihandle *file_menu, *item_exit, *item_open, *item_save;
  Ihandle *format_menu, *item_font;
  Ihandle *help_menu, *item_about;
  Ihandle *sub_menu_file, *sub_menu_format, *sub_menu_help, *menu;

  IupOpen(&argc, &argv);

  multitext = IupText(NULL);
  IupSetAttribute(multitext, "MULTILINE", "YES");
  IupSetAttribute(multitext, "EXPAND", "YES");

  item_open = IupItem("Open...", NULL);
  item_save = IupItem("Save...", NULL);
  item_exit = IupItem("Exit", NULL);
  item_font = IupItem("Font...", NULL);
  item_about = IupItem("About...", NULL);

  IupSetCallback(item_exit, "ACTION", (Icallback)exit_cb);
  IupSetCallback(item_open, "ACTION", (Icallback)open_cb);
  IupSetCallback(item_save, "ACTION", (Icallback)save_cb);
  IupSetCallback(item_font, "ACTION", (Icallback)font_cb);
  IupSetCallback(item_about, "ACTION", (Icallback)about_cb);

  file_menu = IupMenu(
    item_open,
    item_save,
    IupSeparator(),
    item_exit,
    NULL);
  format_menu = IupMenu(
    item_font,
    NULL);
  help_menu = IupMenu(
    item_about,
    NULL);

  sub_menu_file = IupSubmenu("File", file_menu);
  sub_menu_format = IupSubmenu("Format", format_menu);
  sub_menu_help = IupSubmenu("Help", help_menu);

  menu = IupMenu(
    sub_menu_file, 
    sub_menu_format, 
    sub_menu_help, 
    NULL);

  vbox = IupVbox(
    multitext,
    NULL);

  dlg = IupDialog(vbox);
  IupSetAttributeHandle(dlg, "MENU", menu);
  IupSetAttribute(dlg, "TITLE", "Simple Notepad");
  IupSetAttribute(dlg, "SIZE", "QUARTERxQUARTER");

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
  IupSetAttribute(dlg, "USERSIZE", NULL);

  IupMainLoop();

  IupClose();
  return EXIT_SUCCESS;
}
