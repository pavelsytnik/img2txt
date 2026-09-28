# img2txt

**img2txt** is a command-line tool that can convert PNG and JPEG images into a
text-based art.

## Usage


```
Usage: img2txt [OPTIONS] <IMG_FILENAME>

Options:
  -w, --width=INT              Text art width in characters
  -c, --contrast=DOUBLE        Text art contrast
  -t, --terminal               Print art to the terminal
  -o, --output=STRING          Output filename
  -r, --ramp=STRING            Character ramp. Available values are 'ascii' and 'block'
      --no-weighted-grayscale  Disable weighted grayscale
      --no-box-filter          Disable box filtering
```

There are two character ramps you can use for drawing:

- ASCII (`@%#*+=-:. `)
- Block (`█▓▒░ `)

Use `--contrast=-1.0` to invert the colors.

### Defaults

| Option   | Value |
|:---------|:------|
| width    | 60    |
| contrast | 1.0   |
| ramp     | ascii |

By default, if you omit `--output`, the generated art is saved next to the
input image with `.txt` appended to its filename. Thus, `img2txt pic.png`
creates a file named `pic.png.txt`.

## Examples

`img2txt -w80 firefox.png`
![Firefox Logo](/images/firefox.png)

`img2txt -w64 soyjak.png`
![Soyjak](/images/soyjak.png)

`img2txt -w200 -rblock artoria-pendragon.jpg`
![Artoria Pendragon](/images/artoria-pendragon.png)

## TODO

- [ ] Add Braille mode
- [ ] Add animated output to the terminal for GIF
- [ ] Add colorful ouput to the terminal
