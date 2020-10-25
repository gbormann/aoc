def f(x):
    if x[0] > 6:
        return f([x[0] / 3 - 2, x[1] + (x[0] / 3 - 2)]);
    return [x[0], x[1]];

total = 0;
with open('../modulesmass.txt', mode='r') as modmasses:
    for modmass in modmasses:
        total += f([int(modmass), 0])[1];

print(total);
