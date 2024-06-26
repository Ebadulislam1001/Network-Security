#include <bits/stdc++.h>
using namespace std;

#define plainTextSize 64
#define cipherKeySize 64
#define cipherTextSize 64
#define rounds 16

void show(vector<int> text, int indentation);

// Transformations
vector<int> P_box(vector<int> text, vector<int> P_key);
pair<vector<int>, vector<int>> split(vector<int> text);
vector<int> circularShift(vector<int> text, int k);
vector<int> combine(vector<int> left, vector<int> right);
vector<int> XOR(vector<int> left, vector<int> right);
vector<int> swap(vector<int> text);

// Components
vector<vector<int>> roundKeyGenerator(vector<int> cipherKey);
vector<int> DES_function(vector<int> right, vector<int> roundKey);
vector<int> DES_S_boxes(vector<int> text);

int main()
{
    // initiating a filePointer to take input from the txt file
    FILE *readPointer = fopen("DES.txt", "r");
    // reading plainText from the txt file
    vector<int> plainText;
    for (int i = 0; i < plainTextSize; i++)
    {
        int p;
        fscanf(readPointer, "%d%*c", &p);
        plainText.push_back(p);
    }
    // reading cipherKey from the txt file
    vector<int> cipherKey;
    for (int i = 0; i < cipherKeySize; i++)
    {
        int k;
        fscanf(readPointer, "%d%*c", &k);
        cipherKey.push_back(k);
    }
    fclose(readPointer);

    // printing the plain text
    printf("\nPlain text:\n");
    show(plainText, 0);
    // printing the cipher key
    printf("\nCipher Key:\n");
    show(cipherKey, 0);

    // calculating roundKeys using Round Key Generator
    vector<vector<int>> roundKeys = roundKeyGenerator(cipherKey);

    // applying initial permutation
    vector<int> initPermutation = {
        58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7};
    vector<int> temp = P_box(plainText, initPermutation);

    // printing after initial permutation
    printf("\nInitial Permutation:\n");
    show(temp, 0);

    // applying DES rounds
    for (int r = 0; r < rounds; r++)
    {
        // applying split
        pair<vector<int>, vector<int>> halves = split(temp);
        vector<int> left = halves.first;
        vector<int> right = halves.second;

        // applying mixer
        left = XOR(left, DES_function(right, roundKeys[r]));
        printf("\n\t\t\t\tXOR with left result\n");
        show(left, 4);

        // applying swapper
        temp = combine(right, left);

        // printing after each round
        printf("\nAfter round %02d:\n", r + 1);
        show(temp, 0);
    }
    // final counter-swap and
    temp = swap(temp);

    // applying final permutation
    vector<int> finalPermutation = {
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25};
    vector<int> cipherText = P_box(temp, finalPermutation);

    // printing after final permutation
    printf("\nFinal Permutation:\n");
    show(cipherText, 0);
    printf("\nThis is the Cipher Text\n");
    return 0;
}

void show(vector<int> text, int indentation = 0)
{
    vector<int> hexaMap = {'0','1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    for (int i = 0; (8 * i) < text.size(); i++)
    {
        for (int d = 0; d < indentation; d++)
        {
            printf("\t");
        }
        int MSN = 0, LSN = 0;
        for (int j = 0; j < 4 && 8 * i + j < text.size(); j++)
        {
            int bit = text[8 * i + j];
            printf("%d ", bit);
            if(bit == 1)
            {
                MSN += pow(2, 3-j);
            }
        }
        printf("  ");
        for (int j = 4; j < 8 && 8 * i + j < text.size(); j++)
        {
            int bit = text[8 * i + j];
            printf("%d ", bit);
            if(bit == 1)
            {
                LSN += pow(2, 7-j);
            }
        }
        printf("  ");
        printf("%c%c", hexaMap[MSN], hexaMap[LSN]);
        printf("\n");
    }
}

vector<vector<int>> roundKeyGenerator(vector<int> cipherKey)
{
    // applying parity drop and P_box
    vector<int> parityDropPermutation = {
        57, 49, 41, 33, 25, 17, 9, 1,
        58, 50, 42, 34, 26, 18, 10, 2,
        59, 51, 43, 35, 27, 19, 11, 3,
        60, 52, 44, 36, 63, 55, 47, 39,
        31, 23, 15, 7, 62, 54, 46, 38,
        30, 22, 14, 6, 61, 53, 45, 37,
        29, 21, 13, 5, 28, 20, 12, 4};
    cipherKey = P_box(cipherKey, parityDropPermutation);

    vector<vector<int>> roundKeys(rounds);
    // generating round keys
    for (int r = 1; r <= rounds; r++)
    {
        // applying split
        pair<vector<int>, vector<int>> halves = split(cipherKey);
        vector<int> left = halves.first;
        vector<int> right = halves.second;

        // apllying left rotate
        int k = 2;
        if (r == 1 || r == 2 || r == 9 || r == 16)
        {
            k = 1;
        }
        left = circularShift(left, k);
        right = circularShift(right, k);

        // applying combine
        cipherKey = combine(left, right);

        // applying key compression P-box
        vector<int> keyCompressionPermutation = {
            14, 17, 11, 24, 1, 5, 3, 28,
            15, 6, 21, 10, 23, 19, 12, 4,
            26, 8, 16, 7, 27, 20, 13, 2,
            41, 52, 31, 37, 47, 55, 30, 40,
            51, 45, 33, 48, 44, 49, 39, 56,
            34, 53, 46, 42, 50, 36, 29, 32};
        roundKeys[r - 1] = P_box(cipherKey, keyCompressionPermutation);

        // printing round keys
        printf("\n\t\t\t\tRound key %02d:\n", r);
        show(roundKeys[r - 1], 4);
    }
    return roundKeys;
}

vector<int> P_box(vector<int> text, vector<int> P_key)
{
    vector<int> res;
    for (int i = 0; i < P_key.size(); i++)
    {
        res.push_back(text[P_key[i] - 1]);
    }
    return res;
}

pair<vector<int>, vector<int>> split(vector<int> text)
{
    vector<int> left, right;
    int size = text.size();
    int splitSize = size / 2;
    for (int i = 0; i < splitSize; i++)
    {
        left.push_back(text[i]);
        right.push_back(text[i + splitSize]);
    }
    return {left, right};
}

vector<int> circularShift(vector<int> text, int k)
{
    k %= text.size();
    vector<int> result;
    for (int i = k; i < text.size(); i++)
    {
        result.push_back(text[i]);
    }
    for (int i = 0; i < k; i++)
    {
        result.push_back(text[i]);
    }
    return result;
}

vector<int> combine(vector<int> left, vector<int> right)
{
    vector<int> result = left;
    for (int i = 0; i < right.size(); i++)
    {
        result.push_back(right[i]);
    }
    return result;
}

vector<int> XOR(vector<int> left, vector<int> right)
{
    if (left.size() != right.size())
    {
        printf("\nWARNING!!\n");
    }
    for (int i = 0; i < left.size(); i++)
    {
        if (right[i] == 1)
        {
            left[i] = 1 - left[i];
        }
    }
    return left;
}

vector<int> swap(vector<int> text)
{
    int size = text.size();
    int swapSize = size / 2;
    return circularShift(text, swapSize);
}

vector<int> DES_function(vector<int> right, vector<int> roundKey)
{
    // applying expansion P-box
    vector<int> expansionPermutation = {
        32, 1, 2, 3, 4, 5,
        4, 5, 6, 7, 8, 9,
        8, 9, 10, 11, 12, 13,
        12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21,
        20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29,
        28, 29, 30, 31, 32, 1};
    right = P_box(right, expansionPermutation);
    printf("\n\t\t\t\texpansion P-box result\n");
    show(right, 4);

    // applying XOR with the roundKey
    right = XOR(right, roundKey);
    printf("\n\t\t\t\tadd roundKey result\n");
    show(right, 4);

    // applying DES S-boxes for compression
    right = DES_S_boxes(right);
    printf("\n\t\t\t\tS-box result\n");
    show(right, 4);

    // applying straight P-box
    vector<int> DES_straightPermutation = {
        16, 7, 20, 21, 29, 12, 28, 17,
        1, 15, 23, 26, 5, 18, 31, 10,
        2, 8, 24, 14, 32, 27, 3, 9,
        19, 13, 30, 6, 22, 11, 4, 25};
    right = P_box(right, DES_straightPermutation);
    printf("\n\t\t\t\tStraight P-box result\n");
    show(right, 4);

    return right;
}

vector<int> DES_S_boxes(vector<int> text)
{
    vector<int> result;
    vector<vector<vector<int>>> S_boxes = {
        {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
         {0, 15, 7, 4, 14, 2, 13, 10, 3, 6, 12, 11, 9, 5, 3, 8},
         {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
         {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},

        {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
         {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
         {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
         {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},

        {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
         {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
         {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
         {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},

        {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
         {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
         {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
         {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},

        {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
         {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
         {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
         {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},

        {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
         {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
         {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
         {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 10, 0, 8, 13}},

        {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
         {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
         {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
         {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},

        {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
         {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 10, 14, 9, 2},
         {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 10, 15, 3, 5, 8},
         {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 9, 3, 5, 6, 11}}};

    for (int b = 0; b < 8; b++)
    {
        // b is for the box number, and every box takes 6 bits as input
        int offset = 6 * b;
        int rowNo = (2 * text[offset]) + text[offset + 5];
        int colNo = (8 * text[offset + 1]) + (4 * text[offset + 2]) + (2 * text[offset + 3]) + text[offset + 4];
        int temp = S_boxes[b][rowNo][colNo];

        vector<int> subWord = {0, 0, 0, 0};
        if (temp >= 8)
        {
            temp = temp - 8;
            subWord[0] = 1;
        }
        if (temp >= 4)
        {
            temp = temp - 4;
            subWord[1] = 1;
        }
        if (temp >= 2)
        {
            temp = temp - 2;
            subWord[2] = 1;
        }
        if (temp >= 1)
        {
            temp = temp - 1;
            subWord[3] = 1;
        }

        result = combine(result, subWord);
    }
    return result;
}
