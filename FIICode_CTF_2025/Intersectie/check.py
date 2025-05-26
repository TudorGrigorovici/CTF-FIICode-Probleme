with open('result.txt', 'r') as f:
    suma_totala = 0
    k = 0
    total_lines = f.readlines()
    for line in total_lines:
        print(f"\rProcessing line: {k+1}/{len(total_lines)}", end='', flush=True)
        line = line.strip()
        line = line.replace('{', '')
        line = line.replace('}', '')
        elems = line.split()
        for elem in elems:
            if ',' not in elem:
                int_item = int(elem)
                suma_locala = int(int_item)
            else:
                items = elem.split(',')
                int_items = list(map(int, items))
                suma_locala = sum(int_items)
            suma_totala += suma_locala
        k += 1
    print()
    print(f"Suma Totala: {suma_totala}")
