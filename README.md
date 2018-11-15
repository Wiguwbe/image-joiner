# image-joiner
Somekind of a color and gray-scale images joiner

---

#### background

When using `pdfimages` from linux (to extract images from pdf's), it, some times, for a single image, *extracts* two images, one which seems to be the original, same colors, same places, and another one which is mostly black and/or white. It happened sometimes to me, not sure if it happens that much...

Anyway, this time I get the *original* image but with a lot of black color (mostly on the background) and another one only black and white (and gray). So after "analyzing" those images I realized those were actually *that* image.

Firstly I thought the second image were some kind of transparency map, I was probably wrong... Anyway, if you "merged" them together you'd get the original image.

This is the *tool* I made to join those images, I have no idea if there is any similar tool, mostly because I had no idea what to *google* for...

---

#### simplified code

Being `pixel` a pixel of the *color image* and `gray` a pixel of the *gray-scaled image*,

For every pixel: `pixel[n] = pixel[n] | ~gray[n];` or "a pixel binary OR'ed with the binary NEGative of the correspondent gray-scaled pixel"

---

###### anyway...

Have a day
