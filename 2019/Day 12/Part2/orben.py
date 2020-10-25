import copy

def lcm(a, b):
    from fractions import gcd
    return abs(a * b) // gcd(a, b)

def lcm_v(v):
    return reduce(lcm, v, 1)

def sign(x1, x2):
    return 1 if x1 < x2 else (-1 if x1 > x2 else 0)

def step_accel(v, r):
    for m in xrange(0, len(r)):
        dv_m = [(sign(r[m], r[n])) for n in xrange(0, len(r))]
        v[m] += sum(dv_m)
    return v

def step_pos(r, v):
    for m in xrange(0, len(r)):
        r[m] += v[m]
    return r

r_moons = []
with open('../moon_pos.txt', mode='r') as Jmoons:
    for str_cur_pos in Jmoons:
        cur_pos = [(int(x)) for x in str_cur_pos.split()]
        if len(r_moons) == 0:
            r_moons.extend([([]) for i in xrange(0, len(cur_pos))])
        for i in xrange(0, len(cur_pos)):
            r_moons[i].append(cur_pos[i])

r_moons0 = copy.deepcopy(r_moons)
v_moons = [([(0) for k in xrange(0, len(r_moons[i]))]) for i in xrange(0, len(r_moons))]
v_moons0 = [([(0) for k in xrange(0, len(r_moons[i]))]) for i in xrange(0, len(r_moons))]

cnt = [(long(1)) for k in xrange(0, len(r_moons))]
for k in xrange(0, len(r_moons)):
    v_moons[k] = step_accel(v_moons[k], r_moons[k])
    r_moons[k] = step_pos(r_moons[k], v_moons[k])
    while r_moons[k] != r_moons0[k] or v_moons[k] != v_moons0[k]:
        v_moons[k] = step_accel(v_moons[k], r_moons[k])
        r_moons[k] = step_pos(r_moons[k], v_moons[k])
        cnt[k] += 1

print r_moons0, r_moons, v_moons0, v_moons, cnt
print "Universe Periodicity: ", lcm_v(cnt)
