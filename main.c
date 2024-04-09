#include "noob_table.c"

int main(int argc, char *argv[]) {
  table *t = new_table();

  set_item(t, "Wow", "nice");
  set_item(t, "Cool", "awesome");
  set_item(t, "Nice", "Guy");
  delete_item(t, "Wow");

  print_items(t);
}
