#include <iostream>
#include <iomanip>
#include <string>
#include <sstream> // Include this header


// Constants for MD5 algorithm
constexpr uint32_t MD5_PADDING[64] = {
    0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476
};

constexpr uint32_t MD5_SINE_TABLE[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};


// MD5 transformation functions
#define F(X, Y, Z) (((X) & (Y)) | ((~X) & (Z)))
#define G(X, Y, Z) (((X) & (Z)) | ((Y) & (~Z)))
#define H(X, Y, Z) ((X) ^ (Y) ^ (Z))
#define I(X, Y, Z) ((Y) ^ ((X) | (~Z)))

// Left-rotate operation
#define LEFT_ROTATE(X, N) (((X) << (N)) | ((X) >> (32 - (N))))

// MD5 main algorithm
void md5_main_algorithm(uint32_t& A, uint32_t& B, uint32_t& C, uint32_t& D, uint32_t M, uint32_t* chunk) {
    uint32_t F = 0, g = 0;

    if (M >= 0 && M <= 15) {
        F = F(B, C, D);
        g = M;
    }
    else if (M >= 16 && M <= 31) {
        F = G(B, C, D);
        g = (5 * M + 1) % 16;
    }
    else if (M >= 32 && M <= 47) {
        F = H(B, C, D);
        g = (3 * M + 5) % 16;
    }
    else if (M >= 48 && M <= 63) {
        F = I(B, C, D);
        g = (7 * M) % 16;
    }

    uint32_t temp = D;
    D = C;
    C = B;
    B = B + LEFT_ROTATE((A + F + MD5_SINE_TABLE[M] + chunk[g]), MD5_SINE_TABLE[M]);
    A = temp;
}

// NOTE: this is not real MD5, it adds spaces to the end of the message until it is divisible by 64. MD5 padding works differently. But as hash for our purposes works fine.
std::string padInput(const std::string& input) {
    std::string paddedInput = input;
    size_t originalLength = input.length();
    size_t newLength = (originalLength + 63) / 64 * 64;

    if (newLength > originalLength) {
        paddedInput.resize(newLength, ' ');
    }

    return paddedInput;
}

// Calculate the MD5 hash of a string
// NOTE: this is not real MD5, it adds spaces to the end of the message until it is divisible by 64. MD5 padding works differently. But as hash for our purposes works fine.
std::string getMD5(const std::string& inputI) {
    std::string input = padInput(inputI);
    uint32_t A = MD5_PADDING[0];
    uint32_t B = MD5_PADDING[1];
    uint32_t C = MD5_PADDING[2];
    uint32_t D = MD5_PADDING[3];

    // Process the input string
    for (size_t i = 0; i < input.length(); i += 64) {
        uint32_t chunk[16] = { 0 };

        // Break the input into 512-bit chunks
        for (size_t j = 0; j < 64 && i + j < input.length(); j += 4) {
            chunk[j / 4] = (static_cast<uint32_t>(input[i + j]) & 0xFF) |
                ((static_cast<uint32_t>(input[i + j + 1]) & 0xFF) << 8) |
                ((static_cast<uint32_t>(input[i + j + 2]) & 0xFF) << 16) |
                ((static_cast<uint32_t>(input[i + j + 3]) & 0xFF) << 24);
        }

        for (int j = 0; j < 64; j += 4) {
            md5_main_algorithm(A, B, C, D, j / 4, chunk);
        }
    }

    // Convert the result to a hexadecimal string
    std::ostringstream md5_stream;
    md5_stream << std::hex << std::setfill('0')
        << std::setw(8) << A
        << std::setw(8) << B
        << std::setw(8) << C
        << std::setw(8) << D;

    return md5_stream.str();
}

