void line_blur(int offset, int step, int nsteps) {
    int circ[3];
    circ[0] = 0;
    circ[1] = fire[offset];
    circ[2] = fire[offset + step];
    int beg;
    beg = 1;
    int i;
    i = 0;
    while (i < nsteps) {
        fire[offset] = (circ[0] + circ[1] + circ[2]) / 3;
        if (i + 2 < nsteps)
            circ[(beg + 2) % 3] = fire[offset + 2 * step];
        else
            circ[(beg + 2) % 3] = 0;
        beg = beg + 1;
        offset = offset + step;
        i = i + 1;
    }
}

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