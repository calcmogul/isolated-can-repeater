// Copyright (c) Tyler Veness 2016. All Rights Reserved.

#include <msp430g2553.h>
#include <stdint.h>

const uint16_t kDelayCount = 1600;

// 0 = low; 1 = high
uint8_t prevInputPins = 0;
uint8_t currentInputPins = 0;

void checkPins(const uint8_t input, const uint8_t output,
               volatile unsigned int* ACCR0, volatile unsigned int* CCTL1);

/*
 * Software loop is used instead of GPIO interrupts on edges because a rising
 * and falling edge can both occur while processing another interrupt. To avoid
 * that ambiguity, the edges are determined in the software loop at the rate
 * they can be processed.
 */
int main() {
  // Stop watchdog
  WDTCTL = WDTPW | WDTHOLD;

  // Set clock to 1 MHz
  DCOCTL = 0;
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL = CALDCO_16MHZ;

  // Configure P2.0, P2.1, P2.4, and P2.5 for GPIO
  P2SEL &= ~(BIT0 | BIT1 | BIT4 | BIT5);

  P2DIR |= BIT0;   // P2.0 = output for ISO2
  P2DIR &= ~BIT1;  // P2.1 = input for ISO2
  P2DIR &= ~BIT4;  // P2.4 = input for ISO1
  P2DIR |= BIT5;   // P2.5 = output for ISO1

  // Set ISO1 output high
  P2OUT |= BIT5;

  // Set ISO2 output high
  P2OUT |= BIT0;

  /* The delay from ISO1 CAN to ISO2 CAN is ~150ns (under typical conditions) -
   * it's the time needed to receive the signal on one bus plus the time needed
   * to transmit the signal on the other bus - the total delay is equal to the
   * loop delay of one transceiver.
   *
   * A delay of 300ns needs at least a 3.333MHz clock to capture one count.
   * kDelayCount = period * freq
   *             = std::ceil(300ns * 16MHz) to achieve minimum delay of 300ns
   */

  // Stop timer
  TA0CCR0 = 0;
  TA1CCR0 = 0;

  // CCIE = Interrupt when timer counts to TACCR0
  TA0CCTL0 = CCIE;
  TA1CCTL0 = CCIE;

  // Source Timer A from SMCLK (TASSEL_2), up mode (MC_1).
  // Up mode counts up to TACCR0. SLAU144J p. 358
  TA0CTL = TASSEL_2 | MC_1;
  TA1CTL = TASSEL_2 | MC_1;

  __bis_SR_register(GIE);

  while (1) {
    currentInputPins = P2IN;
    checkPins(BIT4, BIT5, &TA0CCR0, &TA0CCTL1);
    checkPins(BIT1, BIT0, &TA1CCR0, &TA1CCTL1);
    prevInputPins = currentInputPins;
  }
}

void checkPins(const uint8_t input, const uint8_t output,
               volatile unsigned int* ACCR0, volatile unsigned int* CCTL1) {
  // If input toggled low
  if ((prevInputPins & input) != 0 && (currentInputPins & input) == 0) {
    // Set output to low immediately
    P2OUT &= ~output;

    /* Stop timer if it's running. This is guaranteed to not be reordered
     * after clearing the interrupt flag by the compiler due to the registers
     * being marked as volatile.
     */
    *ACCR0 = 0;

    // Clear potential pending timer interrupt
    *CCTL1 &= ~CCIFG;
  } else if ((prevInputPins & input) == 0 && (currentInputPins & input) != 0) {
    // Else was toggled high. Start delay timer.

    // Start timer by initializing counter register value
    *ACCR0 = kDelayCount;
  }
}

// This will be called when timer counts to TA0CCR0
void __interrupt_vec(TIMER0_A0_VECTOR) TA0CCR0_ISR() {
  // Set ISO1 output high
  P2OUT |= BIT5;

  // Stop timer
  TA0CCR0 = 0;

  /* Clear interrupt flag so any interrupts generated in the ISR before stopping
   * the timer aren't serviced
   */
  TA0CCTL0 &= ~CCIFG;
}

// This will be called when timer counts to TA1CCR0
void __interrupt_vec(TIMER1_A0_VECTOR) TA1CCR0_ISR() {
  // Set ISO2 output high
  P2OUT |= BIT0;

  // Stop timer
  TA1CCR0 = 0;

  /* Clear interrupt flag so any interrupts generated in the ISR before stopping
   * the timer aren't serviced
   */
  TA1CCTL0 &= ~CCIFG;
}
