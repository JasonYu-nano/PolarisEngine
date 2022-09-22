#include <array>

// https://learn.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=msvc-170
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#ifdef _WIN32
#include <intrin.h>
#include <bitset>

#define cpuid(info, x)    __cpuidex(info, x, 0)
#else
#include <cpuid.h>

void cpuid(int info[4], int InfoType)
{
    __cpuid_count(InfoType, 0, info[0], info[1], info[2], info[3]);
}
#endif

struct CpuInfo
{
    int operator[] (int idx) { return Data[idx]; }

    int Data[4];
};

void DetectFeature()
{
    CpuInfo cpuInfo {};
    cpuid(cpuInfo.Data, 1);

    int num = cpuInfo[0];

    CpuInfo* data = new CpuInfo[num];
    for (int i = 0; i <= num; ++i)
    {
        cpuid(cpuInfo.Data, i);
        data[i] = cpuInfo;
    }

    // Capture vendor string
    char vendor[0x20];
    memset(vendor, 0, sizeof(vendor));
    *reinterpret_cast<int*>(vendor) = data[0][1];
    *reinterpret_cast<int*>(vendor + 4) = data[0][3];
    *reinterpret_cast<int*>(vendor + 8) = data[0][2];

    if (strcmp(vendor, "GenuineIntel") == 0)
    {
        printf("Vendor=Intel");
    }
    else if (strcmp(vendor, "AuthenticAMD") == 0)
    {
        printf("Vendor=AMD");
    }

    // load bitset with flags for function 0x00000001
    if (num >= 1)
    {
        std::bitset<32> f_1_ECX = data[1][2];
        std::bitset<32> f_1_EDX = data[1][3];

        if (f_1_EDX[25])
        {
            printf("SSE=TRUE");
        }

        if (f_1_EDX[26])
        {
            printf("SSE2=TRUE");
        }

        if (f_1_ECX[0])
        {
            printf("SSE3=TRUE");
        }

        if (f_1_ECX[9])
        {
            printf("SSSE3=TRUE");
        }

        if (f_1_ECX[19])
        {
            printf("SSE41=TRUE");
        }

        if (f_1_ECX[20])
        {
            printf("SSE42=TRUE");
        }
    }
}

#else
#error ""No cpuid intrinsic defined for processor architecture"

void DetectFeature()
{

}
#endif

int main(int argv, char** args)
{
    DetectFeature();

    return 0;
}