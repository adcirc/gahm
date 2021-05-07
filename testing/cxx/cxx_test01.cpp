
#include <iostream>
#include "Atcf.h"
#include "Gahm.h"

int main() {
  // Gahm g("test_files/bal082018.dat");
  // g.read();

  Assumptions assume;
  Atcf a("test_files/bal082018.dat", &assume);
  a.read();

  a.write("test_output.22");

  return 0;
}
