
for i6 in xrange(1,7):
    d6_b = i6 == 1;
    d6_e = i6 == 6;
    for i5 in xrange((5 if d6_b else i6), (5 + 1) if d6_e else 10):
        d5_b = d6_b and i5 == 5;
        has_step56 = i5 != i6;
        has_flat_pair56 = i5 == i6;
        for i4 in xrange((6 if d5_b else i5), 10):
            has_step45 = i4 != i5;
            has_flat_pair45 = i4 == i5;
            has_isolated_pair56 = has_flat_pair56 and has_step45;
            for i3 in xrange(i4, 10):
                has_step34 = i3 != i4;
                has_flat_pair34 = i3 == i4;
                has_isolated_pair45 = has_step56 and has_flat_pair45 and has_step34;
                for i2 in xrange(i3, 10):
                    has_step23 = i2 != i3;
                    has_flat_pair23 = i2 == i3;
                    has_isolated_pair34 = has_step45 and has_flat_pair34 and has_step23;
                    for i1 in xrange(i2, 10):
                        has_isolated_pair23 = has_step34 and has_flat_pair23 and i1 != i2;
                        has_isolated_pair = (has_isolated_pair56 or has_isolated_pair45
                                        or has_isolated_pair34 or has_isolated_pair23
                                        or (has_step23 and i1 == i2));
                        if has_isolated_pair:
                            n=10 * (10 * (10 * (10 * (10 * i6 + i5) + i4) + i3) + i2) + i1;
                            print n;
