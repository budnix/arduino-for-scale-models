# Arduino Scripts for Ford Mustang

![Lights preview](ford-mustang-gt.gif)

(https://www.youtube.com/watch?v=4ME1MtxgZ0U)

The scripts allows:
 * Turn on/off front and rear days lights.
 * Turn on/off front and rear indicators. Rear indicators are animated by using three LEDs.
 * Turn on/off the emergency lights. If enabled the rear indicators are not animated (as the real car).

TODO:
 * Implement the stop lights.

### PINs

| PIN |      Description           |
|-----|:--------------------------:|
| 2   | IR Sensor                  |
| 3   | Rear left indicator (oo0)  |
| 4   | Front left indicator       |
| 5   | Rear left indicator (o0o)  |
| 6   | Rear left indicator (0oo)  |
| 7   | Front right indicator      |
| 8   | Front days light           |
| 9   | Rear right indicator (0oo) |
| 10  | Rear right indicator (o0o) |
| 11  | Rear right indicator (oo0) |
| 12  | Front days light           |

\* 0 - describes the led position in the rear lights (important for indicator animation)

### Circuit diagram
 ![Circuit diagram](circuit.png)
