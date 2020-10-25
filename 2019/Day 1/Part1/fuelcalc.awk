BEGIN{
    ac=0
}
{
    ac += int($1 / 3) - 2
}
END{
    print ac
}

