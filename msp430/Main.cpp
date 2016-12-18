// Copyright (c) Tyler Veness 2016. All Rights Reserved.

#include <msp430g2553.h>
#include <stdint.h>

constexpr uint8_t kDelayCount = 5;

int main() {
  // Stop watchdog
  WDTCTL = WDTPW + WDTHOLD;

  // Configure P2.0, P2.1, P2.4, and P2.5 for GPIO
  P2SEL &= ~(BIT0 | BIT1 | BIT4 | BIT5);

  P2DIR |= BIT0;   // P2.0 = output for ISO2
  P2DIR &= ~BIT1;  // P2.1 = input for ISO2
  P2DIR &= ~BIT4;  // P2.4 = input for ISO1
  P2DIR |= BIT5;   // P2.5 = output for ISO1

  // Enable interrupts for input pins
  P2IE |= BIT1 | BIT4;

  // Set GPIO interrupt on edge
  P2IES |= BIT1 | BIT4;

  // Clear GPIO interrupt flags
  P2IFG &= ~(BIT1 | BIT4);

  /* The delay from ISO1 CAN to ISO2 CAN is ~150ns (under typical conditions) -
   * it's the time needed to receive the signal on one bus plus the time needed
   * to transmit the signal on the other bus - the total delay is equal to the
   * loop delay of one transceiver.
   *
   * A delay of 300ns needs at least a 3.333MHz clock to capture one count.
   * kDelayCount = period * freq
   *             = std::ceil(300ns * 16MHz) to achieve minimum delay of 300ns
   */

  // period = 16 MHz * kDelayCount
  TA0CCR0 = kDelayCount;
  TA1CCR0 = kDelayCount;

  // OUTMOD_7 = Reset/set output when the timer counts to TACCR1/TACCR0
  // CCIE = Interrupt when timer counts to TACCR1
  TA0CCTL1 = OUTMOD_7 | CCIE;
  TA1CCTL1 = OUTMOD_7 | CCIE;

  // LPM1 with interrupts enabled
  __bis_SR_register(LPM1_bits | GIE);
}

template <uint8_t Input, uint8_t Output>
void checkPins(volatile unsigned int& CCR1, volatile unsigned int& CTL,
               volatile unsigned int& CCTL1) {
  // If ISO input toggled
  if (P2IFG | Input) {
    // If toggled low
    if ((P2IN | Input) == 0) {
      // Set output to low immediately
      P2OUT |= Output;

      // Stop timer if it's running
      CTL = TASSEL_2 | MC_0;

      // Clear pending timer interrupt
      CCTL1 &= ~CCIFG;
    } else {
      // Else was toggled high. Start delay timer.

      // Initial counter register value
      CCR1 = 0;

      // Source Timer A from SMCLK (TASSEL_2), up mode (MC_1).
      // Up mode counts up to TACCR0. SLAU144J p. 358
      CTL = TASSEL_2 | MC_1;
    }

    // Clear interrupt flag
    P2IFG &= ~Input;
  }
}

void __interrupt_vec(PORT2_VECTOR) GPIO_ISR() {
  checkPins<BIT4, BIT5>(TA0CCR1, TA0CTL, TA0CCTL1);
  checkPins<BIT1, BIT0>(TA1CCR1, TA1CTL, TA1CCTL1);
}

// This will be called when timer counts to TA0CCR1
void __interrupt_vec(TIMER0_A1_VECTOR) TA0CCR1_ISR() {
  // Clear interrupt flag
  TA0CCTL1 &= ~CCIFG;

  // Set ISO1 output high
  P2OUT |= BIT5;

  // Stop timer
  TA0CTL = TASSEL_2 | MC_0;
}

// This will be called when timer counts to TA1CCR1
void __interrupt_vec(TIMER1_A1_VECTOR) TA1CCR1_ISR() {
  // Clear interrupt flag
  TA1CCTL1 &= ~CCIFG;

  // Set ISO2 output high
  P2OUT |= BIT0;

  // Stop timer
  TA1CTL = TASSEL_2 | MC_0;
}
