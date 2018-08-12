# Pomodoro

A [Pomodoro](https://en.wikipedia.org/wiki/Pomodoro_Technique) timer for
[Particle Proton](https://www.particle.io/products/hardware/photon-wifi/) and
the [Internet Button](https://store.particle.io/products/internet-button).

## Operation

 * Press Button3 (down) to start a countdown indicated by red leds dimming one
   by one.
 * Once the countdown is done and all leds have turned off, you get a sound
   queue, a green led ("checkmark") for each cycle and a break.
 * The break is indicated by all-white leds, slowly dimming. There's another
   sound queue once the break is over
 * Once the break is over, you have to press Button3 again to start the next
   cycle.
 * The fourth and last cycle is a longer break, and everything starts again.

You can reset the counts at any time by keeping the Button1 (up) pressed down
for a while.
