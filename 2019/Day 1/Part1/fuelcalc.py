total = 0;
with open('../modulesmass.txt', mode='r') as modmasses:
    for modmass in modmasses:
        total += int(modmass) / 3 - 2;

print(total);
