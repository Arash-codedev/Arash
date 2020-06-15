#include <stdio.h>
#include <string>
#include <stdint.h>

typedef uint32_t CRC;
using std::string;

struct Hash
{
    string name;
    CRC init;
    CRC final_xor;
    CRC polynomial;
    bool ref_in;
    bool ref_out;
};

CRC crc_hash(const string & message, const Hash & hash);

int main(void)
{
    string msg = "123456789";

    Hash crc32;

    crc32.name = "CRC-32";
    crc32.init = 0xFFFFFFFF;
    crc32.final_xor = 0xFFFFFFFF;
    crc32.polynomial = 0x04C11DB7;
    crc32.ref_in = true;
    crc32.ref_out = true;

    CRC result = crc_hash(msg, crc32);
    printf("Message: \"%s\"\n", msg.c_str());
    printf("CRC-32: 0x%X\n", result);
    
    return 0;
}

unsigned long reflect(unsigned long x, uint8_t N)
{
    unsigned long  y = 0;

    for (uint8_t i = 0; i < N; i++)
    {
        if (x & 0x01)
            y |= (1 << ((N - 1) - i));

        x = (x >> 1);
    }

    return y;
}

CRC crc_hash(const string & message, const Hash & hash)
{
    CRC remainder = hash.init;

    const int width = (8 * sizeof(CRC));
    const int top_bit = (1 << (width - 1));


    for (int i = 0; i < message.length(); ++i)
    {
        unsigned char byte;

        if(hash.ref_in)
            byte = reflect((uint8_t) message[i], 8);
        else
            byte = (unsigned char) message[i];

        remainder ^= (byte << (width - 8));

        for (unsigned char bit = 8; bit > 0; --bit)
        {
            if (remainder & top_bit)
                remainder = (remainder << 1) ^ hash.polynomial;
            else
                remainder = (remainder << 1);
        }
    }

    if(hash.ref_out)
        remainder = (CRC) reflect(remainder, width);

    return (remainder ^ hash.final_xor);
}
