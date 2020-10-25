
def sign(x1, x2):
    return 1 if x1 < x2 else (-1 if x1 > x2 else 0)

def step_accel(v, r):
    for k1 in xrange(0, len(r)):
        dv_k = [[], [], []]
        for i in xrange(0, 3):
            dv_k[i] = [(sign(r[k1][i], r[k2][i])) for k2 in xrange(0, len(r))]
        v[k1] = [(v[k1][i] + sum(dv_k[i])) for i in xrange(0, 3)]
    return v

def step_pos(r, v):
    for k in xrange(0, len(r)):
        r[k] = [(r[k][i] + v[k][i]) for i in xrange(0, 3)]
    return r

def rel_pos(r, r_COM):
    return [([(len(r) * r[k][i] - r_COM[i]) for i in xrange(0, 3)]) for k in xrange(0, len(r))]

def calc_pot_en(r):
    P = []
    for r_k in r:
        P.append(sum([(abs(r_k[i])) for i in xrange(0, 3)]))
    return P
        
def calc_kin_en(v):
    K = []
    for v_k in v:
        K.append(sum([(abs(v_k[i])) for i in xrange(0, 3)]))
    return K
        
r_moons = []
with open('../moon_pos.txt', mode='r') as Jmoons:
    for str_cur_pos in Jmoons:
        r_moons.append([(int(x)) for x in str_cur_pos.split()])

r_COM = [(sum([(r_moons[k][i]) for k in xrange(0, len(r_moons))])) for i in xrange(0,3)]
print "    COM = ", r_COM
print "_v__COM = _0_ for all t" #(uncomment COM line below for QED)

v_moons = [([0, 0, 0]) for k in xrange(0, len(r_moons))]

for cnt in xrange(0, 1000):
    v_moons = step_accel(v_moons, r_moons)
    r_moons = step_pos(r_moons, v_moons)

P = calc_pot_en(r_moons)
K = calc_kin_en(v_moons)
E=[(P[k]*K[k]) for k in xrange(0, len(r_moons))]
print P, K, E, sum(E)
