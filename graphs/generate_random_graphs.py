import argparse
import csv
import hashlib
import random
from collections import deque
from pathlib import Path


VERTEX_COUNTS = [12, 16, 20, 24]
AVERAGE_DEGREES = [4, 6, 8]
REPETITIONS = 100

DEFAULT_MASTER_SEED = 20260929


def derive_seed(master_seed: int, n: int, average_degree: int, replication: int) -> int:
    """
    derive a deterministic instance seed from the master seed and
    (n, average degree, replication).
    """

    data = (
        f"{master_seed}:"
        f"{n}:"
        f"{average_degree}:"
        f"{replication}"
    ).encode("utf-8")

    digest = hashlib.sha256(data).digest()

    return int.from_bytes(digest[:8], byteorder="little")


def generate_all_pairs(n: int):
    """
    generate all unordered vertex pairs.
    """

    return [
        (u, v)
        for u in range(n)
        for v in range(u + 1, n)
    ]


def sample_edges(n: int, m: int, rng: random.Random):
    """
    sample m distinct unordered vertex pairs uniformly
    without replacement from all possible pairs.
    """

    pairs = generate_all_pairs(n)

    return rng.sample(pairs, m)


def is_connected(n: int, edges):
    """
    test graph connectivity using bfs.
    """

    adjacency = [[] for _ in range(n)]

    for u, v in edges:
        adjacency[u].append(v)
        adjacency[v].append(u)

    visited = [False] * n

    queue = deque([0])
    visited[0] = True

    visited_count = 1

    while queue:
        u = queue.popleft()

        for v in adjacency[u]:
            if not visited[v]:
                visited[v] = True
                visited_count += 1
                queue.append(v)

    return visited_count == n


def select_terminals(n: int, rng: random.Random):
    """
    select source and sink uniformly without replacement.
    """

    source, sink = rng.sample(range(n), 2)

    return source, sink


def relabel_graph(edges, n: int, source: int, sink: int, rng: random.Random):
    """
    randomly relabel vertices so that:
        source -> 0
        sink   -> n-1

    remaining vertices receive a random permutation of labels
    1 ... n-2.
    """

    remaining_vertices = [
        v for v in range(n)
        if v != source and v != sink
    ]

    remaining_labels = list(range(1, n - 1))

    rng.shuffle(remaining_labels)

    labels = [None] * n

    labels[source] = 0
    labels[sink] = n - 1

    for vertex, label in zip(
        remaining_vertices,
        remaining_labels
    ):
        labels[vertex] = label

    relabeled_edges = []

    for u, v in edges:

        new_u = labels[u]
        new_v = labels[v]

        if new_u > new_v:
            new_u, new_v = new_v, new_u

        relabeled_edges.append((new_u, new_v))

    return relabeled_edges


def shuffle_edges(edges, rng: random.Random):
    """
    randomly permute edge order.
    """

    result = list(edges)

    rng.shuffle(result)

    return result


def canonical_graph_key(edges):
    """
    canonical representation used for duplicate detection.
    """

    sorted_edges = sorted(edges)

    return tuple(sorted_edges)


def sha256_file(path: Path):
    """
    calculate SHA-256 hash of a file.
    """

    sha256 = hashlib.sha256()

    with path.open("rb") as file:
        while True:
            chunk = file.read(1024 * 1024)

            if not chunk:
                break

            sha256.update(chunk)

    return sha256.hexdigest()


def shortest_path_distance(n: int, edges, source: int, sink: int):
    """
    calculate unweighted shortest-path distance.
    """

    adjacency = [[] for _ in range(n)]

    for u, v in edges:
        adjacency[u].append(v)
        adjacency[v].append(u)

    distance = [-1] * n

    queue = deque([source])
    distance[source] = 0

    while queue:
        u = queue.popleft()

        if u == sink:
            return distance[u]

        for v in adjacency[u]:
            if distance[v] == -1:
                distance[v] = distance[u] + 1
                queue.append(v)

    return -1


def degree_statistics(n: int, edges):
    """
    calculate vertex degree statistics.
    """

    degrees = [0] * n

    for u, v in edges:
        degrees[u] += 1
        degrees[v] += 1

    return (
        degrees,
        min(degrees),
        max(degrees)
    )


def write_graph(
    path: Path,
    n: int,
    m: int,
    source: int,
    sink: int,
    seed: int,
    attempts: int,
    edges
):
    """
    write one graph instance.
    """

    with path.open("w", encoding="utf-8") as file:

        file.write("# random connected G(n,m) instance\n")
        file.write(f"# n {n}\n")
        file.write(f"# m {m}\n")
        file.write(f"# source {source}\n")
        file.write(f"# sink {sink}\n")
        file.write(f"# seed {seed}\n")
        file.write(f"# attempts {attempts}\n")

        file.write(f"{n} {m}\n")

        for u, v in edges:
            file.write(f"{u} {v}\n")


def generate_instance(
    n: int,
    average_degree: int,
    replication: int,
    master_seed: int,
    accepted_graphs
):
    """
    generate one connected G(n,m) instance.
    """

    m = round(n * average_degree / 2)

    seed = derive_seed(
        master_seed,
        n,
        average_degree,
        replication
    )

    rng = random.Random(seed)

    attempts = 0

    while True:

        attempts += 1

        # generate G(n,m)
        edges = sample_edges(n, m, rng)

        # condition on connectivity
        if not is_connected(n, edges):
            continue

        # select terminals after graph generation
        source, sink = select_terminals(n, rng)

        # randomly relabel graph
        edges = relabel_graph(
            edges,
            n,
            source,
            sink,
            rng
        )

        # randomize edge order
        edges = shuffle_edges(edges, rng)

        # check duplicate
        key = canonical_graph_key(edges)

        if key in accepted_graphs:
            continue

        accepted_graphs.add(key)

        return (
            edges,
            seed,
            attempts
        )


def main():

    parser = argparse.ArgumentParser(
        description="Generate connected G(n,m) random networks."
    )

    parser.add_argument(
        "output_directory",
        help="directory for generated graph files"
    )

    parser.add_argument(
        "--seed",
        type=int,
        default=DEFAULT_MASTER_SEED,
        help="master random seed"
    )

    args = parser.parse_args()

    output_directory = Path(
        args.output_directory
    )

    output_directory.mkdir(
        parents=True,
        exist_ok=True
    )

    manifest_path = (
        output_directory / "manifest.csv"
    )

    with manifest_path.open(
        "w",
        newline="",
        encoding="utf-8"
    ) as manifest_file:

        writer = csv.writer(manifest_file)

        writer.writerow([
            "instance_id",
            "model",
            "n",
            "m",
            "avg_degree",
            "replication",
            "seed",
            "attempts",
            "source",
            "sink",
            "st_distance",
            "degree_s",
            "degree_t",
            "min_degree",
            "max_degree",
            "sha256"
        ])

        total_instances = 0

        for n in VERTEX_COUNTS:

            for average_degree in AVERAGE_DEGREES:

                accepted_graphs = set()

                m = round(
                    n * average_degree / 2
                )

                for replication in range(
                    1,
                    REPETITIONS + 1
                ):

                    (
                        edges,
                        seed,
                        attempts
                    ) = generate_instance(
                        n,
                        average_degree,
                        replication,
                        args.seed,
                        accepted_graphs
                    )

                    instance_id = (
                        f"Gnm_n{n}"
                        f"_d{average_degree:02d}"
                        f"_r{replication:03d}"
                    )

                    file_path = (
                        output_directory /
                        f"{instance_id}.txt"
                    )

                    # source and sink have already been
                    # relabeled to 0 and n-1
                    source = 0
                    sink = n - 1

                    write_graph(
                        file_path,
                        n,
                        m,
                        source,
                        sink,
                        seed,
                        attempts,
                        edges
                    )

                    (
                        degrees,
                        min_degree,
                        max_degree
                    ) = degree_statistics(
                        n,
                        edges
                    )

                    st_distance = (
                        shortest_path_distance(
                            n,
                            edges,
                            source,
                            sink
                        )
                    )

                    file_hash = sha256_file(
                        file_path
                    )

                    writer.writerow([
                        instance_id,
                        "Gnm-connected",
                        n,
                        m,
                        average_degree,
                        replication,
                        seed,
                        attempts,
                        source,
                        sink,
                        st_distance,
                        degrees[source],
                        degrees[sink],
                        min_degree,
                        max_degree,
                        file_hash
                    ])

                    total_instances += 1

                    print(
                        f"{instance_id} "
                        f"n={n} "
                        f"m={m} "
                        f"attempts={attempts}"
                    )

    print()
    print(
        f"generated {total_instances} connected "
        f"G(n,m) instances."
    )

    print(
        f"master seed: {args.seed}"
    )

    print(
        f"output directory: {output_directory}"
    )


if __name__ == "__main__":
    main()