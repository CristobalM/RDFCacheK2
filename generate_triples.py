
with open("triples.txt", "w") as f:
    for i in range(1, 10_000_000):
        f.write(f"{i},{i},{i}\n")