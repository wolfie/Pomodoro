# Pomodoro

A [Pomodoro](https://en.wikipedia.org/wiki/Pomodoro_Technique) timer for
[Particle Proton](https://www.particle.io/products/hardware/photon-wifi/) and
the [Internet Button](https://store.particle.io/products/internet-button).

## Operation

 * Press Button3 (down) to start a countdown indicated by red LEDs dimming one
   by one.
 * Once the countdown is done and all LEDs have turned off, you get a sound
   queue, a green led ("checkmark") for each cycle and a break.
 * The break is indicated by all-white LEDs, slowly dimming. There's another
   sound queue once the break is over
 * Once the break is over, you have to press Button3 again to start the next
   cycle.
 * The fourth and last cycle is a longer break, and everything starts again.

You can reset the counts at any time by keeping the Button1 (up) pressed down
for a while.

You can also pause (and resume) the countdowns by pressing the Button3.

## Offline-by-Default

Since this Pomodoro-timer does not need internet, the button starts
[offline](https://docs.particle.io/reference/firmware/core/#manual-mode). To put
it into online mode (e.g. for firmware updates without 
[safe mode](https://docs.particle.io/guide/getting-started/modes/core/#safe-mode))
click the Button1 (up) three times quickly.

The LEDs will flash up first as magenta while the system connects to the
internet. Once it's connected, it'll change to a slowly pulsing yellow light.
