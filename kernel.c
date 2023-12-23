

void main()
{
    *(char*)0xb8000 = 'Q';
    *(char*)0xb8002 = 'A';
    return;
}