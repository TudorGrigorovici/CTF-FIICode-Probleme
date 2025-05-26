import sys
import random

possible_values = [
    "20",
    "31",
    "44",
    "58",
    "63",
    "69",
    "71"
]

def gen_matrix(N):
    matA = []
    for i in range(N):
        line_elems = []
        for j in range(N):
            nr_elem = random.randint(0, 6)
            line_elem = [random.choice(possible_values) for _ in range(nr_elem)]
            line_elems += [line_elem]            
        matA += [line_elems]
    return matA

def main():
    if len(sys.argv) != 2:
        print("Usage: python gen.py N")
        print("N - numeric value")
        return

    try:
        N = int(sys.argv[1])
    except ValueError:
       print("N - must be numeric")
       return

    matA = gen_matrix(N)
    matB = gen_matrix(N)

    with open('mat.txt', 'w') as f:
        for ln in matA:
            text_elems = []
            for el in ln:
                if len(el) == 0:
                    el = "{}"
                else:
                    el = '{' + ','.join(el) + '}'
                text_elems += [el]
            f.write(' '.join(text_elems) + '\n')

        f.write('\n')

        for ln in matB:
            text_elems = []
            for el in ln:
                if len(el) == 0:
                    el = "{}"
                else:
                    el = '{' + ','.join(el) + '}'
                text_elems += [el]
            f.write(' '.join(text_elems) + '\n')

        f.write('\n')

        

if __name__ == "__main__":
    main()