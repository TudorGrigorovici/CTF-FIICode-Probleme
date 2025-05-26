from PIL import Image

def getFlag(IMAGE_NAME):
    image = Image.open(IMAGE_NAME).convert("RGBA")
    pixels = image.load()

    width, height = image.size

    hint = ""

    for x in range(width):
        for y in range(height):
            r, g, b, a = pixels[x, y]
            if r == 0 and g == 0 and b == 0:
                hint += chr(a)

    print("HINT: ", hint)

    x, y = 380, 350
    flag = ""


    for i in range(31):
        r, g, b, a = pixels[x, y]
        direction = a
        flag += chr(b)

        if direction == 8 and y - 1 > 0:
            new_x = x
            new_y = y - 1

        if direction == 4 and y + 1 < height:
            new_x = x
            new_y = y + 1

        if direction == 2 and x - 1 > 0:
            new_x = x - 1
            new_y = y

        if direction == 1 and x + 1 < width:
            new_x = x + 1
            new_y = y

        x = new_x
        y = new_y

    print("FLAG: ", flag[::-1])

if __name__ == "__main__":
    getFlag("wood_man.png")