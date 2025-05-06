int fire[320];

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
        if ((i + 2) < nsteps)
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
    HEIGHT = 4;
    int WIDTH;
    WIDTH = 80;

    int palette[256];

    int i;
    int j;

    j = 1;

    while (j < HEIGHT) {  // scroll up
        i = 0;
        while (i < WIDTH) {
            fire[i + j * WIDTH] = 0;
            i = i + 1;
        }
        j = j + 1;
    }

    // while (true) {
    print("\033[2J");  // clear screen
    print("\033[H");   // home

    j = 1;

    while (j < HEIGHT) {  // scroll up
        i = 0;
        while (i < WIDTH) {
            fire[i + (j - 1) * WIDTH] = fire[i + j * WIDTH];
            i = i + 1;
        }
        j = j + 1;
    }

    j = 0;
    while (j < HEIGHT) {
        line_blur(j * WIDTH, 1, WIDTH);
        j = j + 1;
    }

    i = 0;
    while (i < WIDTH) {
        line_blur(i, WIDTH, HEIGHT);
        i = i + 1;
    }

    i = 0;
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
    return 0;
}