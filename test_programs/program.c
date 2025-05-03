int abs(int x) {
    if (x < 0) return (0) - (x);
    return x;
}

int sum(int x, int y) {
    int z;
    z = 3;
    while (z < 20) z = (z) + ((x) + (y));

    return z;
}

int main() {
    print(abs((0 - 10) + (sum(1, 2))));
    return 0;
}