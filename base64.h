//---------------------------------------------------------------------------
// Created by Morphixelf
// https://github.com/morphixelf/
// The MIT License
//---------------------------------------------------------------------------
/*
   base64_encode
    - Output buffer must be large enough.
    - Does not append a terminating null.
    - Returns a pointer to the character just past the last character.

   base64_decode
    - Output buffer must be large enough.
    - Does not append a terminating null.
    - Returns a pointer to the character just past the last character.

   base64_encoded_size
    - Returns the size of buffer that can receive encoded data.

   base64_decoded_size
    - Returns the size of buffer that can receive decoded data.
*/
//---------------------------------------------------------------------------
#ifndef __base64__
#define __base64__
//---------------------------------------------------------------------------
class bits_accum
{
protected:
    int acc;
    int cnt;
public:
    bits_accum()
        : acc(0)
        , cnt(0)
    {}
    int bits_count()
    {
        return cnt;
    }
    void add(int n, int bits)
    {
        acc <<= n;
        acc |= bits;
        cnt += n;
    }
    int extract(int n)
    {
        int mask = (1 << n) - 1;
        cnt -= n;
        return (acc >> cnt) & mask;
    }
    int extract_all()
    {
        int mask = (1 << cnt) - 1;
        cnt = 0;
        return acc & mask;
    }
};
//---------------------------------------------------------------------------
const char *base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
//---------------------------------------------------------------------------
const char base64_codes[] = {
                                                62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
};
//---------------------------------------------------------------------------
char * base64_encode(const char *data, int len, char *out_buf)
{
    bits_accum acc;

    while (len--)
    {
        acc.add(8,(unsigned char)*(data++));
        while (acc.bits_count() >= 6) *(out_buf++) = base64_chars[acc.extract(6)];
    }

    switch (acc.bits_count())
    {
        case 2:
            *(out_buf++) = base64_chars[acc.extract_all() << 4];
            *(out_buf++) = '=';
            *(out_buf++) = '=';
            break;
        case 4:
            *(out_buf++) = base64_chars[acc.extract_all() << 2];
            *(out_buf++) = '=';
            break;
    }

    return out_buf;
}
//---------------------------------------------------------------------------
char * base64_decode(const char *data, int len, char *out_buf)
{
    bits_accum acc;

    while (len--)
    {
        char ch = *(data++);
        if (ch < 43 || ch > 122) break;

        int code = base64_codes[ch-43];
        if (code < 0) break;

        acc.add(6,code);
        if (acc.bits_count() < 8) continue;

        *(out_buf++) = (unsigned char)acc.extract(8);
    }

    return out_buf;
}
//---------------------------------------------------------------------------
int base64_encoded_size(int len)
{
    return (len+2) / 3 * 4;
}
//---------------------------------------------------------------------------
int base64_decoded_size(int len)
{
    return (len+3) / 4 * 3;
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
