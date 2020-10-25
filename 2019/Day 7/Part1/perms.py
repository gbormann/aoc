def all_perms(elms):
    if len(elms) <= 1:
        yield elms
    else:
        for perm in all_perms(elms[1:]):
            for i in xrange(len(elms)):
                #nb elms[0:1] works in both string and list contexts
                yield perm[:i] + elms[0:1] + perm[i:]
