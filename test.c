

int main()
{
    int a;
    int b = 2;
    a = (b * 4) + 8;
    if (a % 2 == 0)
    {
        a >>= 2;
    }

    return a;
}
