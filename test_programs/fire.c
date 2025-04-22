int main() {
    int HEIGHT;
    HEIGHT = 80;
    int WIDTH;
    WIDTH = 25;

    int palette[256];
    palette[0] = 1;

    print(palette[0] + 1);

    // while (true) {
    print("\033[2J");  // clear screen
    print("\033[H");   // home

    int i;
    i = 0;
    int j;
    while (i < HEIGHT) {  // show the buffer
        j = 0;
        while (j < WIDTH) {
            print("\033[48;2;250;250;0m ");
            j = j + 1;
        }
        print("\033[49m\n");
        i = i + 1;
    }
    // }
}