#include "common.h"

#define Reg(x) ((volatile unsigned char*)(UART0 + (x)))

// see http://byterunner.com/16550.html
// For read mode
#define RHR 0 // Receiver Holding Register
#define ISR 2 // Interrupt Status Register
#define LSR 5 // Line Status Register

// For write mode
#define THR 0 // Transmitter Holding Register
#define DLL 0 // Divisor Latch (Low)
#define IER 1 // Interrupt Enable Register
#define DLM 1 // Divisor Latch (High)
#define FCR 2 // FIFO Control Register
#define LCR 3 // Line Control Register

#define LCR_ENABLE_DLAB 0x80
#define LCR_8BIT 0x03
#define FCR_ENABLE_FIFO 0x01
#define FCR_ENABLE_CLEAR 0x06
#define IER_ENABLE_RX 0x01
#define IER_ENABLE_TX 0x02
#define LSR_TX_IDLE 0x20
#define LSR_RX_READY 0x01

#define ReadReg(x) (*Reg(x))
#define WriteReg(r, x) (*(Reg(r)) = (x))

void uartinit(void) {
    // disable interrupts
    WriteReg(IER, 0x00);

    // set DLAB to access DLL and DLM
    WriteReg(LCR, LCR_ENABLE_DLAB);
    // set divisor to 3 (38.4K baud)
    WriteReg(DLL, 0x03);
    WriteReg(DLM, 0x00);
    // 8 data bits, 1 stop bit, no parity
    WriteReg(LCR, LCR_8BIT);

    // enable FIFO
    WriteReg(FCR, FCR_ENABLE_FIFO | FCR_ENABLE_CLEAR);

    // enable receive and transmit interrupts
    WriteReg(IER, IER_ENABLE_RX | IER_ENABLE_TX);
}

void uartputc_sync(char c) {
    while ((ReadReg(LSR) & LSR_TX_IDLE) == 0);
    WriteReg(THR, c);
}