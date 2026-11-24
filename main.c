#include <stdint.h>
#define REG(a) (*(volatile uint32_t *)(a))
#define GPIOA 0x40020000u
#define GPIOC 0x40020800u
#define USART1 0x40011000u

static void init(void) {
    REG(0x40023830) |= 5;                   
    (void)REG(0x40023830);
    REG(GPIOC+0x18) = 1u<<13;             
    REG(GPIOC) = (REG(GPIOC) & ~(3u<<26)) | (1u<<26);
    REG(GPIOA+0x0c) = (REG(GPIOA+0x0c) & ~3u) | 1u; 
    REG(0xe000e014) = 15999;             
    REG(0xe000e018) = 0;
    REG(0xe000e010) = 5;
}
static unsigned tick(void) { return (REG(0xe000e010)>>16) & 1; }
static unsigned pressed(void) { return (REG(GPIOA+0x10)&1) == 0; }
static void led(unsigned on) { REG(GPIOC+0x18) = 1u<<(on ? 29 : 13); }
int main(void);
void Reset_Handler(void) { main(); for (;;) {} }
void Default_Handler(void) { for (;;) {} }
__attribute__((used,section(".vectors")))
const uintptr_t vectors[16] = {0x20020000u, (uintptr_t)Reset_Handler,
    (uintptr_t)Default_Handler,(uintptr_t)Default_Handler,(uintptr_t)Default_Handler,
    (uintptr_t)Default_Handler,(uintptr_t)Default_Handler};
int main(void) {
    init();
    const unsigned intervals[] = {137,287,463,709};
    unsigned elapsed = 0, stable = 0, count = 0, on = 0, index = 0, ms = 0;
    for (;;) {
        if (!tick()) continue;
        ++elapsed;
        ++ms;
        unsigned button = pressed();
        count = button == stable ? 0 : count+1;
        if (count == 25) {
            stable = button;
            count = 0;
            if (stable) {
                unsigned step = 1 + ms%3;
                index = (index+step)%4;
                elapsed = 0;
            }
        }
        if (elapsed >= intervals[index]) {
            led(on ^= 1);
            elapsed = 0;
        }
    }
}
