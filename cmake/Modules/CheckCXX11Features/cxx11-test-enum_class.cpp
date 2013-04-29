enum class Color {
    RED, GREEN, BLUE
};

int main() {
    Color color = Color::RED;
    bool test =
        (color == Color::RED) &&
        (color != Color::GREEN) &&
        (color != Color::BLUE);
    return test ? 0 : 1;
}
