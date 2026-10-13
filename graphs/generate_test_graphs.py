from pathlib import Path
import random


output_dir = Path("./test_graphs")
output_dir.mkdir(parents=True, exist_ok=True)


def write_graph(filename, node_count, edges):
    with filename.open("w") as file:
        file.write(f"{node_count} {len(edges)}\n")

        for u, v in edges:
            file.write(f"{u} {v}\n")


def generate_path(n):
    return [(u, u + 1) for u in range(n - 1)]


def generate_cycle(n):
    edges = [(u, u + 1) for u in range(n - 1)]
    edges.append((n - 1, 0))
    return edges


def generate_star(n):
    return [(0, v) for v in range(1, n)]


# def generate_complete(n):
#     edges = []

#     for u in range(n):
#         for v in range(u + 1, n):
#             edges.append((u, v))

#     return edges


def generate_random(n, probability):
    edges = []

    for u in range(n):
        for v in range(u + 1, n):
            if random.random() < probability:
                edges.append((u, v))

    return edges


random.seed(42)


for n in range(4, 21):

    path_edges = generate_path(n)
    write_graph(
        output_dir / f"path{n}.txt",
        n,
        path_edges
    )


    cycle_edges = generate_cycle(n)
    write_graph(
        output_dir / f"cycle{n}.txt",
        n,
        cycle_edges
    )


    star_edges = generate_star(n)
    write_graph(
        output_dir / f"star{n}.txt",
        n,
        star_edges
    )


    # complete_edges = generate_complete(n)
    # write_graph(
    #     output_dir / f"complete{n}.txt",
    #     n,
    #     complete_edges
    # )


    for probability in [0.2, 0.5, 0.8]:

        random_edges = generate_random(
            n,
            probability
        )

        write_graph(
            output_dir /
            f"random{n}_{int(probability * 100)}.txt",
            n,
            random_edges
        )


    print(
        f"n={n}: "
        f"path={len(path_edges)}, "
        f"cycle={len(cycle_edges)}, "
        f"star={len(star_edges)}, "
        # f"complete={len(complete_edges)}"
    )