int fire[2000];

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

void palette(int i) {
    if (i == 0) {
        print("\033[48;2;0;0;0m ");
        return;
    }
    if (i == 1) {
        print("\033[48;2;0;4;4m ");
        return;
    }
    if (i == 2) {
        print("\033[48;2;0;16;20m ");
        return;
    }
    if (i == 3) {
        print("\033[48;2;0;28;36m ");
        return;
    }
    if (i == 4) {
        print("\033[48;2;0;32;44m ");
        return;
    }
    if (i == 5) {
        print("\033[48;2;0;36;48m ");
        return;
    }
    if (i == 6) {
        print("\033[48;2;60;24;32m ");
        return;
    }
    if (i == 7) {
        print("\033[48;2;100;16;16m ");
        return;
    }
    if (i == 8) {
        print("\033[48;2;132;12;12m ");
        return;
    }
    if (i == 9) {
        print("\033[48;2;160;8;8m ");
        return;
    }
    if (i == 10) {
        print("\033[48;2;192;8;8m ");
        return;
    }
    if (i == 11) {
        print("\033[48;2;220;4;4m ");
        return;
    }
    if (i == 12) {
        print("\033[48;2;252;0;0m ");
        return;
    }
    if (i == 13) {
        print("\033[48;2;252;0;0m ");
        return;
    }
    if (i == 14) {
        print("\033[48;2;252;12;0m ");
        return;
    }
    if (i == 15) {
        print("\033[48;2;252;28;0m ");
        return;
    }
    if (i == 16) {
        print("\033[48;2;252;40;0m ");
        return;
    }
    if (i == 17) {
        print("\033[48;2;252;52;0m ");
        return;
    }
    if (i == 18) {
        print("\033[48;2;252;64;0m ");
        return;
    }
    if (i == 19) {
        print("\033[48;2;252;80;0m ");
        return;
    }
    if (i == 20) {
        print("\033[48;2;252;92;0m ");
        return;
    }
    if (i == 21) {
        print("\033[48;2;252;104;0m ");
        return;
    }
    if (i == 22) {
        print("\033[48;2;252;116;0m ");
        return;
    }
    if (i == 23) {
        print("\033[48;2;252;132;0m ");
        return;
    }
    if (i == 24) {
        print("\033[48;2;252;144;0m ");
        return;
    }
    if (i == 25) {
        print("\033[48;2;252;156;0m ");
        return;
    }
    if (i == 26) {
        print("\033[48;2;252;156;0m ");
        return;
    }
    if (i == 27) {
        print("\033[48;2;252;160;0m ");
        return;
    }
    if (i == 28) {
        print("\033[48;2;252;160;0m ");
        return;
    }
    if (i == 29) {
        print("\033[48;2;252;164;0m ");
        return;
    }
    if (i == 30) {
        print("\033[48;2;252;168;0m ");
        return;
    }
    if (i == 31) {
        print("\033[48;2;252;168;0m ");
        return;
    }
    if (i == 32) {
        print("\033[48;2;252;172;0m ");
        return;
    }
    if (i == 33) {
        print("\033[48;2;252;176;0m ");
        return;
    }
    if (i == 34) {
        print("\033[48;2;252;176;0m ");
        return;
    }
    if (i == 35) {
        print("\033[48;2;252;180;0m ");
        return;
    }
    if (i == 36) {
        print("\033[48;2;252;180;0m ");
        return;
    }
    if (i == 37) {
        print("\033[48;2;252;184;0m ");
        return;
    }
    if (i == 38) {
        print("\033[48;2;252;188;0m ");
        return;
    }
    if (i == 39) {
        print("\033[48;2;252;188;0m ");
        return;
    }
    if (i == 40) {
        print("\033[48;2;252;192;0m ");
        return;
    }
    if (i == 41) {
        print("\033[48;2;252;196;0m ");
        return;
    }
    if (i == 42) {
        print("\033[48;2;252;196;0m ");
        return;
    }
    if (i == 43) {
        print("\033[48;2;252;200;0m ");
        return;
    }
    if (i == 44) {
        print("\033[48;2;252;204;0m ");
        return;
    }
    if (i == 45) {
        print("\033[48;2;252;204;0m ");
        return;
    }
    if (i == 46) {
        print("\033[48;2;252;208;0m ");
        return;
    }
    if (i == 47) {
        print("\033[48;2;252;212;0m ");
        return;
    }
    if (i == 48) {
        print("\033[48;2;252;212;0m ");
        return;
    }
    if (i == 49) {
        print("\033[48;2;252;216;0m ");
        return;
    }
    if (i == 50) {
        print("\033[48;2;252;220;0m ");
        return;
    }
    if (i == 51) {
        print("\033[48;2;252;220;0m ");
        return;
    }
    if (i == 52) {
        print("\033[48;2;252;224;0m ");
        return;
    }
    if (i == 53) {
        print("\033[48;2;252;228;0m ");
        return;
    }
    if (i == 54) {
        print("\033[48;2;252;228;0m ");
        return;
    }
    if (i == 55) {
        print("\033[48;2;252;232;0m ");
        return;
    }
    if (i == 56) {
        print("\033[48;2;252;232;0m ");
        return;
    }
    if (i == 57) {
        print("\033[48;2;252;236;0m ");
        return;
    }
    if (i == 58) {
        print("\033[48;2;252;240;0m ");
        return;
    }
    if (i == 59) {
        print("\033[48;2;252;240;0m ");
        return;
    }
    if (i == 60) {
        print("\033[48;2;252;244;0m ");
        return;
    }
    if (i == 61) {
        print("\033[48;2;252;248;0m ");
        return;
    }
    if (i == 62) {
        print("\033[48;2;252;248;0m ");
        return;
    }
    if (i == 63) {
        print("\033[48;2;252;252;0m ");
        return;
    }
    if (i == 64) {
        print("\033[48;2;252;252;252m ");
        return;
    }
    if (i > 64) print("\033[48;2;252;252;252m ");
}

int main() {
    int HEIGHT;
    HEIGHT = 25;
    int WIDTH;
    WIDTH = 80;

    int i;
    int j;

    j = 0;
    while (j < HEIGHT) {
        i = 0;
        while (i < WIDTH) {
            fire[i + j * WIDTH] = 0;
            i = i + 1;
        }
        j = j + 1;
    }

    while (true) {
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
        while (i < WIDTH * HEIGHT) {  // cool
            if (rand() % 10 < 10)
                if (fire[i] > 2)
                    fire[i] = fire[i] - 2;
                else
                    fire[i] = 0;
            i = i + 1;
        }

        i = 0;
        while (i < WIDTH) {  // add heat to the bed
            int idx;
            idx = i + (HEIGHT - 1) * WIDTH;
            if (rand() % 10 == 0)
                fire[idx] = 128 + rand() % 128;  // sparks
            else if (fire[idx] < 16)
                fire[idx] = 16;
            else
                fire[idx] = fire[idx];  // ember bed
            i = i + 1;
        }

        j = 0;
        while (j < HEIGHT) {  // show the buffer
            i = 0;
            while (i < WIDTH) {
                palette(fire[i + j * WIDTH]);
                i = i + 1;
            }
            print("\033[49m\n");
            j = j + 1;
        }
    }
    return 0;
}
