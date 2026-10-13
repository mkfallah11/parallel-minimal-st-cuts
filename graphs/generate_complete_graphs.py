from pathlib import Path


output_dir = Path("./graphs/complete/")


for n in range(101, 201):
    edge_count = n * (n - 1) // 2

    filename = output_dir / f"K{n}.txt"

    with filename.open("w") as file:
        file.write(f"{n} {edge_count}\n")

        for u in range(n):
            for v in range(u + 1, n):
                file.write(f"{u} {v}\n")

    minimal_cuts = 2 ** (n - 1) - 1

    print(
        f"K{n}: "
        f"nodes={n}, "
        f"edges={edge_count}, "
        f"minimal_cuts={minimal_cuts}"
    )