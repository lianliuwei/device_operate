#include <cstring>

int main(int argc, char** argv) {
  char* leak = new char[100];
  strcpy(leak, "deadbeef");

  //delete[] leak;

  return 0;
}