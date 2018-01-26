#include "lib.h"


int length(const char s[]) {
    int i = 0;

    while (s[i] != 0)
        i++;

    return i;
}


void reverse(char s[]) {
    char c;
    int ll = length(s);
    int j = ll - 1;

    for (int i = 0; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


// itoa:  конвертируем n в символы в s
void itoa(int n, char s[]) {
    int i = 0, sign;

    if ((sign = n) < 0)  // записываем знак
        n = -n;          // делаем n положительным числом

    do {       // генерируем цифры в обратном порядке
        s[i++] = (char) (n % 10 + '0');   // берем следующую цифру
    } while ((n /= 10) > 0);             // удаляем последнюю цифру

    if (sign < 0)
        s[i++] = '-';

    s[i] = '\0';
    reverse(s);
}


/*char* ltoa(long value, char* str, unsigned char len)
{
  char sign = value > 0 ? (char)'+' : (char)'-';

  str[--len] = 0;
  while ((value != 0) && (len-1))
  {
    str[--len] = (char)('0' + (value % 10));
    value /= 10;
  }

  while (len-1) str[--len] = '0';
  str[--len]=sign;

  return str;
} //*/
