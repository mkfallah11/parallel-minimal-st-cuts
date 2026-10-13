from pathlib import Path


output_dir = Path("./graphs/mesh/")

m_values = [4, 5]
n_values = [4, 5]

for m in m_values:
    for n in n_values:
        node_count = m * n
        edge_count = m * (n - 1) + n * (m - 1)

        filename = output_dir / f"M{m}x{n}.txt"

        filename.parent.mkdir(parents=True, exist_ok=True)

        with filename.open("w") as file:
            file.write(f"{node_count} {edge_count}\n")

            for row in range(m):
                for col in range(n):
                    u = row * n + col

                    # horizontal edge
                    if col + 1 < n:
                        v = row * n + (col + 1)
                        file.write(f"{u} {v}\n")

                    # vertical edge
                    if row + 1 < m:
                        v = (row + 1) * n + col
                        file.write(f"{u} {v}\n")

        print(
            f"M{m}x{n}: "
            f"nodes={node_count}, "
            f"edges={edge_count}"
        )