#!/usr/bin/python

from perms import *

def f(x, a, b):
    return a * x + b

def power(a, b, P):
    return f(f(f(f(f(0, a[P[0]], b[P[0]]), a[P[1]], b[P[1]]), a[P[2]], b[P[2]]), a[P[3]], b[P[3]]), a[P[4]], b[P[4]])

alpha=[3, 5, 16, 9, 3]
beta=[9, 0, 44, 39, 6]
phi=[0, 1, 2, 3, 4]

tunings = [([power(alpha, beta, Pphi_k), Pphi_k]) for Pphi_k in all_perms(phi)]
optimum = max(tunings)

print "Optimal power and tuning:",optimum
