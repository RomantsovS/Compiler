int abs(int x) { return x; }

int main() {
    int x;
    if (1 < 2)
        x = 3 + 4 * 2;
    else
        x = 5;
    while (x < 10) x = x + 1;
    print(x);
}
