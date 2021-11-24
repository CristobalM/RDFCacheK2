
with open("triples.txt", "w") as f:
    start = 10_000_000
    count = 10_000_000
    for i in range(start, start+count):
        f.write(f"{i},{i},{i}\n")