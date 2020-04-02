#ifndef __CAM_OS_UTIL_H__
#define __CAM_OS_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CAM_OS_CONTAINER_OF(ptr, type, member) ({          \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#ifndef likely
#define CAM_OS_LIKELY(x) __builtin_expect(!!(x), 1)
#else
#define CAM_OS_LIKELY(x) likely(x)
#endif

#ifndef unlikely
#define CAM_OS_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define CAM_OS_UNLIKELY(x) unlikely(x)
#endif

static __always_inline int32_t CAM_OS_FLS(int32_t x)
{
    int r = 32;

    if (!x)
        return 0;
    if (!(x & 0xffff0000u)) {
        x <<= 16;
        r -= 16;
    }
    if (!(x & 0xff000000u)) {
        x <<= 8;
        r -= 8;
    }
    if (!(x & 0xf0000000u)) {
        x <<= 4;
        r -= 4;
    }
    if (!(x & 0xc0000000u)) {
        x <<= 2;
        r -= 2;
    }
    if (!(x & 0x80000000u)) {
        x <<= 1;
        r -= 1;
    }
    return r;
}

#if CAM_OS_BITS_PER_LONG == 32
static __always_inline int32_t CAM_OS_FLS64(uint64_t x)
{
    uint32_t h = x >> 32;
    if (h)
        return CAM_OS_FLS(h) + 32;
    return CAM_OS_FLS(x);
}
#elif CAM_OS_BITS_PER_LONG == 64
static __always_inline int32_t _CAM_OS_FLS(uint64_t word)
{
    int32_t num = CAM_OS_BITS_PER_LONG - 1;

//#if CAM_OS_BITS_PER_LONG == 64
    if (!(word & (~0ul << 32))) {
        num -= 32;
        word <<= 32;
    }
//#endif
    if (!(word & (~0ul << (CAM_OS_BITS_PER_LONG-16)))) {
        num -= 16;
        word <<= 16;
    }
    if (!(word & (~0ul << (CAM_OS_BITS_PER_LONG-8)))) {
        num -= 8;
        word <<= 8;
    }
    if (!(word & (~0ul << (CAM_OS_BITS_PER_LONG-4)))) {
        num -= 4;
        word <<= 4;
    }
    if (!(word & (~0ul << (CAM_OS_BITS_PER_LONG-2)))) {
        num -= 2;
        word <<= 2;
    }
    if (!(word & (~0ul << (CAM_OS_BITS_PER_LONG-1))))
        num -= 1;
    return num;
}

static __always_inline int32_t CAM_OS_FLS64(uint64_t x)
{
    if (x == 0)
        return 0;
    return _CAM_OS_FLS(x) + 1;
}
#else
#error CAM_OS_BITS_PER_LONG not 32 or 64
#endif

#define CAM_OS_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__CAM_OS_UTIL_H__